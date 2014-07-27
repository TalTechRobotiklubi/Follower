#include "stm32f4xx.h"
#include "TaskHandler.h"
#include "GPIO.h"
#include "Vision.h"
#include "CAN.h"

/*Table of initialization tasks. All functions in tables are called once after initializing system clock.
 * With adding new init check that id is corresponding to enum value.*/
const INIT_STRUCT TaskHandler_tableOfInits[] = {
		/*id              	   taskPointer */
		{INIT_GPIO,				GPIO_init },
		{INIT_VISION,			Vision_init }
};
#define NUMBER_OF_INITS  (sizeof(TaskHandler_tableOfInits) / sizeof(INIT_STRUCT))

/*Table of run tasks. Add new task here if necessary. When adding period then be
 *convince that period is not bigger than TIMER_EXCEED_VALUE and divides with it.
 *With adding new task check that id is corresponding to enum value.*/
const TASK_STRUCT TaskHandler_tableOfTasks[] = {
		/*id              	  period (ms)   offset (ms)   taskPointer */
		{TASK_VISION,			   20,			0,			Vision_TASK      	    },
		{TASK_CAN,			       20,			2,			CAN_TASK      	        },
		{TASK_LED,				  400,			0,			GPIO_TASK_blinkLED	    },
};
#define NUMBER_OF_TASKS  (sizeof(TaskHandler_tableOfTasks) / sizeof(TASK_STRUCT))

/*private function declarations*/
uint8_t checkIfTimeForTask(TASK_STRUCT task, uint16_t time);

/* keeps the amount of 1 ms resolution system ticks*/
volatile uint16_t taskHandler_systemTicks = 0;


void TaskHandler_init(void)
{
	uint8_t i;
	// initialize system timer for 1 ms periodic interrupt
	//SysTick_Config(48000);
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while (1);
	}
	// call initializing functions
	for(i = 0; i < NUMBER_OF_INITS; i++)
	{
		(TaskHandler_tableOfInits[i].taskPointer)();
	}
}


uint8_t checkIfTimeForTask(TASK_STRUCT task, uint16_t time)
{
	uint16_t period = TaskHandler_tableOfTasks[task.id].period;
	uint16_t offset = TaskHandler_tableOfTasks[task.id].offset;
	uint8_t result = 0;

	if ((time % period) == offset)
	{
		result = 1;
	}
	return result;
}

void TaskHandler_run(void)
{
	static uint16_t prev_timer = TIMER_EXCEED_VALUE;  // set something different than 0
	uint16_t timer;
	uint8_t i;

	// reset system ticks as init increased it
	taskHandler_systemTicks = 0;

	//run infinitely and wait timer ticks every 1 ms
	while (1)
	{
		timer = taskHandler_systemTicks;

		// check task execution only if 1 ms has elapsed
		if (timer != prev_timer)
		{
			// go through all tasks and check if it is needed to execute them
			for(i = 0; i < NUMBER_OF_TASKS; i++)
			{
				if (checkIfTimeForTask(TaskHandler_tableOfTasks[i], timer))
				{
					(TaskHandler_tableOfTasks[i].taskPointer)();
				}
			}
			// do not come back here during this ms
			prev_timer = timer;
		}
	}
}
