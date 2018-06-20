#include "unity.h"

void test_dummy(void)
{
    TEST_ASSERT_EQUAL_HEX8(33, 42);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dummy);
    return UNITY_END();
}
