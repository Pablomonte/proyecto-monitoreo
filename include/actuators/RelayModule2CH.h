#ifndef RELAY_MODULE_2CH_H
#define RELAY_MODULE_2CH_H

#include "../ModbusManager.h"
#include <Arduino.h>

/**
 * 2-Channel RS485 Modbus Relay Module
 * 
 * Features:
 * - 2 Relays (NO/NC)
 * - 1 Digital Input (IN1)
 * - Modbus RTU (9600 8N1 default)
 * 
 * Addressing (Standard):
 * - Relay 0: Coil 0 (0x0000)
 * - Relay 1: Coil 1 (0x0001)
 * - Input 1: Discrete Input 0 (0x0000)
 */
class RelayModule2CH {
private:
    uint8_t _address;
    String _alias;
    bool _relayState[2]; // Cache for relay states (0 and 1)
    bool _inputState[2]; // Cache for input states
    
    // State machine for robustness
    bool _active;
    int _failureCount;
    int _inactiveCheckCount;

    // Callback state
    static bool _cbComplete;
    static bool _cbError;

    static bool modbusCallback(Modbus::ResultCode event, uint16_t transactionId, void* data) {
        _cbComplete = true;
        _cbError = (event != Modbus::EX_SUCCESS);
        return true;
    }

public:
    RelayModule2CH(uint8_t address = 1, String alias = "") 
        : _address(address), _alias(alias), _active(false), _failureCount(0), _inactiveCheckCount(0) {
        _relayState[0] = false;
        _relayState[1] = false;
        _inputState[0] = false;
        _inputState[1] = false;
    }

    void setAlias(String alias) { _alias = alias; }
    String getAlias() const { return _alias; }
    uint8_t getAddress() const { return _address; }

    bool getState(uint8_t channel) const {
        if (channel < 2) return _relayState[channel];
        return false;
    }

    bool getInputState(uint8_t channel) const {
        if (channel < 2) return _inputState[channel];
        return false;
    }

    bool init() {
        Serial.printf("[Relay %d] Initializing...\n", _address);
        if (!ModbusManager::getInstance().isInitialized()) {
            Serial.printf("[Relay %d] Modbus manager not ready\n", _address);
            _active = false;
            return false;
        }
        
        // Test read to verify connectivity
        if (syncState()) {
            _active = true;
            _failureCount = 0;
            Serial.printf("[Relay %d] Initialized successfully\n", _address);
        } else {
            _active = false;
            Serial.printf("[Relay %d] Not responding\n", _address);
        }
        return _active;
    }

    bool isActive() {
        if (_active) {
            _inactiveCheckCount = 0;
            return true;
        }

        // If inactive, try to recover periodically
        _inactiveCheckCount++;
        if (_inactiveCheckCount >= 10) {
            Serial.printf("[Relay %d] Attempting to recover connection...\n", _address);
            _inactiveCheckCount = 0;
            return init(); // Attempt to re-initialize
        }
        return false;
    }

    /**
     * Set relay state
     * @param channel 0 or 1
     * @param state true=ON, false=OFF
     */
    bool setRelay(uint8_t channel, bool state) {
        ModbusRTU* mb = ModbusManager::getInstance().getModbus();
        if (!mb) return false;

        Serial.printf("[Relay %d] Setting channel %d to %s\n", _address, channel, state ? "ON" : "OFF");

        _cbComplete = false;
        _cbError = false;
        mb->writeCoil(_address, channel, state, modbusCallback);

        unsigned long start = millis();
        while (!_cbComplete && millis() - start < 1000) {
            mb->task();
            delay(10);
        }

        if (_cbComplete && !_cbError) {
            if (channel < 2) _relayState[channel] = state;
            _failureCount = 0; // Reset on success
            Serial.printf("[Relay %d] Set channel %d successful\n", _address, channel);
            return true;
        }

        _failureCount++;
        Serial.printf("[Relay %d] Set channel %d FAILED (failures: %d)\n", _address, channel, _failureCount);
        if (_failureCount >= 3) {
            _active = false;
            Serial.printf("[Relay %d] Disabled after 3 consecutive failures\n", _address);
        }
        return false;
    }

