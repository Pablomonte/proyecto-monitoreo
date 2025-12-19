#ifndef RELAY_MANAGER_H
#define RELAY_MANAGER_H

#include <vector>
#include <ArduinoJson.h>
#include "RelayModule2CH.h"

class RelayManager {
private:
    std::vector<RelayModule2CH*> relays;

public:
    ~RelayManager() {
        for (auto r : relays) delete r;
        relays.clear();
    }

    void loadFromConfig(JsonDocument& doc) {
        // Clear existing
        for (auto r : relays) delete r;
        relays.clear();

        if (!doc["relays"].is<JsonArray>()) {
            Serial.println("[RelayMgr] No 'relays' array in config");
            return;
        }

        JsonArrayConst relayArray = doc["relays"].as<JsonArrayConst>();
        Serial.printf("[RelayMgr] Found %d relays in config\n", relayArray.size());

        for (JsonObjectConst r : relayArray) {
            bool enabled = r["enabled"].as<bool>();
            if (enabled) {
                uint8_t addr = r["config"]["address"] | 1;
                String alias = r["config"]["alias"] | "";
                relays.push_back(new RelayModule2CH(addr, alias));
                Serial.printf("[RelayMgr] Added relay: Addr=%d, Alias='%s'\n", addr, alias.c_str());
            } else {
                Serial.println("[RelayMgr] Relay found but disabled");
            }
        }
    }

    std::vector<RelayModule2CH*>& getRelays() {
        return relays;
    }
    
    RelayModule2CH* getRelay(int index) {
        if(index >= 0 && index < relays.size()) return relays[index];
        return nullptr;
    }
};

#endif // RELAY_MANAGER_H