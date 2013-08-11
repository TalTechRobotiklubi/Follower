#include "stm32f10x.h"
#include "TASK_HANDLER.h"
#include "PARAMS.h"
#include "GPIO.h"

/*Task table. Add new task here if necessary. When adding period then be
 *convince that period is not bigger than TIMER_EXCEED_VALUE and divides with it*/
const struct TASK_STRUCT TaskHandler_tableOfTasks[] = {
		/*id              period (*100us)   offset (*100us)   taskPointer */
		{TASK_LED,		  	2000,            0,            	GPIO_TASK_blinkLED 	},
		{TASK_CAN,		  	1000,            0,            	CAN_TASK 	},
		{TASK_RAMP,		  	 100,            0,            	RAMP_TASK 	},
		{TASK_QEI,	          20,            1,         	QEI_TASK   },
		{TASK_ANALOG,	     100,            0,          	ANALOG_TASK   },
		{TASK_PID,	          20,            2,         	PID_TASK   },
		{TASK_MODULATION,	  20,            3,         	MODULATION_TASK   }
};
#define NUMBER_OF_TASKS  (sizeof(TaskHandler_tableOfTasks) / sizeof(struct TASK_STRUCT))

/* keeps the amount of 1 ms resolution timer ticks*/
volatile uint32_t taskHandler_timerTicks = 0;

void TaskHandler_init(void)
{
	// initialize system timer for 1 us periodic interrupt, clock source for timer is 48MHz
	SysTick_Config(4800);
}


uint8_t checkIfTimeForTask(struct TASK_STRUCT task, uint16_t time)
{
	uint32_t period = TaskHandler_tableOfTasks[task.id].period;
	uint32_t offset = TaskHandler_tableOfTasks[task.id].offset;
	uint16_t result = 0;

	if ((time % period) == offset)
	{
		result = 1;
	}
	return result;
}

void TaskHandler_run(void)
{
	static uint32_t prev_timer = TIMER_EXCEED_VALUE;  // set something different than 0
	uint32_t timer;
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

void SysTick_Handler(void)
{
	taskHandler_timerTicks++;
	if (taskHandler_timerTicks >= TIMER_EXCEED_VALUE)
	{
		taskHandler_timerTicks = 0;
	}
}
