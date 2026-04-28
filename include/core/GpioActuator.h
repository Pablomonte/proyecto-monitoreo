#pragma once
#include "IActuator.h"
#include <Arduino.h>

/**
 * GpioActuator — actuador que controla un pin GPIO del ESP32.
 *
 * Características:
 *   - Nivel activo configurable (activeHigh=true → HIGH=ON, false → LOW=ON)
 *   - Auto-off basado en millis() cuando durationMs > 0
 *   - tick() debe llamarse desde loop() para procesar el auto-off
 *
 * RelayActuator es un alias de GpioActuator (un relé de 1 canal directo).
 */
class GpioActuator : public IActuator {
public:
    /**
     * @param id         Identificador único del actuador
     * @param pin        Pin GPIO
     * @param name       Nombre legible (para logs y API)
     * @param activeHigh true = HIGH activa el relé; false = LOW activa
     */
    GpioActuator(uint8_t id, uint8_t pin, const char* name = "gpio",
                 bool activeHigh = true)
        : _id(id), _pin(pin), _name(name), _activeHigh(activeHigh),
          _state(false), _offAt(0) {}

    uint8_t getId() const override { return _id; }

    const char* getName() const override { return _name; }

    bool begin() override {
        pinMode(_pin, OUTPUT);
        _write(false);
        return true;
    }

    void execute(const ActuatorCommand& cmd) override {
        _write(cmd.state);
        if (cmd.state && cmd.durationMs > 0) {
            _offAt = millis() + cmd.durationMs;
        } else {
            _offAt = 0;  // sin auto-off
        }
    }

    bool getState() const override { return _state; }

    /** Procesar auto-off. Llamar desde loop(). */
    void tick() override {
        if (_state && _offAt > 0 && millis() >= _offAt) {
            _write(false);
            _offAt = 0;
        }
    }

private:
    uint8_t     _id;
    uint8_t     _pin;
    const char* _name;
    bool        _activeHigh;
    bool        _state;
    uint32_t    _offAt;   ///< millis() en que se debe apagar; 0 = no auto-off

    void _write(bool on) {
        _state = on;
        digitalWrite(_pin, (_activeHigh ? on : !on) ? HIGH : LOW);
    }
};

/** Alias semántico para usar GpioActuator como relé de 1 canal */
using RelayActuator = GpioActuator;
