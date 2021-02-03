#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"

P4_PRIVATE(void) test_version(void) {
    P4_String version = P4_Version();
    TEST_ASSERT_EQUAL_STRING(version, "0.1.0");
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_version);

    return UNITY_END();
}
