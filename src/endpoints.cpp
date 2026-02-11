#include "endpoints.h"
#include "actuators/RelayManager.h"
#include "configFile.h"
#include "constants.h"
#include "debug.h"
#include "globals.h"
#include "version.h"
#include "web_assets.h"
// #include "webConfigPage.h" // Removed
#include "sensors/ICO2Sensor.h"
#include "sensors/IHumiditySensor.h"
#include "sensors/IMoistureSensor.h"
#include "sensors/IPressureSensor.h"
#include "sensors/ISensor.h"
#include "sensors/ISoilSensor.h"
#include "sensors/ITemperatureSensor.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>

#ifdef SENSOR_MULTI
#include "sensors/ICO2Sensor.h"
#include "sensors/IHumiditySensor.h"
#include "sensors/IMoistureSensor.h"
#include "sensors/IPressureSensor.h"
#include "sensors/ISensor.h"
#include "sensors/ISoilSensor.h"
#include "sensors/ITemperatureSensor.h"
#include <vector>
class SensorManager;
extern SensorManager sensorMgr;
extern std::vector<ISensor *> &getSensorList();
#endif

#ifdef ENABLE_ESPNOW
#include "ESPNowManager.h"
extern ESPNowManager espnowMgr;
#endif

extern RelayManager relayMgr;

// Helper to get sensor icon based on capabilities
String getSensorIcon(ISensor *s) {
#ifdef SENSOR_MULTI
  if (dynamic_cast<ICO2Sensor *>(s))
    return "🌬️";
  if (dynamic_cast<ISoilSensor *>(s))
    return "🌱";
  if (dynamic_cast<IMoistureSensor *>(s))
    return "🌱";
  if (dynamic_cast<IPressureSensor *>(s))
    return "🌡️";
  if (dynamic_cast<ITemperatureSensor *>(s))
    return "🌡️";
#endif
  return "📊";
}

// Helper to serve static files from SPIFFS with content-type and cache control
void serveStaticFile(const char *path, const char *contentType) {
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    if (file) {
      server.streamFile(file, contentType);
      file.close();
      return;
    }
  }
  server.send(404, "text/plain", "File not found");
}

void handleMediciones() {
  JsonDocument doc;
  JsonArray sensors = doc["sensors"].to<JsonArray>();

  String wifiStatus =
      (WiFi.status() == WL_CONNECTED) ? "connected" : "disconnected";

#ifdef SENSOR_MULTI
  for (auto *s : getSensorList()) {
    if (!s || !s->isActive())
      continue;

    // Read sensor (if not already read recently)
    // Note: main loop handles periodic reading, but we can try read here if
    // needed. However, read() might be slow. Better to use cached values or
    // assume main loop updates them. For single sensor architecture, read() was
    // called here. Let's assume data is relatively fresh or read it if ready.
    if (s->dataReady())
      s->read();

    JsonObject sensorObj = sensors.add<JsonObject>();
    sensorObj["type"] = s->getSensorType();
    sensorObj["id"] = s->getSensorID();
    sensorObj["icon"] = getSensorIcon(s);
    sensorObj["active"] = true;
    sensorObj["error"] = false; // TODO: Implement error checking

    JsonArray readings = sensorObj["readings"].to<JsonArray>();

    auto *tempSensor = dynamic_cast<ITemperatureSensor *>(s);
    auto *humSensor = dynamic_cast<IHumiditySensor *>(s);
    auto *co2Sensor = dynamic_cast<ICO2Sensor *>(s);
    auto *moistSensor = dynamic_cast<IMoistureSensor *>(s);
    auto *presSensor = dynamic_cast<IPressureSensor *>(s);
    auto *soilSensor = dynamic_cast<ISoilSensor *>(s);

    if (tempSensor) {
      JsonObject r = readings.add<JsonObject>();
      r["label"] = "Temp";
      r["value"] = String(tempSensor->getTemperature(), 1);
      r["unit"] = "°C";
      r["status"] = "ok";
    }
    if (humSensor) {
      JsonObject r = readings.add<JsonObject>();
      r["label"] = "Humedad";
      r["value"] = String(humSensor->getHumidity(), 1);
      r["unit"] = "%";
      r["status"] = "ok";
    }
    if (co2Sensor) {
      JsonObject r = readings.add<JsonObject>();
      r["label"] = "CO2";
      r["value"] = String(co2Sensor->getCO2(), 0);
      r["unit"] = "ppm";

      float val = co2Sensor->getCO2();
      if (val > 1000)
        r["status"] = "warn";
      else if (val > 1500)
        r["status"] = "bad";
      else
        r["status"] = "ok";
    }
    if (moistSensor) {
      JsonObject r = readings.add<JsonObject>();
      r["label"] = "Humedad";
      r["value"] = String(moistSensor->getMoisture(), 1);
      r["unit"] = "%";
      r["status"] = "ok";
    }
    if (presSensor) {
      JsonObject r = readings.add<JsonObject>();
      r["label"] = "Presión";
      r["value"] = String(presSensor->getPressure(), 1);
      r["unit"] = "hPa";
      r["status"] = "ok";
    }

    // Soil Sensor specific extras (N, P, K, pH, EC) if available via generic
    // interface Currently specialized casting might be needed if ISoilSensor
    // exposes them directly
    if (soilSensor) {
      // Assuming ISoilSensor might have these methods in future or we need to
      // cast to specific implementation For now, let's stick to standard
      // interfaces. If specific ModbusSoilSensor, we might need dynamic_cast if
      // headers are available
    }
  }
#else
  if (sensor && sensor->isActive()) {
    if (sensor->dataReady())
      sensor->read();

    JsonObject sensorObj = sensors.add<JsonObject>();
    sensorObj["type"] = sensor->getSensorType();
    sensorObj["id"] = sensor->getSensorID();
    sensorObj["icon"] = getSensorIcon(sensor);
    sensorObj["active"] = true;
    sensorObj["error"] = !sensor->dataReady(); // Rough check

    JsonArray readings = sensorObj["readings"].to<JsonArray>();

    auto *tempSensor = dynamic_cast<ITemperatureSensor *>(sensor);
    auto *humSensor = dynamic_cast<IHumiditySensor *>(sensor);
    auto *co2Sensor = dynamic_cast<ICO2Sensor *>(sensor);

    if (tempSensor) {
      JsonObject r = readings.add<JsonObject>();
      r["label"] = "Temp";
      r["value"] = String(tempSensor->getTemperature(), 1);
      r["unit"] = "°C";
      r["status"] = "ok";
    }
    if (humSensor) {
      JsonObject r = readings.add<JsonObject>();
      r["label"] = "Humedad";
      r["value"] = String(humSensor->getHumidity(), 1);
      r["unit"] = "%";
      r["status"] = "ok";
    }
    if (co2Sensor) {
      JsonObject r = readings.add<JsonObject>();
      r["label"] = "CO2";
      r["value"] = String(co2Sensor->getCO2(), 0);
      r["unit"] = "ppm";
      r["status"] = "ok";
    }
  }
#endif

  // Legacy fields for backward compatibility (optional, but good practice)
  // We can populate them from the first sensor found, or leave empty.
  doc["wifi_status"] = wifiStatus;

  // Add Uptime for Live Data page
  unsigned long uptimeSec = millis() / 1000;
  String uptime = String(uptimeSec / 3600) + "h " +
                  String((uptimeSec % 3600) / 60) + "m " +
                  String(uptimeSec % 60) + "s";
  doc["uptime"] = uptime;

  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}

