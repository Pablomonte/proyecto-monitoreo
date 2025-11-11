#ifndef CREATE_GRAFANA_MESSAGE_H
#define CREATE_GRAFANA_MESSAGE_H

#include <Arduino.h>

String create_grafana_message(float temperature, float humidity, float co2, const char* sensorType = "Unknown");

#endif // CREATE_GRAFANA_MESSAGE_H