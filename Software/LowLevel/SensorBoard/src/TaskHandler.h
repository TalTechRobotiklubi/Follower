#ifndef _TASKHANDLER_H_
#define _TASKHANDLER_H_

#include <stdint.h>

enum Inits
{
	INIT_GPIO,
	INIT_SENSOR,
	INIT_CAN,
	INIT_USART,
	INIT_DRIVE,
	INIT_DATALAYER
};

enum Tasks
{
	TASK_DATAHANDLER,
	TASK_CAN,
	TASK_USART,
	TASK_DRIVE,
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
	TASK_LED
};

typedef struct
{
	enum Inits id;
	void (*taskPointer)();
}
INIT_STRUCT;

typedef struct
{
	enum Tasks id;
	uint32_t period;
	uint32_t offset;
	void (*taskPointer)();
}
TASK_STRUCT;

/*global variables*/
extern const INIT_STRUCT TaskHandler_tableOfInits[];
extern const TASK_STRUCT TaskHandler_tableOfTasks[];

/*global functions*/
void TaskHandler_init(void);
void TaskHandler_run(void);
void TaskHandler_setPreviousTicks(uint32_t ticks);

#endif /* _TASKHANDLER_H_ */
