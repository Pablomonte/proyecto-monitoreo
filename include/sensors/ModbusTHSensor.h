#ifndef MODBUS_TH_SENSOR_H
#define MODBUS_TH_SENSOR_H

#include "ISensor.h"
#include <ModbusRTU.h>
#include <HardwareSerial.h>
#include "../ModbusManager.h"

/**
 * Sensor TH-MB-04S - Temperatura y Humedad via Modbus RTU
 *
 * Protocolo: Modbus RTU sobre RS485
 * Registros:
 *   - 1 (0x01): Humedad (int16 * 10, ej: 399 = 39.9% RH)
 *   - 2 (0x02): Temperatura (int16 * 10, ej: 282 = 28.2°C)
 *
 * Soporta multiples sensores en el mismo bus RS485 con diferentes direcciones.
 * El bus se inicializa una sola vez y se comparte entre todas las instancias.
 *
 * Config:
 *   - address: Direccion Modbus (1-254)
 *   - rxPin: GPIO RX RS485 (default 16)
 *   - txPin: GPIO TX RS485 (default 17)
 *   - dePin: GPIO DE/RE control (default 18, -1 si no usa)
 *   - baudrate: Baudrate RS485 (default 9600)
 */
class ModbusTHSensor : public ISensor {
private:
    uint8_t modbusAddress;
    int rxPin;
    int txPin;
    int dePin;
    uint32_t baudrate;

    float temperature = 999;
    float humidity = 99;
    bool active;
    int readFailureCount;
    int inactiveCheckCount;

    // Modbus read result callbacks
    static uint16_t registerBuffer[2];
    static bool readComplete;
    static bool readError;

    static bool readCallback(Modbus::ResultCode event, uint16_t transactionId, void* data) {
        readComplete = true;
        if (event == Modbus::EX_SUCCESS) {
            readError = false;
            return true;
        } else {
            Serial.printf("[ModbusTH] Read error: %02X\n", event);
            readError = true;
            return false;
        }
    }

    // Initialize shared bus (only once)
    static bool initBus(int rx, int tx, int de, uint32_t baud) {
        return ModbusManager::getInstance().begin(rx, tx, de, baud);
    }

public:
    ModbusTHSensor(uint8_t address = 1,
                   int rx = 16,
                   int tx = 17,
                   int de = 18,
                   uint32_t baud = 9600)
        : modbusAddress(address),
          rxPin(rx),
          txPin(tx),
          dePin(de),
          baudrate(baud),
          temperature(-1),
          humidity(-1),
          active(false),
          readFailureCount(0),
          inactiveCheckCount(0) {
    }

    bool init() override {
        Serial.printf("[ModbusTH] Initializing sensor addr=%d\n", modbusAddress);

        // Initialize shared bus
        if (!initBus(rxPin, txPin, dePin, baudrate)) {
            Serial.println("[ModbusTH] Failed to initialize bus");
            return false;
        }

        // Test read to verify sensor is accessible
        delay(100);
        bool testRead = readRegisters();

        if (testRead) {
            active = true;
            readFailureCount = 0;
            Serial.printf("[ModbusTH] Sensor addr=%d initialized successfully\n", modbusAddress);
        } else {
            active = false;
            Serial.printf("[ModbusTH] Sensor addr=%d not responding\n", modbusAddress);
        }

        return active;
    }

    bool dataReady() override {
        return active;
    }

    bool read() override {
        if (!active) return false;

        bool success = readRegisters();

        if (success) {
            readFailureCount = 0;
            // Registers come multiplied by 10
            humidity = registerBuffer[0] / 10.0;
            temperature = registerBuffer[1] / 10.0;

            Serial.printf("[ModbusTH] Addr %d: T=%.1f C, H=%.1f%%\n",
                         modbusAddress, temperature, humidity);
            return true;
        }
        
        readFailureCount++;
        if (readFailureCount >= 5) {
            active = false;
            Serial.printf("[ModbusTH] Addr %d: Disabled after 5 consecutive read failures\n", modbusAddress);
        }

        // On failure, set invalid values ... do not just keep the lastone 
        humidity = 99;
        temperature = 999;

        Serial.printf("[ModbusTH] Addr %d: T=%.1f C, H=%.1f%%\n",
                        modbusAddress, temperature, humidity);
        Serial.printf("[ModbusTH] Addr %d: Read failed\n", modbusAddress);
        return false;

    }

    float getTemperature() override {
        return temperature;
    }

    float getHumidity() override {
        return humidity;
    }

    float getCO2() override {
        return -1;  // Not supported
    }

    const char* getSensorType() override {
        static char sensorName[32];
        snprintf(sensorName, sizeof(sensorName), "modbus_th_%d", modbusAddress);
        return sensorName;
    }
    // Identifier format: "th-mod-address"
    const char* getSensorID() override {
        static char sensorID[32];
        snprintf(sensorID, sizeof(sensorID), "th-mod-%d", modbusAddress);
        return sensorID;
    }

    const char* getMeasurementsString() override {
        static char measString[32];
        //" temp=" + String(temperature, 2) + ",hum=" + String(humidity, 2) +  ",co2=" + String(co2) +
        snprintf(measString, sizeof(measString), "temp=%.1f,hum=%.1f", temperature, humidity);
        return measString;
    }

    bool calibrate(float reference) override {
        return false;
    }

    bool isActive() override {
        if (active) {
            inactiveCheckCount = 0;
            return true;
        }
        inactiveCheckCount++;
        if (inactiveCheckCount >= 10) {
            inactiveCheckCount = 0;
            return init();
        }
        return false;
    }

    uint8_t getAddress() const {
        return modbusAddress;
    }

private:
    bool readRegisters() {
        ModbusRTU* mb = ModbusManager::getInstance().getModbus();
        if (!mb) return false;

        readComplete = false;
        readError = false;

        // Ensure library state is clean before starting new transaction
        mb->task();

        // Read 2 holding registers starting from address 1
        // Register 0 (0x00): Humidity
        // Register 1 (0x01): Temperature
        if (!mb->readHreg(modbusAddress, 0, registerBuffer, 2, readCallback)) {
            Serial.printf("[ModbusTH] Addr %d: Failed to initiate read\n", modbusAddress);
            mb->task(); // Process any pending tasks cleanup
            return false;
        }

        // Wait for response (with timeout)
        unsigned long startTime = millis();
        while (!readComplete && (millis() - startTime < 2000)) {
            mb->task();
            delay(10);
        }

        if (!readComplete) {
            Serial.printf("[ModbusTH] Addr %d: Read timeout\n", modbusAddress);
            Serial.printf("[ModbusTH] Addr %d: Read timeout\n", modbusAddress);
            return false;
        }

        if (readError){
            Serial.printf("[ModbusTH] Addr %d: Read error\n", modbusAddress);
            return false;
        }
        return true;
    }
};

// Static member initialization
uint16_t ModbusTHSensor::registerBuffer[2] = {0, 0};
bool ModbusTHSensor::readComplete = false;
bool ModbusTHSensor::readError = false;

#endif // MODBUS_TH_SENSOR_H
