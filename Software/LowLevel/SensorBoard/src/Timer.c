#include "Timer.h"
#include "TaskHandler.h"


void Timer_setOneShotCounter(Timer_IdDef timer, uint32_t timeus, Boolean enableInterrupt)
{
	NVIC_InitTypeDef 		 NVIC_InitStruct;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
	uint16_t PrescalerValue = (SystemCoreClock / 1000000) - 1; // counter resolution 1 micro sec

	//Interrupt initialization
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

	//Time base
	TIM_TimeBaseStruct.TIM_Period = timeus;
	TIM_TimeBaseStruct.TIM_Prescaler = 0; // overwritten by TIM_PrescalerConfig() call
	TIM_TimeBaseStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;

	switch(timer)
	{
		case TIMER1_ID:
			RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
			/*set interrupt*/
			NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
			NVIC_Init(&NVIC_InitStruct);

			TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);
			TIM_PrescalerConfig(TIM1, PrescalerValue, TIM_PSCReloadMode_Immediate);
			TIM_SelectOnePulseMode(TIM1, TIM_OPMode_Single);
			Timer_stopTimer(TIMER1_ID);
			break;
		case TIMER2_ID:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
			/*set interrupt*/
			NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
			NVIC_Init(&NVIC_InitStruct);

			TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);
			TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);
			TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single);
			Timer_stopTimer(TIMER2_ID);
			break;
		case TIMER3_ID:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
			/*set interrupt*/
			NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
			NVIC_Init(&NVIC_InitStruct);

			TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);
			TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
			TIM_SelectOnePulseMode(TIM3, TIM_OPMode_Single);
			Timer_stopTimer(TIMER3_ID);
			break;
		default:
			break;
	}
}

void Timer_startTimer(Timer_IdDef timer)
{
	switch(timer)
	{
		case TIMER1_ID:
			/*reset counter value*/
			TIM_SetCounter(TIM1, 0);
			/* TIM enable counter */
			TIM_Cmd(TIM1, ENABLE);
			/* TIM IT enable */
			TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
			break;
		case TIMER2_ID:
			/*reset counter value*/
			TIM_SetCounter(TIM2, 0);
			/* TIM enable counter */
			TIM_Cmd(TIM2, ENABLE);
			/* TIM IT enable */
			TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
			break;
		case TIMER3_ID:
			/*reset counter value*/
			TIM_SetCounter(TIM3, 0);
			/* TIM enable counter */
			TIM_Cmd(TIM3, ENABLE);
			/* TIM IT enable */
			TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
			break;
		default:
			break;
	}

}

void Timer_stopTimer(Timer_IdDef timer)
{
	switch(timer)
	{
		case TIMER1_ID:
			/* TIM IT disable */
			TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
			/* TIM disable counter */
			TIM_Cmd(TIM1, DISABLE);
			break;
		case TIMER2_ID:
			/* TIM IT disable */
			TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
			/* TIM disable counter */
			TIM_Cmd(TIM2, DISABLE);
			break;
		case TIMER3_ID:
			/* TIM IT disable */
			TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
			/* TIM disable counter */
			TIM_Cmd(TIM3, DISABLE);
			break;
		default:
			break;
	}
}

uint16_t Timer_getTimerValue(Timer_IdDef timer)
{
	uint16_t value;
	switch(timer)
	{
		case TIMER1_ID:
			value = TIM_GetCounter(TIM1);
			break;
		case TIMER2_ID:
			value = TIM_GetCounter(TIM2);
			break;
		case TIMER3_ID:
			value = TIM_GetCounter(TIM3);
			break;
		default:
			value = 0;
			break;
	}
	return value;
}


void TIM1_UP_IRQHandler(void)
{

}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		//counter = taskHandler_timerTicks;

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}

}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		//counter = taskHandler_timerTicks;

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}

}

