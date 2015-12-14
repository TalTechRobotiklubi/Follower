#include "stm32f4xx.h"
#include "TaskHandler.h"
#include "SystemTicks.h"
#include "GPIO.h"
#include "CAN.h"
#include "usb.h"
#include "servo.h"

/*Table of initialization tasks. All functions in tables are called once after initializing system clock.
 * With adding new init check that id is corresponding to enum value.*/
const INIT_STRUCT TaskHandler_tableOfInits[] = {
		/*id              	   taskPointer */
		{INIT_GPIO,				GPIO_init   },
		{INIT_CAN,				CAN_init    },
		{INIT_USB,				USB_init	},
		{INIT_SERVO,			servo_init	}
};
#define NUMBER_OF_INITS  (sizeof(TaskHandler_tableOfInits) / sizeof(INIT_STRUCT))

/*Table of run tasks. Add new task here if necessary.
 *Check that id is corresponding to enum value.*/
const TASK_STRUCT TaskHandler_tableOfTasks[] = {
		/*id              	  period (ms)   offset (ms)   taskPointer */
		{TASK_CAN,			       20,		   10,			CAN_task      	},
		{TASK_USB,				   20,		    0,			USB_task		},
		{TASK_LED,				  200,		    0,			GPIO_task		},
		{TASK_SERVO,			   20,			5,			servo_task		}
};
#define NUMBER_OF_TASKS  (sizeof(TaskHandler_tableOfTasks) / sizeof(TASK_STRUCT))

/*private function declarations*/
uint8_t checkIfTimeForTask(TASK_STRUCT task, uint32_t time);


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
	// delay 1s.
	while (systemTicks < 1500);
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
