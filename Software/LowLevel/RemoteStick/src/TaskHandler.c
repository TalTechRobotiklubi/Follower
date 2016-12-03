#include "TaskHandler.h"

#include "SystemTicks.h"
#include "GPIO.h"
#include "button.h"
#include "power.h"
#include "logic.h"
#include "st7735.h"
#include "USART.h"
#include "DataLayer.h"

/*Table of initialization tasks. All functions in tables are called once after initializing system clock.
 * With adding new init check that id is corresponding to enum value.*/
const INIT_STRUCT TaskHandler_tableOfInits[] = {
		/*id              	   taskPointer */
		{INIT_GPIO,				GPIO_init   },
		{INIT_BUTTON,			Button_Init },
		{INIT_POWER,			Power_Init  },
		{INIT_USART,			USART_init  },
		{INIT_LCD,				ST7735_Init },
		{INIT_LOGIC,			Logic_Init  },
		{INIT_DATAHANDLER,		DL_init		}
};
#define NUMBER_OF_INITS  (sizeof(TaskHandler_tableOfInits) / sizeof(INIT_STRUCT))

/*Table of run tasks. Add new task here if necessary.
 *Check that id is corresponding to enum value.*/
const TASK_STRUCT TaskHandler_tableOfTasks[] = {
		/*id              	  period (ms)   offset (ms)   taskPointer */
		{TASK_DATAHANDLER,		  5,			0,			DL_task							},
		{TASK_USART,			  5,			4,			USART_task						},
		{TASK_BUTTON,			 10,			0,			Button_Task						},
		{TASK_POWER,			 10,			1,			Power_Task						},
		{TASK_LED,				400,			0,			GPIO_TASK_blinkLED	    		},
		{TASK_LOGIC,			 40,			2,			Logic_Task						},
		{TASK_LCD,				 10,			3,			ST7735_Task						}
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
