#ifndef _TASKHANDLER_H_
#define _TASKHANDLER_H_

enum INITS
{
	INIT_GPIO,
	INIT_VISION,
	INIT_CAN
};

enum TASKS
{
	TASK_VISION,
	TASK_CAN,
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
	uint32_t period;
	uint32_t offset;
	void (*taskPointer)();
}
TASK_STRUCT;

/*global variables*/
extern const INIT_STRUCT TaskHandler_tableOfInits[];
extern const TASK_STRUCT TaskHandler_tableOfTasks[];

/*global functions*/
extern void TaskHandler_init(void);
extern void TaskHandler_run(void);


#endif /* _TASKHANDLER_H_ */
