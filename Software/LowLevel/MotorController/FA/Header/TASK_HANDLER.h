#ifndef _TASKHANDLER_H_
#define _TASKHANDLER_H_

#include <stdint.h>

enum INITS
{
	INIT_GPIO,
	INIT_CAN
};

enum TASKS
{
	TASK_CAN,
	TASK_QEI,
	TASK_ANALOG,
	TASK_LED,
	TASK_MODULATION     
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


#endif /* _TASKHANDLER_H_ */
