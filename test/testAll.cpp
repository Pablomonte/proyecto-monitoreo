#include <unity.h>

extern void testCreateConfigFile();
extern void testHandleMediciones();
extern void testHandleConfiguracion();
extern void testSendDataGrafana();
extern void testCreateGrafanaMessage();
extern void testCheckForUpdates();
extern void testGetLatestReleaseTag();
extern void testWifiPasswordLength_EmptyIsValid();
extern void testWifiPasswordLength_AcceptsWPA2Range();
extern void testWifiPasswordLength_RejectsTooShort();
extern void testWifiPasswordLength_RejectsTooLong();
extern void testAdminPassword_RejectsShort();
extern void testAdminPassword_RejectsTooLong();
extern void testAdminPassword_RejectsSameAsCurrent();
extern void testAdminPassword_RequiresCurrentWhenConfigured();
extern void testAdminPassword_AllowsFirstSetWhenUnconfigured();

void setUp() {}
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
    RUN_TEST(testWifiPasswordLength_EmptyIsValid);
    RUN_TEST(testWifiPasswordLength_AcceptsWPA2Range);
    RUN_TEST(testWifiPasswordLength_RejectsTooShort);
    RUN_TEST(testWifiPasswordLength_RejectsTooLong);
    RUN_TEST(testAdminPassword_RejectsShort);
    RUN_TEST(testAdminPassword_RejectsTooLong);
    RUN_TEST(testAdminPassword_RejectsSameAsCurrent);
    RUN_TEST(testAdminPassword_RequiresCurrentWhenConfigured);
    RUN_TEST(testAdminPassword_AllowsFirstSetWhenUnconfigured);
    return UNITY_END();
}
//void setup() {
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
