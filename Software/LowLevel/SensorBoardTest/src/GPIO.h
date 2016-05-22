#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

extern void   GPIO_init(void);
extern void   GPIO_TASK_blinkLED(void);

extern int32_t blinkLedCallCount;

#endif /*GPIO_H_*/
