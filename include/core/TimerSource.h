#pragma once
#include "ControlMediator.h"
#include "MediatorConfig.h"
#include <Arduino.h>

/**
 * TimerSource — fuente de activación periódica para actuadores.
 *
 * Permite programar ciclos automáticos independientes del ciclo de lectura
 * de sensores. Por ejemplo, encender una bomba cada 6 horas durante 30 s.
 *
 * Cada entrada dispara mediator.onTimerTick() cuando su período vence.
 * DEBE llamarse desde loop():  timerSource.tick(mediator);
 */
struct TimerEntry {
    uint8_t  actuatorId;
    uint32_t periodMs;    ///< período de repetición
    uint32_t durationMs;  ///< duración del pulso ON; 0 = mantener
    uint32_t lastFireMs;  ///< millis() de la última activación
    bool     enabled;
};

class TimerSource {
public:
    TimerSource() : _count(0) {}

    /**
     * Agregar un temporizador.
     * @param actuatorId Actuador a disparar.
     * @param periodMs   Cada cuántos ms se dispara.
     * @param durationMs Duración del pulso ON (0 = mantener activo).
     */
    void addTimer(uint8_t actuatorId, uint32_t periodMs,
                  uint32_t durationMs = 0) {
        if (_count >= ACTUATOR_MAX) return;
        _entries[_count++] = {
            actuatorId, periodMs, durationMs,
            millis(),   // empezar a contar desde ahora
            true
        };
    }

    /** Deshabilitar un timer por ID de actuador */
    void disable(uint8_t actuatorId) {
        for (uint8_t i = 0; i < _count; i++) {
            if (_entries[i].actuatorId == actuatorId)
                _entries[i].enabled = false;
        }
    }

    /** Habilitar un timer por ID de actuador */
    void enable(uint8_t actuatorId) {
        for (uint8_t i = 0; i < _count; i++) {
            if (_entries[i].actuatorId == actuatorId)
                _entries[i].enabled = true;
        }
    }

    /** Procesar todos los timers. Llamar desde loop(). */
    void tick(ControlMediator& mediator) {
        uint32_t now = millis();
        for (uint8_t i = 0; i < _count; i++) {
            TimerEntry& e = _entries[i];
            if (!e.enabled) continue;
            if (now - e.lastFireMs >= e.periodMs) {
                e.lastFireMs = now;
                mediator.onTimerTick(e.actuatorId, true, e.durationMs);
            }
        }
    }

    uint8_t getCount() const { return _count; }

private:
    TimerEntry _entries[ACTUATOR_MAX];
    uint8_t    _count;
};
