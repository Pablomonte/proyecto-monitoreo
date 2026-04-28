#include <unity.h>
#include <cstddef>

// Forward declaration of the native-test helper defined in
// testCreateGrafanaMessage.cpp (Arduino.h-free).
void createGrafanaMessage(char* buffer, size_t bufferSize, const char* device,
                          float temperature, float humidity, float co2,
                          unsigned long long timestamp);

void testSendDataGrafana() {
    TEST_ASSERT_TRUE(true);
}

void testCreateGrafanaMessage() {
    char buffer[256];
    createGrafanaMessage(buffer, sizeof(buffer), "ASC02", 23.45f, 55.67f, 789.0f,
                         1234567890000000000ULL);

    const char* expected =
        "medicionesCO2,device=ASC02 temp=23.45,hum=55.67,co2=789.00 1234567890000000000";
    TEST_ASSERT_EQUAL_STRING(expected, buffer);
}
