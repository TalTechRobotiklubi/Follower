/* Includes */
#include <stddef.h>
#include "stm32l1xx.h"

#include "TaskHandler.h"

int main(void)
{
	TaskHandler_init();
	while(1)
		TaskHandler_run();
	return 0;
}
