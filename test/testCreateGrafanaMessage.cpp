// Native-test helper that mirrors the InfluxDB line-protocol format used by
// the firmware's String-based create_grafana_message(). Kept independent of
// Arduino.h so it can run under [env:native_test].

#include <cstddef>
#include <cstdio>

void createGrafanaMessage(char* buffer, size_t bufferSize, const char* device,
                          float temperature, float humidity, float co2,
                          unsigned long long timestamp) {
    snprintf(buffer, bufferSize,
             "medicionesCO2,device=%s temp=%.2f,hum=%.2f,co2=%.2f %llu",
             device, temperature, humidity, co2, timestamp);
}
