#ifndef ISENSOR_H
#define ISENSOR_H

#include "core/SensorKey.h"

/**
 * Base interface for all sensors.
 *
 * Extended to support the ControlMediator via SensorKey identity and
 * readValue(). Existing lifecycle and measurement interfaces are preserved.
 *
 * Measurement capabilities via dynamic_cast:
 *   - ITemperatureSensor : getTemperature()
 *   - IHumiditySensor    : getHumidity()  (air)
 *   - IMoistureSensor    : getMoisture()  (soil)
 *   - ICO2Sensor         : getCO2()
 *   - IPressureSensor    : getPressure()
 *   - ISoilSensor        : getEC(), getPH(), getNitrogen(), ...
 */
class ISensor {
public:
    virtual ~ISensor() = default;

    // ── Lifecycle (existing) ──────────────────────────────────────────────
    virtual bool init()      = 0;
    virtual bool dataReady() = 0;
    virtual bool read()      = 0;   ///< reads and caches values internally
    virtual bool isActive()  = 0;

    // ── Identification (existing) ─────────────────────────────────────────
    virtual const char* getSensorType()        = 0;
    virtual const char* getSensorID()          = 0;
    virtual const char* getMeasurementsString() = 0;

    // ── Mediator identity (new) ───────────────────────────────────────────
    /**
     * Returns the SensorKey for this sensor.
     * deviceId is always 0x00 for local sensors.
     * sensorId is assigned by SensorManager at registration time via setSensorId().
     */
    virtual SensorKey getKey() const = 0;

    /**
     * Human-readable name for logging and rules UI.
     * Default implementation delegates to getSensorType().
     */
    virtual const char* getName() const { return const_cast<ISensor*>(this)->getSensorType(); }

    // ── Mediator reading (new) ────────────────────────────────────────────
    /**
     * Read the primary value of this sensor into `out`.
     * Should only be called after a successful read().
     * @return false if no valid data is available.
     */
    virtual bool readValue(SensorReading& out) = 0;

    /**
     * Assign the sensorId component of this sensor's SensorKey.
     * Called by SensorManager when the sensor is registered.
     */
    virtual void setSensorId(uint8_t id) = 0;

    // ── Optional calibration (existing) ──────────────────────────────────
    virtual bool calibrate(float reference = 0) { return false; }
};

#endif // ISENSOR_H
