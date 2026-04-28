#ifndef RELAY_MODULE_2CH_H
#define RELAY_MODULE_2CH_H

#include "../ModbusManager.h"
#include "../core/IActuator.h"
#include "../debug.h"
#include <Arduino.h>

/**
 * 2-Channel RS485 Modbus Relay Module
 *
 * Implements IActuator.
 * actuatorId encoding: bits[7:4] = Modbus address, bits[3:0] = channel (0 or 1).
 * Use RelayModule2CH::makeActuatorId(addr, ch) to build the ID.
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
class RelayModule2CH : public IActuator {
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
        DBG_VERBOSE("[Relay CB] %d\n", event);
        return true;
    }

    /** Build actuatorId: high nibble = Modbus address, low nibble = channel */
    static uint8_t makeActuatorId(uint8_t modbusAddr, uint8_t channel) {
        return (uint8_t)((modbusAddr << 4) | (channel & 0x0F));
    }

public:
    RelayModule2CH(uint8_t address = 1, String alias = "")
        : _address(address), _alias(alias), _active(false), _failureCount(0), _inactiveCheckCount(0) {
        _relayState[0] = false;
        _relayState[1] = false;
        _inputState[0] = false;
        _inputState[1] = false;
    }

    // ── IActuator ────────────────────────────────────────────────────────────
    /** actuatorId = (address<<4) | 0  (channel 0 by default) */
    uint8_t getId() const override { return makeActuatorId(_address, 0); }

    const char* getName() const override {
        static char buf[24];
        if (_alias.length() > 0) {
            snprintf(buf, sizeof(buf), "%s", _alias.c_str());
        } else {
            snprintf(buf, sizeof(buf), "relay_mod_%d", _address);
        }
        return buf;
    }

    /** begin() → calls init() and returns result */
    bool begin() override { return init(); }

    /**
     * execute() → routes to the correct relay channel.
     * Channel is decoded from the low nibble of cmd.actuatorId.
     * If actuatorId matches this module's base ID, channel 0 is used.
     */
    void execute(const ActuatorCommand& cmd) override {
        uint8_t ch = cmd.actuatorId & 0x0F;  // low nibble = channel
        if (ch > 1) ch = 0;                  // clamp to valid range
        setRelay(ch, cmd.state);
        // auto-off is managed by ControlMediator via tick()
    }

    /** Returns state of channel 0 (primary channel). */
    bool getState() const override { return _relayState[0]; }

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
        DBG_VERBOSE("[Relay %d] Init...\n", _address);
        if (!ModbusManager::getInstance().isInitialized()) {
            DBG_ERROR("[Relay %d] Modbus not ready\n", _address);
            _active = false;
            return false;
        }

        if (syncState()) {
            _active = true;
            _failureCount = 0;
            DBG_INFO("[Relay %d] Init OK\n", _address);
        } else {
            _active = false;
            DBG_ERROR("[Relay %d] Not responding\n", _address);
        }
        return _active;
    }

    bool isActive() {
        return true;
        if (_active) {
            _inactiveCheckCount = 0;
            return true;
        }

        _inactiveCheckCount++;
        if (_inactiveCheckCount >= 10) {
            DBG_VERBOSE("[Relay %d] Recovery attempt\n", _address);
            _inactiveCheckCount = 0;
            return init();
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

        delay(20);

        DBG_VERBOSE("[Relay %d] ch%d -> %s\n", _address, channel, state ? "ON" : "OFF");

        mb->task();

        _cbComplete = false;
        _cbError = false;

        if (!mb->writeCoil(_address, channel, state, modbusCallback)) {
            return false;
        }

        unsigned long start = millis();
        while (!_cbComplete && millis() - start < 1000) {
            mb->task();
            delay(10);
        }

        if (_cbComplete && !_cbError) {
            if (channel < 2) _relayState[channel] = state;
            _failureCount = 0;
            DBG_VERBOSE("[Relay %d] ch%d OK\n", _address, channel);
            return true;
        }

        _failureCount++;
        DBG_ERROR("[Relay %d] ch%d FAIL (%d)\n", _address, channel, _failureCount);
        if (_failureCount >= 5) {
            _active = false;
            DBG_ERROR("[Relay %d] Disabled\n", _address);
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

        delay(50);
        mb->task();

        _cbComplete = false;
        _cbError = false;

        bool coils[8];
        if (mb->readCoil(_address, 0, coils, 8, modbusCallback)) {
            unsigned long start = millis();
            while (!_cbComplete && millis() - start < 1000) {
                mb->task();
                delay(10);
            }
        }

        if (_cbComplete && !_cbError) {
            _relayState[0] = coils[0];
            _relayState[1] = coils[1];
            _failureCount = 0;
            DBG_VERBOSE("[Relay %d] sync R0=%d R1=%d\n", _address, _relayState[0], _relayState[1]);
            return true;
        }
        _failureCount++;
        DBG_ERROR("[Relay %d] sync FAIL (%d)\n", _address, _failureCount);
        if (_failureCount >= 5) {
            _active = false;
            DBG_ERROR("[Relay %d] Disabled\n", _address);
        }
        return false;
    }

    bool syncInputs() {
        ModbusRTU* mb = ModbusManager::getInstance().getModbus();
        if (!mb) return false;

        mb->task();

        _cbComplete = false;
        _cbError = false;

        bool inputs[8];
        if (!mb->readIsts(_address, 0, inputs, 8, modbusCallback)) {
            DBG_ERROR("[Relay %d] Input read error\n", _address);
            return false;
        }

        unsigned long start = millis();
        while (!_cbComplete && millis() - start < 1000) {
            mb->task();
            delay(10);
        }

        if (_cbComplete && !_cbError) {
            _inputState[0] = inputs[0];
            _inputState[1] = inputs[1];
            _failureCount = 0;
            DBG_VERBOSE("[Relay %d] IN1=%d IN2=%d\n", _address, _inputState[0], _inputState[1]);
            return true;
        }

        _failureCount++;
        DBG_ERROR("[Relay %d] Input FAIL (%d)\n", _address, _failureCount);
        if (_failureCount >= 3) {
            _active = false;
            DBG_ERROR("[Relay %d] Disabled\n", _address);
        }
        return false;
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
