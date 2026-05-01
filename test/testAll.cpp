#include <unity.h>
#include <ArduinoFake.h>
using namespace fakeit;

extern void testCreateConfigFile();
extern void testHandleMediciones();
extern void testHandleConfiguracion();
extern void testSendDataGrafana();
extern void testCreateGrafanaMessage();
extern void testCheckForUpdates();
extern void testGetLatestReleaseTag();
extern void runRuleConditionTests();
extern void runSensorIdentityTests();

void setUp() {
    When(Method(ArduinoFake(), millis)).AlwaysReturn(1000);
}
void tearDown() {}

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

    return UNITY_END();
}
//    UNITY_BEGIN();
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
