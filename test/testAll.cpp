#include <unity.h>

#ifndef ARDUINO
#include <ArduinoFake.h>
using namespace fakeit;
#else
#include <Arduino.h>
#endif

extern void testCreateConfigFile();
extern void testHandleMediciones();
extern void testHandleConfiguracion();
extern void testSendDataGrafana();
extern void testCreateGrafanaMessage();
extern void testCheckForUpdates();
extern void testGetLatestReleaseTag();
extern void runRuleConditionTests();
extern void runSensorIdentityTests();
extern void runAutomationFlowTests();
extern void runRuleLoaderTests();

void setUp() {
#ifndef ARDUINO
    When(Method(ArduinoFake(), millis)).AlwaysReturn(1000);
#endif
}
void tearDown() {}

#ifndef ARDUINO
int main() {
    UNITY_BEGIN();
    RUN_TEST(testCreateConfigFile);
    RUN_TEST(testHandleMediciones);
    RUN_TEST(testHandleConfiguracion);
    RUN_TEST(testCheckForUpdates);
    RUN_TEST(testCreateGrafanaMessage);
    RUN_TEST(testSendDataGrafana);
    RUN_TEST(testGetLatestReleaseTag);

    runRuleConditionTests();
    runSensorIdentityTests();
    runAutomationFlowTests();
    runRuleLoaderTests();

    return UNITY_END();
}
#else
void setup() {
    // Damos un pequeño retraso para que el monitor serie se conecte
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(testCreateConfigFile);
    RUN_TEST(testHandleMediciones);
    RUN_TEST(testHandleConfiguracion);
    RUN_TEST(testCheckForUpdates);
    RUN_TEST(testCreateGrafanaMessage);
    RUN_TEST(testSendDataGrafana);
    RUN_TEST(testGetLatestReleaseTag);

    runRuleConditionTests();
    runSensorIdentityTests();
    runAutomationFlowTests();
    runRuleLoaderTests();
    UNITY_END();
}
void loop() {}
#endif
//    RUN_TEST(testCreateConfigFile);
//    RUN_TEST(testHandleMediciones);
//    RUN_TEST(testHandleConfiguracion);
//    RUN_TEST(testCheckForUpdates);
//    RUN_TEST(testCreateGrafanaMessage);
//    RUN_TEST(testSendDataGrafana);
//    RUN_TEST(testGetLatestReleaseTag);    
//    UNITY_END();
//}

//void loop() {}
