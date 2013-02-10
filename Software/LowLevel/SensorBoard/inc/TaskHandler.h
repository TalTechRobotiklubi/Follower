#ifndef _TASKHANDLER_H_
#define _TASKHANDLER_H_

#define TIMER_EXCEED_VALUE     400   //smallest value, which every task's period can be divided

enum TASKS
{
	TASK_CAN,
	TASK_LED,
	TASK_CONTROL,
	TASK_UART,
	TASK_DATA,
//	TASK_SENSOR1_START,
//	TASK_SENSOR1_READ,
	TASK_SENSOR2_START,
	TASK_SENSOR2_READ,
	TASK_LCD
};

typedef struct
{
	enum TASKS id;
	uint16_t period;
	uint16_t offset;
	void (*taskPointer)();
}
TASK_STRUCT;

/*global variables*/
extern const TASK_STRUCT TaskHandler_tableOfTasks[];
volatile extern uint16_t taskHandler_timerTicks;

/*global functions*/
extern void TaskHandler_init(void);
extern void TaskHandler_run(void);


#endif /* _TASKHANDLER_H_ */