void handleStatus() {
  JsonDocument doc;

  // System
  JsonDocument config = loadConfig();
  doc["device_name"] = config["incubator_name"] | "Unknown";
  doc["device_id"] = config["hash"] | "N/A";
  doc["firmware_version"] = FIRMWARE_VERSION;

  // WiFi
  bool connected = (WiFi.status() == WL_CONNECTED);
  doc["wifi_connected"] = connected;
  if (connected) {
    doc["wifi_ssid"] = WiFi.SSID();
    doc["wifi_ip"] = WiFi.localIP().toString();
    int rssi = WiFi.RSSI();
    doc["wifi_rssi"] = rssi;

    String signalClass = "signal-weak";
    if (rssi > -50)
      signalClass = "signal-excellent";
    else if (rssi > -60)
      signalClass = "signal-good";
    else if (rssi > -70)
      signalClass = "signal-fair";
    doc["signal_class"] = signalClass;
  }

  // Sensors
  int activeSensors = 0;
  int totalSensors = 0;
#ifdef SENSOR_MULTI
  for (auto *s : getSensorList()) {
    totalSensors++;
    if (s && s->isActive())
      activeSensors++;
  }
#else
  totalSensors = 1;
  if (sensor && sensor->isActive())
    activeSensors = 1;
#endif
  doc["active_sensors"] = activeSensors;
  doc["total_sensors"] = totalSensors;

  // Uptime
  unsigned long uptimeSec = millis() / 1000;
  String uptime =
      String(uptimeSec / 3600) + "h " + String((uptimeSec % 3600) / 60) + "m";
  doc["uptime"] = uptime;

  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}

void handleHome() {
  // If WiFi not connected, redirect to Portal
  if (WiFi.status() != WL_CONNECTED) {
    server.sendHeader("Location", "/wifi-setup", true);
    server.send(302, "text/plain", "");
    return;
  }
  server.send(200, "text/html", index_html);
}

void handleData() { server.send(200, "text/html", data_html); }

void handleSettings() { server.send(200, "text/html", config_html); }

void handleStyle() { server.send(200, "text/css", style_css); }

void handleConfigJs() { server.send(200, "application/javascript", config_js); }

void handleFavicon() { server.send(200, "image/svg+xml", favicon_svg); }

void handleConfiguracion() {
  JsonDocument doc = loadConfig();

  if (doc.isNull() || doc.size() == 0) {
    server.send(500, "application/json",
                "{\"error\": \"No se pudo cargar config.json\"}");
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    doc["current_wifi_channel"] = WiFi.channel();
  } else {
    doc["current_wifi_channel"] = 0;
  }

  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}

