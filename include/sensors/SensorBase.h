#pragma once
#include "ISensor.h"

/**
 * SensorBase — mixin que implementa la gestión de SensorKey para todos
 * los sensores concretos.
 *
 * Uso: heredar de SensorBase además de ISensor (o junto con el sensor
 * concreto). Proporciona:
 *   - setSensorId(id)
 *   - getKey() const
 *
 * deviceId siempre es 0x00 (sensor local). Los sensores remotos usan
 * SensorKey directamente sin pasar por esta clase.
 */
class SensorBase {
public:
    SensorBase() : _key({0x00, 0x00}) {}

    void setSensorId(uint8_t id) { _key.sensorId = id; }

    SensorKey getKey() const { return _key; }

protected:
    SensorKey _key;
};
