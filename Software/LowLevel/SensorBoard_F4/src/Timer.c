#include "Timer.h"
#include "TaskHandler.h"


void Timer_setOneShotCounter(Timer_IdDef timer, uint32_t timeus, Boolean enableInterrupt)
{
	NVIC_InitTypeDef 		 NVIC_InitStruct;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
	uint16_t PrescalerValue; //= (SystemCoreClock / 1000000) - 1; // counter resolution 1 micro sec

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
		case TIMER2_ID:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
			/*set interrupt*/
			if (enableInterrupt)
			{
				NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
				NVIC_Init(&NVIC_InitStruct);
			}

			TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);
			PrescalerValue = ((SystemCoreClock / 2) / 1000000) - 1; // counter resolution 1 micro sec
			TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);
			TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single);
			Timer_stopTimer(TIMER2_ID);
			break;
		case TIMER3_ID:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
			/*set interrupt*/
			if (enableInterrupt)
			{
				NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
				NVIC_Init(&NVIC_InitStruct);
			}

			TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);
			PrescalerValue = ((SystemCoreClock / 2) / 1000000) - 1; // counter resolution 1 micro sec
			TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
			TIM_SelectOnePulseMode(TIM3, TIM_OPMode_Single);
			Timer_stopTimer(TIMER3_ID);
			break;
		case TIMER4_ID:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
			/*set interrupt*/
			if (enableInterrupt)
			{
				NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
				NVIC_Init(&NVIC_InitStruct);
			}

			TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStruct);
			PrescalerValue = ((SystemCoreClock / 2) / 1000000) - 1; // counter resolution 1 micro sec
			TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);
			TIM_SelectOnePulseMode(TIM4, TIM_OPMode_Single);
			Timer_stopTimer(TIMER4_ID);
			break;
		default:
			break;
	}
}

void Timer_startTimer(Timer_IdDef timer)
{
	switch(timer)
	{
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
		case TIMER4_ID:
			/*reset counter value*/
			TIM_SetCounter(TIM4, 0);
			/* TIM enable counter */
			TIM_Cmd(TIM4, ENABLE);
			/* TIM IT enable */
			TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
			break;
		default:
			break;
	}

}

void Timer_stopTimer(Timer_IdDef timer)
{
	switch(timer)
	{
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
		case TIMER4_ID:
			/* TIM IT disable */
			TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
			/* TIM disable counter */
			TIM_Cmd(TIM4, DISABLE);
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
		case TIMER2_ID:
			value = TIM_GetCounter(TIM2);
			break;
		case TIMER3_ID:
			value = TIM_GetCounter(TIM3);
			break;
		case TIMER4_ID:
			value = TIM_GetCounter(TIM4);
			break;
		default:
			value = 0;
			break;
	}
	return value;
}

/*Timer 2 interrupt*/
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		//counter = taskHandler_timerTicks;

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}

}


/*Timer 3 interrupt*/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		//counter = taskHandler_timerTicks;

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}

}


/*Timer 4 interrupt*/
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		//counter = taskHandler_timerTicks;

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}

}
