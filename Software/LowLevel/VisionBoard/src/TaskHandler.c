#include "TaskHandler.h"

#include "stm32f4xx.h"

#include "SystemTicks.h"
#include "GPIO.h"
#include "CAN.h"
#include "usb.h"
#include "servo.h"
#include "DataLayer.h"

/*Table of initialization tasks. All functions in tables are called once after initializing system clock.
 * With adding new init check that id is corresponding to enum value.*/
const INIT_STRUCT TaskHandler_tableOfInits[] = {
		/*id              	   taskPointer */
		{INIT_GPIO,				GPIO_init   },
		{INIT_CAN,				CAN_init    },
		{INIT_USB,				USB_init	},
		{INIT_SERVO,			Servo_init	},
		{INIT_DATALAYER,		DL_init		}
};
#define NUMBER_OF_INITS  (sizeof(TaskHandler_tableOfInits) / sizeof(INIT_STRUCT))

/*Table of run tasks. Add new task here if necessary.
 *Check that id is corresponding to enum value.*/
const TASK_STRUCT TaskHandler_tableOfTasks[] = {
		/*id              	  period (ms)   offset (ms)   taskPointer */
		{TASK_DATAHANDLER,			5,			0,			DL_task				},
		{TASK_CAN,			        5,		    1,			CAN_task      		},
		{TASK_USB,				    5,		    2,			USB_task			},
		{TASK_LED,				  400,		    0,			GPIO_task			},
		{TASK_SERVO,			   20,			5,			Servo_task			}
};
#define NUMBER_OF_TASKS  (sizeof(TaskHandler_tableOfTasks) / sizeof(TASK_STRUCT))

/*private variable declarations*/
static uint32_t prevTicks = 100;  // set something different than 0

/*private function declarations*/
static uint8_t checkIfTimeForTask(TASK_STRUCT task, uint32_t time);

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
	// reset system ticks as this method (TaskHandler_init) increased it
	systemTicks = 0;
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
	uint32_t timer;
	uint8_t i;

	// wait timer ticks every 1 ms
	timer = systemTicks;

	// check task execution only if 1 ms has elapsed
	if (timer != prevTicks)
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
		prevTicks = timer;
	}
}

void TaskHandler_setPreviousTicks(uint32_t ticks)
{
	prevTicks = ticks;
}
