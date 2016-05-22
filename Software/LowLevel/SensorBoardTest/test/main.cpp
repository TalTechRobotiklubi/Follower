#include <iostream>

#include <UnitTest++.h>

extern "C" {
#include "SystemTicks.h"
#include "TaskHandler.h"
#include "GPIO.h"
}

TEST(SystemTickDefault)
{
    CHECK_EQUAL(0, systemTicks);
}

TEST(TaskHandler_blinkLedCallCount)
{
	TaskHandler_setPreviousTicks(100);
	systemTicks = 0;

	TaskHandler_run();
    CHECK_EQUAL(1, blinkLedCallCount);
}

int main(void){

	return UnitTest::RunAllTests();
}
