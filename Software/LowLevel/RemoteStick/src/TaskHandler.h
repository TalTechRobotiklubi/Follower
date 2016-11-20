#ifndef _TASKHANDLER_H_
#define _TASKHANDLER_H_

#include <stdint.h>

enum Inits
{
	INIT_GPIO,
	INIT_BUTTON,
	INIT_POWER,
	INIT_USART,
	INIT_LCD,
	INIT_LOGIC,
	INIT_DATAHANDLER
};

enum Tasks
{
	TASK_DATAHANDLER,
	TASK_USART,
	TASK_BUTTON,
	TASK_POWER,
	TASK_LED,
	TASK_LOGIC,
	TASK_LCD
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
