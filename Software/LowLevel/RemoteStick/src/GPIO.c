#include "GPIO.h"

const GPIO_Struct GPIO_table[] = {
	/*id    			port    	pin   			clk						exti		port source			pin source			interrupt*/
/*Buttons*/
	{BTN_LEFT,			GPIOB,	GPIO_Pin_2, 	RCC_AHBPeriph_GPIOB,	EXTI_Line2,		EXTI_PortSourceGPIOB,	EXTI_PinSource2,	EXTI2_IRQn},
	{BTN_RIGHT,			GPIOC,	GPIO_Pin_5, 	RCC_AHBPeriph_GPIOC,	EXTI_Line5,		EXTI_PortSourceGPIOC,	EXTI_PinSource5,	EXTI9_5_IRQn},
	{BTN_START,			GPIOC,	GPIO_Pin_4,		RCC_AHBPeriph_GPIOC,	EXTI_Line4,		EXTI_PortSourceGPIOC,	EXTI_PinSource4,	EXTI4_IRQn},
	{BTN_STOP,			GPIOB,	GPIO_Pin_10,	RCC_AHBPeriph_GPIOB,	EXTI_Line10, 	EXTI_PortSourceGPIOB,	EXTI_PinSource10,	EXTI15_10_IRQn},
/* LED */
	{LED,				GPIOC,  GPIO_Pin_8,		RCC_AHBPeriph_GPIOC,	EXTI_Line8,		EXTI_PortSourceGPIOC,   EXTI_PinSource8, 	EXTI9_5_IRQn},
/* Power */
//#define PWR_CTRL          PB11
//#define CHG_STAT          PC9
//#define UBAT_PIN          PB1
/* USART2*/
	{USART2_TX, 		GPIOA,  GPIO_Pin_2,     RCC_AHBPeriph_GPIOA,    EXTI_Line2,     EXTI_PortSourceGPIOA,   EXTI_PinSource2,	USART2_IRQn},
	{USART2_RX, 		GPIOA,  GPIO_Pin_3,     RCC_AHBPeriph_GPIOA,    EXTI_Line3,     EXTI_PortSourceGPIOA,   EXTI_PinSource3,	USART2_IRQn}
};

/*private function declarations*/
void initOutput(GPIO_IdDef io);
void initInput(GPIO_IdDef io,GPIOPuPd_TypeDef puPd);

/*function definitions*/
void GPIO_init(void)
{
	initOutput(LED);
}

void initOutput(GPIO_IdDef io)
{
	//Structure for GPIO initialization
	GPIO_InitTypeDef GPIO_InitStructure;

	//Enable clock on pin
	RCC_AHBPeriphClockCmd(GPIO_table[io].clk, ENABLE);

	/* Configure pin in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_table[io].pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	//Initialize pin
	GPIO_Init(GPIO_table[io].port, &GPIO_InitStructure);
}

void initInput(GPIO_IdDef io,GPIOPuPd_TypeDef puPd)
{
	//Structure for GPIO initialization
	GPIO_InitTypeDef GPIO_InitStructure;

	//Enable clock on pin
	RCC_APB2PeriphClockCmd(GPIO_table[io].clk, ENABLE);

	/* Configure pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = puPd;//GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin  = GPIO_table[io].pin;

	//GPIO speed
	//GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_2MHz;

	//Initialize pin
	GPIO_Init(GPIO_table[io].port, &GPIO_InitStructure);
}

void GPIO_outputOn(GPIO_IdDef io)
{
	//set output high
	GPIO_table[io].port->BSRRL = GPIO_table[io].pin;
}

void GPIO_outputOff(GPIO_IdDef io)
{
	//set output low
	GPIO_table[io].port->BSRRH = GPIO_table[io].pin;
}

GPIO_InputDef GPIO_inputValue(GPIO_IdDef io)
{
	if((GPIO_table[io].port->IDR & GPIO_table[io].pin) != (uint32_t)Bit_RESET)
	{
		return INPUT_ON;
	}
	else
	{
		return INPUT_OFF;
	}
}

void GPIO_TASK_blinkLED(void)
{
	static uint8_t state = 0;

	// blink LED
	if (state == 0)
	{
		GPIO_outputOn(LED);
		state = 1;
	}
	else
	{
		GPIO_outputOff(LED);
		state = 0;
	}
}

