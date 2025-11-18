#include "createGrafanaMessage.h"
#include <WiFi.h>
#include <constants.h>

String create_grafana_message(float temperature, float humidity, float co2, const char* sensorType) {
  unsigned long long timestamp = time(nullptr) * 1000000000ULL;

  char device_name[64]={0};

  // If this is mesh sensor data, use sensor ID as device name
  if (strncmp(sensorType, "mesh_", 5) == 0) {
    snprintf(device_name, sizeof(device_name), "%s", sensorType);
  } else {
    // For local sensors, use gateway MAC
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    snprintf(device_name, sizeof(device_name), "moni-%s", mac.c_str());
  }

  // InfluxDB line protocol: measurement,tag1=value1,tag2=value2 field1=value1,field2=value2 timestamp
  String message = "medicionesCO2,device=" + String(device_name) +
                   " temp=" + String(temperature, 2) +
                   ",hum=" + String(humidity, 2) +
                   ",co2=" + String(co2) +
                   " " + String(timestamp);

  return message;
}
