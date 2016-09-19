#include <iostream>

#include <UnitTest++.h>

extern "C" {
#include "SystemTicks.h"
#include "TaskHandler.h"
#include "GPIO.h"
}

int main(void){

	return UnitTest::RunAllTests();
}
