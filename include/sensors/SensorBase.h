#pragma once
#include "ISensor.h"
#include <Arduino.h>   // ESP.getEfuseMac()

/**
 * SensorBase — mixin that provides stable SensorKey identity and a
 * per-sensor monotonic read counter for all concrete sensor classes.
 *
 * Key design decisions:
 *   - deviceId  = last byte of EFuse MAC → unique per board, no config needed.
 *   - sensorId  = stable physical address passed at construction:
 *                   pin number       for ADC/GPIO sensors (Capacitive, HD38)
 *                   I2C address      for I2C sensors (SCD30=0x61, BME280=0x76/0x77)
 *                   Modbus address   for RS485 sensors (ModbusTH, ModbusSoil)
 *                   last addr byte   for OneWire sensors
 *   - counter   = incremented every time _fillReading() is called.
 *                 ControlMediator rejects readings with counter ≤ stored.
 *
 * setSensorId() is intentionally absent — IDs must be stable at construction.
 */
class SensorBase {
public:
    explicit SensorBase(uint8_t stableSensorId)
        : _key({_localDeviceId(), stableSensorId}), _counter(0) {}

    SensorKey getKey() const { return _key; }

protected:
    SensorKey _key;
    uint32_t  _counter;

    /**
     * Fill a SensorReading with the current counter and value.
     * Increments counter before filling — always call after a successful read().
     */
    bool _fillReading(SensorReading& out, float value) {
        ++_counter;
        out.key     = _key;
        out.value   = value;
        out.counter = _counter;
        return true;
    }

    /**
     * Returns the last byte of this device's EFuse MAC.
     * Used once at construction; result is stable for the lifetime of the board.
     */
    static uint8_t _localDeviceId() {
        return (uint8_t)(ESP.getEfuseMac() & 0xFF);
    }
};
