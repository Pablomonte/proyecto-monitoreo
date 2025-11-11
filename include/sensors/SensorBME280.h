#ifndef SENSOR_BME280_H
#define SENSOR_BME280_H

#include "ISensor.h"
#include <Adafruit_BME280.h>

class SensorBME280 : public ISensor {
private:
    Adafruit_BME280 bme;
    bool active;
    float temperature;
    float humidity;
    float pressure;  // hPa (not exposed via interface, but stored)

public:
    SensorBME280() : active(false), temperature(99), humidity(100), pressure(0) {}

    bool init() override {
        // Try I2C address 0x76 (default) or 0x77 (alternate)
        active = bme.begin(0x76) || bme.begin(0x77);

        if (!active) {
            Serial.println("No se pudo inicializar el sensor BME280!");
        } else {
            Serial.println("Sensor BME280 inicializado correctamente");
            // Configure sensor settings (optional)
            bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                          Adafruit_BME280::SAMPLING_X2,  // temperature
                          Adafruit_BME280::SAMPLING_X16, // pressure
                          Adafruit_BME280::SAMPLING_X1,  // humidity
                          Adafruit_BME280::FILTER_X16,
                          Adafruit_BME280::STANDBY_MS_500);
        }
        return active;
    }

    bool dataReady() override {
        // BME280 is always ready in normal mode
        return active;
    }

    bool read() override {
        if (!active) return false;

        temperature = bme.readTemperature();
        humidity = bme.readHumidity();
        pressure = bme.readPressure() / 100.0F;  // Convert Pa to hPa

        // Validate readings
        if (isnan(temperature) || isnan(humidity) || isnan(pressure)) {
            Serial.println("Error leyendo sensor BME280!");
            return false;
        }

        return true;
    }

    float getTemperature() override { return temperature; }
    float getHumidity() override { return humidity; }
    float getCO2() override { return -1; }  // Not available
    const char* getSensorType() override { return "BME280"; }
    bool isActive() override { return active; }

    // BME280-specific method (not in interface)
    float getPressure() { return pressure; }
};

#endif // SENSOR_BME280_H
