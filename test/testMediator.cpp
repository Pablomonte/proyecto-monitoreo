/**
 * test/testMediator.cpp
 *
 * Unified test runner for native platform (pio test -e native_test).
 *
 * Includes:
 *   - Rule condition tests (1 / 2 / 3 / 4 conditions)
 *   - Sensor identity tests (SensorKey, counter, MAC derivation)
 */

#include <unity.h>

// Declared in their respective files
extern void runRuleConditionTests();
extern void runSensorIdentityTests();

void setUp()    {}
void tearDown() {}

int main() {
    UNITY_BEGIN();
    runRuleConditionTests();
    runSensorIdentityTests();
    return UNITY_END();
}
