#ifndef TIMER_H_
#define TIMER_H_

#include "stm32f10x.h"
#include "Typedef.h"

typedef enum
{
	TIMER1_ID,
	TIMER2_ID,
	TIMER3_ID,
	NUM_OF_TIMERS
}
Timer_IdDef;


extern void 	Timer_setOneShotCounter(Timer_IdDef timer, uint32_t timeus, Boolean enableInterrupt);
extern void 	Timer_startTimer(Timer_IdDef timer);
extern void 	Timer_stopTimer(Timer_IdDef timer);
extern uint16_t Timer_getTimerValue(Timer_IdDef timer);

void TIM1_UP_IRQHandler(void);
void TIM2_IRQHandler(void);

#endif /*Timer_H_*/
