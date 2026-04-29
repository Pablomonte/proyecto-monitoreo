#pragma once
#include <stdint.h>

/**
 * SensorKey — identidad compuesta de una magnitud de sensor.
 *
 * deviceId : last byte of the device's EFuse MAC (ESP.getEfuseMac() & 0xFF).
 *            Unique per physical board; 0x00 is fallback only.
 * sensorId : stable within a device — derived from the sensor's physical
 *            address (pin, I2C addr, Modbus addr, OneWire last byte).
 *            NOT a sequential registration index.
 *
 * toU32()  : encodes key as uint32_t for use as hash index in the
 *            ControlMediator state-store.
 */
struct SensorKey {
    uint8_t deviceId;
    uint8_t sensorId;

    bool operator==(const SensorKey& o) const {
        return deviceId == o.deviceId && sensorId == o.sensorId;
    }
    bool operator!=(const SensorKey& o) const { return !(*this == o); }

    /** [15:8]=deviceId  [7:0]=sensorId */
    uint32_t toU32() const {
        return ((uint32_t)deviceId << 8) | (uint32_t)sensorId;
    }
};

/**
 * SensorReading — a single sample from one sensor magnitude.
 *
 * key     : which sensor on which device.
 * value   : reading in the sensor's native unit.
 * counter : monotonic read counter incremented by SensorBase::_fillReading().
 *           Does NOT require time-sync. ControlMediator uses it to discard
 *           stale/out-of-order readings.
 *           For remote ESP-NOW nodes, use the packet sequence field.
 */
struct SensorReading {
    SensorKey key;
    float     value;
    uint32_t  counter;
};
