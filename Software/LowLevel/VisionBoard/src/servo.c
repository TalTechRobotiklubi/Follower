#include "servo.h"

#include "stm32f4xx.h"

#include "DataLayer.h"

#define CENTER_POINT	100

static void gpioConfig(void);
static void interruptConfig(void);
static void timerConfig(void);

static int8_t servoX = 0;

void servo_init()
{
	gpioConfig();
	interruptConfig();
	timerConfig();
}

void servo_task()
{
	DL_getData(DLParamCameraRequestXDegree, &servoX);
}


void gpioConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM9 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

	/* GPIOE clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	/* GPIOD Configuration: TIM9 CH1 (PE5) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* Connect TIM9 pin to AF2 */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);
}

void timerConfig()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t prescalerValue;
	uint32_t CCRx_val = CENTER_POINT;  // x ms

	/* Compute the prescaler value*/
	prescalerValue = (uint16_t) ((SystemCoreClock / 2) / 100000) - 1;  // 0.01 milli sec resolution

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 2000;  // 20 ms
	TIM_TimeBaseStructure.TIM_Prescaler = prescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCRx_val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM9, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM9, ENABLE);
	/* TIM9 enable counter */
	TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM9, ENABLE);
}

void interruptConfig()
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_Init(&NVIC_InitStruct);
}

void TIM1_BRK_TIM9_IRQHandler()
{
	if (TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
	{
		TIM9->CCR1 = CENTER_POINT + servoX;
		TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
	}
}
