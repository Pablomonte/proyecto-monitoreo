#pragma once
#include "IActuator.h"
#include "MediatorConfig.h"
#include "Rule.h"
#include "SensorKey.h"
#include <math.h>

/**
 * ControlMediator — núcleo de control del sistema.
 *
 * Responsabilidades:
 *   1. Mantiene un state-store de lecturas recientes (float por SensorKey).
 *   2. Evalúa todas las reglas activas con cada nueva lectura.
 *   3. Despacha ActuatorCommand a los actuadores registrados con
 *      arbitraje de prioridad.
 *   4. Gestiona auto-off por durationMs (via tick()).
 *
 * Política de prioridad:
 *   - Mayor prioridad siempre reemplaza la activa.
 *   - Igual prioridad: último en escribir gana.
 *   - Comandos manuales (priority=3) con durationMs=0 persisten hasta
 *     el próximo comando manual.
 *
 * Uso típico:
 *   ControlMediator mediator;
 *   mediator.registerActuator(&myRelay);
 *   mediator.addRule(rule);
 *   // en loop():
 *   mediator.onSensorReading(r);
 *   mediator.tick();
 */
class ControlMediator {
public:
    ControlMediator() {
        _clear();
    }

    // ── Registro ──────────────────────────────────────────────────────────

    /**
     * Registrar un actuador. Debe llamarse en setup() antes de addRule().
     * El actuador debe permanecer válido durante toda la vida del mediador.
     */
    void registerActuator(IActuator* a) {
        if (!a || _actuatorCount >= ACTUATOR_MAX) return;
        _actuators[_actuatorCount++] = a;
    }

    /**
     * Agregar una expresión al pool. Retorna el índice asignado.
     * Retorna 0xFF si el pool está lleno.
     */
    uint8_t addExpr(const RuleExpr& expr) {
        if (_exprCount >= RULE_EXPR_POOL) return 0xFF;
        _exprPool[_exprCount] = expr;
        return _exprCount++;
    }

    /**
     * Agregar una regla. La expresión raíz (rootExprIdx) debe estar
     * previamente registrada con addExpr().
     */
    void addRule(const Rule& rule) {
        if (_ruleCount >= RULE_MAX) return;
        _rules[_ruleCount++] = rule;
    }

    /** Limpiar todas las reglas y expresiones (para reload). */
    void clearRules() {
        _exprCount = 0;
        _ruleCount = 0;
    }

    // ── Entradas ──────────────────────────────────────────────────────────

    /**
     * Recibir una lectura de sensor (local o remota via ESP-NOW).
     * Actualiza el state-store y evalúa todas las reglas.
     */
    void onSensorReading(const SensorReading& r) {
        uint8_t idx = _storeIndex(r.key);
        _state[idx]      = r.value;
        _stateValid[idx] = true;
        evaluateAllRules();
    }

    /**
     * Recibir un comando manual (desde la API web o interfaz de usuario).
     * Priority = 3 (manual), siempre tiene preferencia sobre reglas y timers.
     */
    void onManualCommand(const ActuatorCommand& cmd) {
        dispatch(cmd);
    }

    /**
     * Recibir disparo de timer (desde TimerSource).
     * Priority = 1 (timer, menor prioridad).
     */
    void onTimerTick(uint8_t actuatorId, bool state, uint32_t durationMs) {
        ActuatorCommand cmd;
        cmd.actuatorId = actuatorId;
        cmd.state      = state;
        cmd.durationMs = durationMs;
        cmd.priority   = 1;
        dispatch(cmd);
    }

    // ── Loop ──────────────────────────────────────────────────────────────

    /**
     * Procesar auto-off y delegar tick() a actuadores.
     * DEBE llamarse desde loop().
     */
    void tick() {
        uint32_t now = millis();
        for (uint8_t i = 0; i < _actuatorCount; i++) {
            if (_actuators[i]) {
                _actuators[i]->tick();
            }
            // Auto-off gestionado por el mediador (respaldo al tick del actuador)
            if (_activeUntil[i] > 0 && now >= _activeUntil[i]) {
                ActuatorCommand off;
                off.actuatorId = _actuators[i]->getId();
                off.state      = false;
                off.durationMs = 0;
                off.priority   = _active[i].priority;
                _activeUntil[i] = 0;
                _executeOn(i, off);
            }
        }
    }

    // ── Consultas ─────────────────────────────────────────────────────────

    /** Número de actuadores registrados */
    uint8_t getActuatorCount() const { return _actuatorCount; }

    /** Acceso por índice (para serializar en API) */
    IActuator* getActuator(uint8_t idx) const {
        if (idx >= _actuatorCount) return nullptr;
        return _actuators[idx];
    }