    /**
     * Toggle relay state
     */
    bool toggleRelay(uint8_t channel) {
        if (channel >= 2) return false;
        if (!isActive()) return false; // Quick fail
        return setRelay(channel, !_relayState[channel]);
    }

    /**
     * Read actual coils from device to update local cache
     */
    bool syncState() {
        ModbusRTU* mb = ModbusManager::getInstance().getModbus();
        if (!mb) return false;

        _cbComplete = false;
        _cbError = false;
        
        bool coils[2];
        mb->readCoil(_address, 0, coils, 2, modbusCallback);
        
        unsigned long start = millis();
        while (!_cbComplete && millis() - start < 1000) {
            mb->task();
            delay(10);
        }
        
        if (_cbComplete && !_cbError) {
            _relayState[0] = coils[0];
            _relayState[1] = coils[1];
            _failureCount = 0; // Reset on success
            return true;
        }

        _failureCount++;
        Serial.printf("[Relay %d] syncState FAILED (failures: %d)\n", _address, _failureCount);
        if (_failureCount >= 3) {
            _active = false;
            Serial.printf("[Relay %d] Disabled after 3 consecutive failures\n", _address);
        }
        return false;
    }

    bool syncInputs() {
        ModbusRTU* mb = ModbusManager::getInstance().getModbus();
        if (!mb) return false;

        Serial.printf("[Relay %d] Reading inputs...\n", _address);

        _cbComplete = false;
        _cbError = false;
        
        bool inputs[8]; 
        mb->readIsts(_address, 0, inputs, 8, modbusCallback);
        
        unsigned long start = millis();
        while (!_cbComplete && millis() - start < 1000) {
            mb->task();
            delay(10);
        }
        
        if (_cbComplete && !_cbError) {
            _inputState[0] = inputs[0];
            _inputState[1] = inputs[1];
            _failureCount = 0; // Reset on success
            Serial.printf("[Relay %d] Inputs read: IN1=%d, IN2=%d\n", _address, _inputState[0], _inputState[1]);
            return true;
        }

        _failureCount++;
        Serial.printf("[Relay %d] Read inputs FAILED (failures: %d)\n", _address, _failureCount);
        if (_failureCount >= 3) {
            _active = false;
            Serial.printf("[Relay %d] Disabled after 3 consecutive failures\n", _address);
        }
        return false;
    }

    /**
     * Read input state (IN1)
     * Returns 1 if active, 0 if inactive, -1 on error
     */
    int readInput() {
        ModbusRTU* mb = ModbusManager::getInstance().getModbus();
        if (!mb) return -1;

        _cbComplete = false;
        _cbError = false;
        
        bool inputs[8]; // Buffer for inputs (Module returns 8 bits usually)
        
        // Read Discrete Inputs (Function 02)
        // Start Address 0, Count 8 (to be safe, or 1)
        mb->readIsts(_address, 0, inputs, 8, modbusCallback);
        
        unsigned long start = millis();
        while (!_cbComplete && millis() - start < 1000) {
            mb->task();
            delay(10);
        }
        
        if (_cbComplete && !_cbError) {
            // IN1 corresponds to the first bit usually
            return inputs[0] ? 1 : 0;
        }
        return -1;
    }

    /**
     * Get JSON representation for Web UI
     */
    String getStatusJSON() {
        String json = "{";
        json += "\"address\":" + String(_address) + ",";
        json += "\"alias\":\"" + _alias + "\",";
        json += "\"r0\":" + String(_relayState[0] ? 1 : 0) + ",";
        json += "\"r1\":" + String(_relayState[1] ? 1 : 0);
        json += "}";
        return json;
    }

    String getGrafanaString() {
        String s = "";
        s += "relay1=" + String(_relayState[0] ? 1 : 0);
        s += ",relay2=" + String(_relayState[1] ? 1 : 0);
        s += ",in1=" + String(_inputState[0] ? 1 : 0);
        s += ",in2=" + String(_inputState[1] ? 1 : 0);
        return s;
    }
};

#endif // RELAY_MODULE_2CH_H
