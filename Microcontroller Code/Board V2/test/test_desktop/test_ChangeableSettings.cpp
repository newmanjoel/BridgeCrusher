#include <unity.h>
#include "ChangeableSettings.h"
#include "Arduino.h"

void testIntCreation(void)
{
    int intNumber = 34;
    ChangeableSetting<int> changeableInt = ChangeableSetting<int>(String("Changing Int"), &intNumber, 1, 0, 50, 1);
    TEST_ASSERT_EQUAL(intNumber, changeableInt.value);
}

void willFail(void)
{
    TEST_ASSERT_EQUAL(1,2);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(testIntCreation);
    //RUN_TEST(willFail);
    UNITY_END();
    return 0;
}