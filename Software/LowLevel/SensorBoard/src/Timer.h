#ifndef TIMER_H_
#define TIMER_H_

#include "stm32f4xx.h"
#include "Typedef.h"

typedef enum
{
	TIMER2_ID,
	TIMER3_ID,
	TIMER4_ID,
	NUM_OF_TIMERS
}
Timer_IdDef;


void Timer_setOneShotCounter(Timer_IdDef timer, uint32_t timeus, Boolean enableInterrupt);
void Timer_startTimer(Timer_IdDef timer);
void Timer_stopTimer(Timer_IdDef timer);
uint16_t Timer_getTimerValue(Timer_IdDef timer);

void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);

#endif /*Timer_H_*/