    /** Número de reglas cargadas */
    uint8_t getRuleCount() const { return _ruleCount; }

private:
    // ── State-store ───────────────────────────────────────────────────────
    float _state[STATE_STORE_SIZE];
    bool  _stateValid[STATE_STORE_SIZE];

    // ── Pool de expresiones y reglas ──────────────────────────────────────
    RuleExpr _exprPool[RULE_EXPR_POOL];
    uint8_t  _exprCount;

    Rule    _rules[RULE_MAX];
    uint8_t _ruleCount;

    // ── Actuadores ────────────────────────────────────────────────────────
    IActuator* _actuators[ACTUATOR_MAX];
    uint8_t    _actuatorCount;

    // ── Estado activo por actuador ────────────────────────────────────────
    ActuatorCommand _active[ACTUATOR_MAX];
    uint32_t        _activeUntil[ACTUATOR_MAX];

    // ── Inicialización ────────────────────────────────────────────────────
    void _clear() {
        for (uint16_t i = 0; i < STATE_STORE_SIZE; i++) {
            _state[i]      = 0.0f;
            _stateValid[i] = false;
        }
        _exprCount    = 0;
        _ruleCount    = 0;
        _actuatorCount = 0;
        for (uint8_t i = 0; i < ACTUATOR_MAX; i++) {
            _actuators[i]  = nullptr;
            _activeUntil[i] = 0;
            _active[i]     = {0, false, 0, 0};
        }
    }

    // ── Índice en el state-store ──────────────────────────────────────────
    static uint8_t _storeIndex(const SensorKey& key) {
        return (uint8_t)(key.toU32() % STATE_STORE_SIZE);
    }

    // ── Evaluación de árbol de expresiones ────────────────────────────────
    bool evalExpr(uint8_t idx) const {
        if (idx >= _exprCount) return false;
        const RuleExpr& e = _exprPool[idx];

        switch (e.type) {
            case ExprType::LEAF: {
                uint8_t si = _storeIndex(e.cond.key);
                if (!_stateValid[si]) return false;
                float v = _state[si];
                switch (e.cond.op) {
                    case CondOp::GT:  return v >  e.cond.threshold;
                    case CondOp::LT:  return v <  e.cond.threshold;
                    case CondOp::EQ:  return fabsf(v - e.cond.threshold) < 1e-4f;
                    case CondOp::GTE: return v >= e.cond.threshold;
                    case CondOp::LTE: return v <= e.cond.threshold;
                }
                return false;
            }
            case ExprType::AND:
                return evalExpr(e.leftIdx) && evalExpr(e.rightIdx);
            case ExprType::OR:
                return evalExpr(e.leftIdx) || evalExpr(e.rightIdx);
        }
        return false;
    }

    // ── Evaluación de todas las reglas ─────────────────────────────────────
    void evaluateAllRules() {
        for (uint8_t i = 0; i < _ruleCount; i++) {
            const Rule& r = _rules[i];
            bool triggered = evalExpr(r.rootExprIdx);
            if (triggered) {
                ActuatorCommand cmd;
                cmd.actuatorId = r.actuatorId;
                cmd.state      = r.triggerState;
                cmd.durationMs = r.durationMs;
                cmd.priority   = r.priority;
                dispatch(cmd);
            } else {
                // Si la regla ya no se cumple y tiene durationMs=0, apagar
                int8_t ai = _findActuatorIndex(r.actuatorId);
                if (ai >= 0 && _active[ai].priority == r.priority
                    && r.durationMs == 0 && _actuators[ai]->getState()) {
                    ActuatorCommand off;
                    off.actuatorId = r.actuatorId;
                    off.state      = false;
                    off.durationMs = 0;
                    off.priority   = r.priority;
                    dispatch(off);
                }
            }
        }
    }

    // ── Despacho con arbitraje de prioridad ────────────────────────────────
    void dispatch(const ActuatorCommand& cmd) {
        int8_t ai = _findActuatorIndex(cmd.actuatorId);
        if (ai < 0) return;

        // Aceptar si: mayor prioridad, o igual prioridad (último gana)
        if (cmd.priority >= _active[ai].priority) {
            _active[ai] = cmd;
            if (cmd.state && cmd.durationMs > 0) {
                _activeUntil[ai] = millis() + cmd.durationMs;
            } else if (!cmd.state) {
                _activeUntil[ai] = 0;
            }
            _executeOn(ai, cmd);
        }
    }

    void _executeOn(uint8_t actuatorIndex, const ActuatorCommand& cmd) {
        if (_actuators[actuatorIndex]) {
            _actuators[actuatorIndex]->execute(cmd);
        }
    }

    int8_t _findActuatorIndex(uint8_t actuatorId) const {
        for (uint8_t i = 0; i < _actuatorCount; i++) {
            if (_actuators[i] && _actuators[i]->getId() == actuatorId) {
                return (int8_t)i;
            }
        }
        return -1;
    }
};
