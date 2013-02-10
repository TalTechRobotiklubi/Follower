#include "stm32f10x.h"
#include "TaskHandler.h"
#include "CAN.h"
#include "GPIO.h"
#include "USART.h"
#include "DataHandler.h"
#include "OLED_LCD.h"
#include "Sensor.h"

/*Task table. Add new task here if necessary. When adding period then be
 *convince that period is not bigger than TIMER_EXCEED_VALUE and divides with it.
 *With adding new task check that id is corresponding to enum value.*/
const TASK_STRUCT TaskHandler_tableOfTasks[] = {
		/*id              	  period (ms)   offset (ms)   taskPointer */
		{TASK_CAN,				    5,          0,          CAN_TASK 			    },
		{TASK_LED,				  400,			4,			GPIO_TASK_blinkLED	    },
		{TASK_CONTROL,			   10,          4,			GPIO_TASK_readButtons   },
		{TASK_UART,				    5,		    2,			USART_TASK				},
		{TASK_DATA,				   10,			4,          DataHandler_TASK		},
	//	{TASK_SENSOR1_START,	  200,			0,			Sensor_TASK_startMeasurement1		},
	//	{TASK_SENSOR1_READ,		  200,		   90,			Sensor_TASK_readDistance1			},
		{TASK_SENSOR2_START,	  200,		  100,			Sensor_TASK_startMeasurement2		},
		{TASK_SENSOR2_READ,		  200,		  160,			Sensor_TASK_readDistance2			},
		{TASK_LCD,				  200,		  170,			OLED_LCD_TASK			}
};
#define NUMBER_OF_TASKS  (sizeof(TaskHandler_tableOfTasks) / sizeof(TASK_STRUCT))

/*private function declarations*/
uint8_t checkIfTimeForTask(TASK_STRUCT task, uint16_t time);

/* keeps the amount of 1 ms resolution timer ticks*/
volatile uint16_t taskHandler_timerTicks = 0;


void TaskHandler_init(void)
{
	// initialize system timer for 1 ms periodic interrupt, clock source for timer is 48MHz
	SysTick_Config(48000);
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

	//run infinitely and wait timer ticks every 1 ms
	while (1)
	{
		timer = taskHandler_timerTicks;

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
