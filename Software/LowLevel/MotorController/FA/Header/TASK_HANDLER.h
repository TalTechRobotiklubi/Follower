#ifndef _TASKHANDLER_H_
#define _TASKHANDLER_H_

#define TIMER_EXCEED_VALUE     400000   //smallest value, which every task's period can be divided

enum TASKS
{
	TASK_UART,
	TASK_CAN,
	TASK_RAMP,
	TASK_QEI,
	TASK_ANALOG,
	TASK_PID,
	TASK_MODULATION     
};

struct TASK_STRUCT
{
	enum TASKS id;
	uint32_t period;
	uint32_t offset;
	void (*taskPointer)();
};

/*global variables*/
extern const struct TASK_STRUCT TaskHandler_tableOfTasks[];
volatile extern uint32_t taskHandler_timerTicks;

/*global functions*/
extern void TaskHandler_init(void);
extern void TaskHandler_run(void);
void SysTick_Handler(void);

#endif /* _TASKHANDLER_H_ */