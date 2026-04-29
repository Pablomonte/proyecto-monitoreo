#pragma once
#include "IActuator.h"
#include "MediatorConfig.h"
#include "Rule.h"
#include "SensorKey.h"
#include <math.h>

/**
 * ControlMediator — central control arbiter.
 *
 * State-store: flat float array indexed by (key.toU32() % STATE_STORE_SIZE).
 * Each slot also tracks the last seen counter to reject stale readings.
 *
 * onSensorReading(): only updates state if reading.counter > stored counter
 *                    — guarantees origin consistency without wall-clock time.
 *
 * Priority arbitration: higher priority always wins; equal priority = last writer.
 */
class ControlMediator {
public:
    ControlMediator() { _clear(); }

    // ── Registration ──────────────────────────────────────────────────────
    void registerActuator(IActuator* a) {
        if (!a || _actuatorCount >= ACTUATOR_MAX) return;
        _actuators[_actuatorCount++] = a;
    }

    uint8_t addExpr(const RuleExpr& expr) {
        if (_exprCount >= RULE_EXPR_POOL) return 0xFF;
        _exprPool[_exprCount] = expr;
        return _exprCount++;
    }

    void addRule(const Rule& rule) {
        if (_ruleCount >= RULE_MAX) return;
        _rules[_ruleCount++] = rule;
    }

    void clearRules() { _exprCount = 0; _ruleCount = 0; }

    // ── Inputs ────────────────────────────────────────────────────────────

    /**
     * Receive a sensor reading (local or remote via ESP-NOW).
     * Rejected if reading.counter <= the stored counter for this slot
     * (prevents processing stale or out-of-order readings).
     */
    void onSensorReading(const SensorReading& r) {
        uint8_t idx = _storeIndex(r.key);
        if (_stateValid[idx] && r.counter <= _stateCounter[idx]) {
            return;  // stale — discard
        }
        _state[idx]        = r.value;
        _stateCounter[idx] = r.counter;
        _stateValid[idx]   = true;
        evaluateAllRules();
    }

    /** Manual command (priority=3 by convention). */
    void onManualCommand(const ActuatorCommand& cmd) { dispatch(cmd); }

    /** Timer-sourced command (priority=1 by convention). */
    void onTimerTick(uint8_t actuatorId, bool state, uint32_t durationMs) {
        ActuatorCommand cmd{actuatorId, state, durationMs, 1};
        dispatch(cmd);
    }

    // ── Loop ──────────────────────────────────────────────────────────────
    /**
     * Call from loop().  Clears mediator priority tracking when
     * the actuator's hardware timer has already fired its auto-off
     * (GpioActuator uses esp_timer internally and does not need tick()).
     */
    void tick() {
        uint32_t now = millis();
        for (uint8_t i = 0; i < _actuatorCount; i++) {
            // delegated tick (RelayModule2CH or other non-esp_timer actuators)
            if (_actuators[i]) _actuators[i]->tick();

            // clear mediator priority slot when duration has elapsed
            if (_activeUntil[i] > 0 && now >= _activeUntil[i]) {
                _activeUntil[i]       = 0;
                _active[i].priority   = 0;   // reset so next rule can fire
            }
        }
    }

    // ── Queries ───────────────────────────────────────────────────────────
    uint8_t    getActuatorCount() const { return _actuatorCount; }
    IActuator* getActuator(uint8_t idx) const {
        return idx < _actuatorCount ? _actuators[idx] : nullptr;
    }
    uint8_t getRuleCount() const { return _ruleCount; }

private:
    float    _state[STATE_STORE_SIZE];
    uint32_t _stateCounter[STATE_STORE_SIZE];   // per-slot monotonic counter
    bool     _stateValid[STATE_STORE_SIZE];

    RuleExpr _exprPool[RULE_EXPR_POOL];
    uint8_t  _exprCount;

    Rule    _rules[RULE_MAX];
    uint8_t _ruleCount;

    IActuator*      _actuators[ACTUATOR_MAX];
    uint8_t         _actuatorCount;
    ActuatorCommand _active[ACTUATOR_MAX];
    uint32_t        _activeUntil[ACTUATOR_MAX];

    void _clear() {
        for (uint16_t i = 0; i < STATE_STORE_SIZE; i++) {
            _state[i]        = 0.0f;
            _stateCounter[i] = 0;
            _stateValid[i]   = false;
        }
        _exprCount = _ruleCount = _actuatorCount = 0;
        for (uint8_t i = 0; i < ACTUATOR_MAX; i++) {
            _actuators[i]   = nullptr;
            _activeUntil[i] = 0;
            _active[i]      = {0, false, 0, 0};
        }
    }

    static uint8_t _storeIndex(const SensorKey& key) {
        return (uint8_t)(key.toU32() % STATE_STORE_SIZE);
    }

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
            case ExprType::AND: return evalExpr(e.leftIdx) && evalExpr(e.rightIdx);
            case ExprType::OR:  return evalExpr(e.leftIdx) || evalExpr(e.rightIdx);
        }
        return false;
    }

    void evaluateAllRules() {
        for (uint8_t i = 0; i < _ruleCount; i++) {
            const Rule& r = _rules[i];
            if (evalExpr(r.rootExprIdx)) {
                ActuatorCommand cmd{r.actuatorId, r.triggerState, r.durationMs, r.priority};
                dispatch(cmd);
            } else {
                int8_t ai = _findActuatorIndex(r.actuatorId);
                if (ai >= 0 && _active[ai].priority == r.priority
                    && r.durationMs == 0 && _actuators[ai]->getState()) {
                    ActuatorCommand off{r.actuatorId, false, 0, r.priority};
                    dispatch(off);
                }
            }
        }
    }

    void dispatch(const ActuatorCommand& cmd) {
        int8_t ai = _findActuatorIndex(cmd.actuatorId);
        if (ai < 0) return;
        if (cmd.priority >= _active[ai].priority) {
            _active[ai] = cmd;
            _activeUntil[ai] = (cmd.state && cmd.durationMs > 0)
                                ? millis() + cmd.durationMs : 0;
            if (_actuators[ai]) _actuators[ai]->execute(cmd);
        }
    }

    int8_t _findActuatorIndex(uint8_t id) const {
        for (uint8_t i = 0; i < _actuatorCount; i++)
            if (_actuators[i] && _actuators[i]->getId() == id) return (int8_t)i;
        return -1;
    }
};