void habldePostConfig() {
  DBG_INFO("Updating config...\n");

  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No JSON data received");
    return;
  }

  String jsonString = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    DBG_ERROR("JSON parse error: %s\n", error.c_str());
    server.send(400, "text/plain", "Invalid JSON format");
    return;
  }

  const char *new_ssid = doc["ssid"];
  const char *new_password = doc["passwd"];

  if (new_ssid && strlen(new_ssid) > 0 && strcmp(new_ssid, "ToChange") != 0) {
    wifiManager.onChange(String(new_ssid), String(new_password));
    DBG_INFO("WiFi updated: %s\n", new_ssid);
  }

  if (updateConfig(doc)) {
    server.send(
        200, "text/plain",
        "Configuration updated successfully. Some changes require restart.");
    DBG_INFO("Config saved\n");
  } else {
    server.send(500, "text/plain", "Failed to save configuration");
    DBG_ERROR("Config save failed\n");
  }
}

void handleSCD30Calibration() {
  DBG_VERBOSE("Calibration called: %s\n",
              sensor ? sensor->getSensorType() : "NULL");

  String response = "{";
  int httpStatus = 200;

  if (!sensor || !sensor->isActive()) {
    response += "\"status\":\"error\",";
    response += "\"message\":\"No sensor active\",";
    response += "\"sensor_detected\":false,";
    response += "\"calibration_performed\":false";
    httpStatus = 503;
  } else {
    bool calibrationSuccess = sensor->calibrate(400);

    if (calibrationSuccess) {
      response += "\"status\":\"success\",";
      response += "\"message\":\"Sensor calibration completed successfully\",";
      response +=
          "\"sensor_type\":\"" + String(sensor->getSensorType()) + "\",";
      response += "\"sensor_detected\":true,";
      response += "\"calibration_performed\":true,";
      response += "\"target_co2\":400,";
      response += "\"note\":\"Allow 2-3 minutes for sensor to stabilize after "
                  "calibration\"";
      DBG_INFO("Calibration OK\n");
    } else {
      response += "\"status\":\"error\",";
      response += "\"message\":\"Calibration not supported or failed for " +
                  String(sensor->getSensorType()) + "\",";
      response +=
          "\"sensor_type\":\"" + String(sensor->getSensorType()) + "\",";
      response += "\"sensor_detected\":true,";
      response += "\"calibration_performed\":false";
      httpStatus = 500;
      DBG_ERROR("Calibration failed: %s\n", sensor->getSensorType());
    }
  }

  response += "}";
  server.send(httpStatus, "application/json", response);
}

void handleRestart() {
  server.send(200, "text/plain", "Restarting ESP32...");
  delay(1000);
  ESP.restart();
}

void handleConfigReset() {
  DBG_INFO("Resetting config...\n");

  if (SPIFFS.exists(CONFIG_FILE_PATH)) {
    if (SPIFFS.remove(CONFIG_FILE_PATH)) {
      DBG_INFO("Config removed\n");
    } else {
      DBG_ERROR("Remove failed\n");
    }
  }

  createConfigFile();

  JsonDocument doc;
  doc["success"] = true;
  doc["message"] = "Configuration reset to defaults. Restarting...";

  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);

  DBG_INFO("Restarting...\n");
  delay(1000);
  ESP.restart();
}

#ifdef ENABLE_ESPNOW
void handleESPNowStatus() {
  JsonDocument doc;

  JsonDocument config = loadConfig();
  bool espnowEnabled = config["espnow_enabled"] | false;
  String forcedMode = config["espnow_force_mode"] | "";
  String actualMode = espnowMgr.getMode();

  doc["enabled"] = espnowEnabled;
  doc["mode"] = actualMode;
  doc["forced_mode"] = forcedMode;
  doc["mac_address"] = espnowMgr.getMACAddress();
  doc["channel"] = (WiFi.status() == WL_CONNECTED) ? WiFi.channel() : 0;

  if (actualMode == "sensor") {
    doc["paired"] = espnowMgr.isPaired();
    doc["peer_count"] = 0;
  } else {
    doc["paired"] = true;
    doc["peer_count"] = espnowMgr.getActivePeerCount();
  }

  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}
#endif

void handleRelayList() {
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();

  for (auto *r : relayMgr.getRelays()) {
    if (r) {
      r->syncState();
      JsonDocument tempDoc;
      deserializeJson(tempDoc, r->getStatusJSON());
      arr.add(tempDoc);
    }
  }

  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}

void handleRelayToggle() {
  if (!server.hasArg("addr") || !server.hasArg("ch")) {
    server.send(400, "text/plain", "Missing addr or ch param");
    return;
  }

  int addr = server.arg("addr").toInt();
  int ch = server.arg("ch").toInt();

  for (auto *r : relayMgr.getRelays()) {
    if (r->getAddress() == addr) {
      if (r->toggleRelay(ch)) {
        server.send(200, "text/plain", "OK");
      } else {
        server.send(500, "text/plain", "Failed to toggle");
      }
      return;
    }
  }

  server.send(404, "text/plain", "Relay not found");
}
