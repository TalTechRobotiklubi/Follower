#include "stm32f10x.h"
#include "TASK_HANDLER.h"
#include "SystemTicks.h"
#include "GPIO.h"
#include "CAN.h"
#include "ANALOG.h"
#include "QEI.h"
#include "MODULATION.h"

/*Table of initialization tasks. All functions in tables are called once after initializing system clock.
 * With adding new init check that id is corresponding to enum value.*/
const INIT_STRUCT TaskHandler_tableOfInits[] = {
		/*id              	   taskPointer */
		{INIT_GPIO,				GPIO_init   },
		{INIT_CAN,				CAN_init    }
};
#define NUMBER_OF_INITS  (sizeof(TaskHandler_tableOfInits) / sizeof(INIT_STRUCT))

/*Table of run tasks. Add new task here if necessary.
 *Check that id is corresponding to enum value.*/
const TASK_STRUCT TaskHandler_tableOfTasks[] = {
		/*id              period (*100us)   offset (*100us)   taskPointer */
		{TASK_CAN,		  	 200,            0,            	CAN_task 			},
		{TASK_QEI,	          20,            1,         	QEI_task    		},
		{TASK_ANALOG,	     100,            0,          	Analog_task   		},
		{TASK_LED,		  	2000,            0,            	GPIO_TASK_blinkLED 	},
		{TASK_MODULATION,	  20,            3,         	Modulation_task     }
};
#define NUMBER_OF_TASKS  (sizeof(TaskHandler_tableOfTasks) / sizeof(TASK_STRUCT))

/*private function declarations*/
uint8_t checkIfTimeForTask(TASK_STRUCT task, uint32_t time);


void TaskHandler_init(void)
{
	int i;
	// initialize system timer for 1 us periodic interrupt, clock source for timer is 48MHz
	SysTick_Config(4800);
	// call initializing functions
	for(i = 0; i < NUMBER_OF_INITS; i++)
	{
		(TaskHandler_tableOfInits[i].taskPointer)();
	}
}


uint8_t checkIfTimeForTask(TASK_STRUCT task, uint32_t time)
{
	uint32_t period = TaskHandler_tableOfTasks[task.id].period;
	uint32_t offset = TaskHandler_tableOfTasks[task.id].offset;
	uint8_t result = 0;

	if ((time % period) == offset)
	{
		result = 1;
	}
	return result;
}

void TaskHandler_run(void)
{
	static uint32_t prev_timer = 100;  // set something different than 0
	uint32_t timer;
	uint8_t i;

	// reset system ticks as init increased it
	systemTicks = 0;

	//run infinitely and wait timer ticks every 1 ms
	while (1)
	{
		timer = systemTicks;

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
