#include "TaskHandler.h"

#include "stm32f4xx.h"

#include "SystemTicks.h"
#include "GPIO.h"
#include "Sensor.h"
#include "USART.h"
#include "CAN.h"
#include "Drive.h"
#include "DataLayer.h"

/*Table of initialization tasks. All functions in tables are called once after initializing system clock.
 * With adding new init check that id is corresponding to enum value.*/
const INIT_STRUCT TaskHandler_tableOfInits[] = {
		/*id              	   taskPointer */
		{INIT_GPIO,				GPIO_init   },
		{INIT_SENSOR,			Sensor_init },
		{INIT_CAN,				CAN_init    },
		{INIT_USART,			USART_init  },
		{INIT_DRIVE,			Drive_init	},
		{INIT_DATALAYER,		DL_init		},
};
#define NUMBER_OF_INITS  (sizeof(TaskHandler_tableOfInits) / sizeof(INIT_STRUCT))

/*Table of run tasks. Add new task here if necessary.
 *Check that id is corresponding to enum value.*/
const TASK_STRUCT TaskHandler_tableOfTasks[] = {
		/*id              	  period (ms)   offset (ms)   taskPointer */
		{TASK_DATAHANDLER,			5,			0,			DL_task								},
		{TASK_CAN,		  	  	  	5,		    1,			CAN_task							},
		{TASK_USART,		  	    5,		    2,			USART_task							},
		{TASK_DRIVE,			   20,			3,			Drive_task							},
		{TASK_SENSOR1_START,	  200,			0,			Sensor_TASK_startMeasurement1		},
		{TASK_SENSOR1_READ,		  200,		   49,			Sensor_TASK_readDistance1			},
		{TASK_SENSOR2_START,	  200,		   50,			Sensor_TASK_startMeasurement2		},
		{TASK_SENSOR2_READ,		  200,		   99,			Sensor_TASK_readDistance2			},
		{TASK_SENSOR3_START,	  200,		  100,			Sensor_TASK_startMeasurement3		},
		{TASK_SENSOR3_READ,		  200,		  149,			Sensor_TASK_readDistance3			},
		{TASK_SENSOR4_START,	  200,		  150,			Sensor_TASK_startMeasurement4		},
		{TASK_SENSOR4_READ,	  	  200,		  199,			Sensor_TASK_readDistance4			},
		{TASK_SENSOR5_START,	  200,			0,			Sensor_TASK_startMeasurement5		},
		{TASK_SENSOR5_READ,		  200,		   49,			Sensor_TASK_readDistance5			},
		{TASK_SENSOR6_START,	  200,		   50,			Sensor_TASK_startMeasurement6		},
		{TASK_SENSOR6_READ,		  200,		   99,			Sensor_TASK_readDistance6			},
		{TASK_SENSOR7_START,	  200,		  100,			Sensor_TASK_startMeasurement7		},
		{TASK_SENSOR7_READ,		  200,		  149,			Sensor_TASK_readDistance7			},
		{TASK_SENSOR8_START,	  200,		  150,			Sensor_TASK_startMeasurement8		},
		{TASK_SENSOR8_READ,	      200,		  199,			Sensor_TASK_readDistance8			},
		{TASK_LED,				  400,			0,			GPIO_TASK_blinkLED	    			},
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
