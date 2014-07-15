#ifndef _TASKHANDLER_H_
#define _TASKHANDLER_H_

#define TIMER_EXCEED_VALUE     400   //smallest value, which every task's period can be divided

enum INITS
{
	INIT_GPIO,
	INIT_VISION
};

enum TASKS
{
	TASK_VISION,
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
