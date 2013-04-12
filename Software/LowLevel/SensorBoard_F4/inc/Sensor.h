#ifndef SENSOR_H_
#define SENSOR_H_

#include "GPIO.h"

/*Connect sensor to URF echo pin*/
#define DISTANCE_SENSOR_1 	URF1_ECHO


extern void Sensor_init(void);

extern void Sensor_TASK_startMeasurement1(void);
extern void Sensor_TASK_startMeasurement2(void);
extern void Sensor_TASK_startMeasurement3(void);
extern void Sensor_TASK_startMeasurement4(void);
extern void Sensor_TASK_readDistance1(void);
extern void Sensor_TASK_readDistance2(void);
extern void Sensor_TASK_readDistance3(void);
extern void Sensor_TASK_readDistance4(void);

#endif /*SENSOR_H_*/
