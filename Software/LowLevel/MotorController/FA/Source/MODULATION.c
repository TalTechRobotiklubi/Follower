#include "MODULATION.h"
#include "MessageId.h"
#include "DataLayer.h"

static void vModulationEnable();
static void vModulationDisable();

//----------------------------------------------------------------------------------------------------------------------------
// Configure peripherals necessary for modulation
//----------------------------------------------------------------------------------------------------------------------------
void vModulationHardwareInit()
{
	TIM_OCInitTypeDef	    TIM_OCInitStruct;
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;
	GPIO_InitTypeDef          GPIO_InitStruct;

	uint16_t u16PrescalerValue = 0;

	//--------------------------------------------------------------------------------------------------------------------------
	// Configure PA3 (PWM output), as push-pull type output
	//--------------------------------------------------------------------------------------------------------------------------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIOA->BRR = GPIO_Pin_3;

	//--------------------------------------------------------------------------------------------------------------------------
	// Configure PA4 and PA6 as outputs to VNH2SP30 directional inputs
	//--------------------------------------------------------------------------------------------------------------------------
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIOA->BRR = GPIO_Pin_4;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIOA->BRR = GPIO_Pin_6;
	//--------------------------------------------------------------------------------------------------------------------------
	// Configure PA7 as output to VNH2SP30 enable input
	//--------------------------------------------------------------------------------------------------------------------------
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIOA->BRR = GPIO_Pin_7;

	//--------------------------------------------------------------------------------------------------------------------------
	// Configure Timer 2 to generate PWM with 10kHz frequency (from 48MHz)
	//--------------------------------------------------------------------------------------------------------------------------
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	u16PrescalerValue = (SystemCoreClock / 48000000) - 1;
	TIM_TimeBaseInitStruct.TIM_Period = 4800;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	TIM_PrescalerConfig(TIM2,u16PrescalerValue, TIM_PSCReloadMode_Immediate);
	TIM_OCStructInit( &TIM_OCInitStruct );
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC4Init(TIM2,&TIM_OCInitStruct);
	TIM_CtrlPWMOutputs(TIM2,ENABLE);

	TIM2->CCR4 = 0;
	vModulationEnable();
}

//----------------------------------------------------------------------------------------------------------------------------
// Cyclic function for modulation. To be executed every 2 milliseconds.
//----------------------------------------------------------------------------------------------------------------------------
void Modulation_task()
{
	int16_t requestSpeed;

	if (CurrentId == Motor1Message)
		DL_getDataWithoutAffectingStatus(DLParamMotor1RequestSpeed, &requestSpeed);
	else if (CurrentId == Motor2Message)
		DL_getDataWithoutAffectingStatus(DLParamMotor2RequestSpeed, &requestSpeed);
	if (requestSpeed != 0)
	{
		if (requestSpeed > 0)
		{
			TIM2->CCR4 = requestSpeed;
			GPIOA->BSRR = GPIO_Pin_4;
			GPIOA->BRR = GPIO_Pin_6;
		}
		else
		{
			TIM2->CCR4 = -requestSpeed;
			GPIOA->BRR = GPIO_Pin_4;
			GPIOA->BSRR = GPIO_Pin_6;
		}
	}
	else
	{
		TIM2->CCR4 = 0;
	}
}


//----------------------------------------------------------------------------------------------------------------------------
// Enable modulation
//----------------------------------------------------------------------------------------------------------------------------
void vModulationEnable()
{
	GPIOA->BSRR = GPIO_Pin_7;
	TIM_ARRPreloadConfig(TIM2,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
}

//----------------------------------------------------------------------------------------------------------------------------
// Disable modulation
//----------------------------------------------------------------------------------------------------------------------------
void vModulationDisable()
{
	GPIOA->BRR = GPIO_Pin_7;
	TIM_ARRPreloadConfig(TIM2,DISABLE);
	TIM_Cmd(TIM2,DISABLE);
}
