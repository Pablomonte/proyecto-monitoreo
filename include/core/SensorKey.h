#pragma once
#include <stdint.h>

/**
 * SensorKey — identidad compuesta de una magnitud de sensor.
 *
 * deviceId : 0x00 = este dispositivo; nodo remoto = último byte de su MAC
 *            (o campo node_id si el payload lo trae).
 * sensorId : único dentro de un dispositivo; asignado por SensorManager
 *            en orden de registro (0, 1, 2, …).
 *
 * toU32()  : codifica la clave como uint32_t para usar como índice de hash
 *            en el state-store del ControlMediator.
 */
struct SensorKey {
    uint8_t deviceId;
    uint8_t sensorId;

    bool operator==(const SensorKey& o) const {
        return deviceId == o.deviceId && sensorId == o.sensorId;
    }

    bool operator!=(const SensorKey& o) const {
        return !(*this == o);
    }

    /** Codifica la clave en 32 bits: [31:8]=deviceId, [7:0]=sensorId */
    uint32_t toU32() const {
        return ((uint32_t)deviceId << 8) | (uint32_t)sensorId;
    }
};

/**
 * SensorReading — una lectura puntual de una magnitud de sensor.
 *
 * key         : identifica qué sensor y de qué dispositivo.
 * value       : valor de la magnitud en la unidad nativa del sensor.
 * timestampMs : millis() en el momento de la lectura.
 */
struct SensorReading {
    SensorKey key;
    float     value;
    uint32_t  timestampMs;
};
