#ifndef _TASKHANDLER_H_
#define _TASKHANDLER_H_

#include <stdint.h>

#define TIMER_EXCEED_VALUE     400   //smallest value, which every task's period can be divided

enum INITS
{
	INIT_GPIO,
	INIT_SENSOR,
	INIT_CAN,
	INIT_USART,
	INIT_ACCELERATION
};

enum TASKS
{
	TASK_DATAHANDLER,
	TASK_CAN,
	TASK_USART,
	TASK_SENSOR1_START,
	TASK_SENSOR1_READ,
	TASK_SENSOR2_START,
	TASK_SENSOR2_READ,
	TASK_SENSOR3_START,
	TASK_SENSOR3_READ,
	TASK_SENSOR4_START,
	TASK_SENSOR4_READ,
	TASK_SENSOR5_START,
	TASK_SENSOR5_READ,
	TASK_SENSOR6_START,
	TASK_SENSOR6_READ,
	TASK_SENSOR7_START,
	TASK_SENSOR7_READ,
	TASK_SENSOR8_START,
	TASK_SENSOR8_READ,
	TASK_ACCELERATION,
	TASK_LED
};

typedef struct
{
	enum INITS id;
	void (*taskPointer)();
}
INIT_STRUCT;

typedef struct
{
	enum TASKS id;
	uint16_t period;
	uint16_t offset;
	void (*taskPointer)();
}
TASK_STRUCT;

/*global variables*/
extern const INIT_STRUCT TaskHandler_tableOfInits[];
extern const TASK_STRUCT TaskHandler_tableOfTasks[];
volatile extern uint16_t taskHandler_systemTicks;

/*global functions*/
extern void TaskHandler_init(void);
extern void TaskHandler_run(void);


#endif /* _TASKHANDLER_H_ */
