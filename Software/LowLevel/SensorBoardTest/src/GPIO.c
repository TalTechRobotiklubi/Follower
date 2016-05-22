#include "GPIO.h"

int32_t blinkLedCallCount = 0;

/*function definitions*/
void GPIO_init(void)
{
}

void GPIO_TASK_blinkLED(void)
{
	++blinkLedCallCount;
}



