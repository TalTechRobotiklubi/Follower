#include "GPIO.h"
#include "stm32f4xx.h"

/*Four user LEDs, LD3 (orange), LD4
(green), LD5 (red) and LD6 (blue
*/
const GPIO_Struct GPIO_table[] = {
	/*id    				port    	pin   			clk						exti		port source				pin source			interrupt*/
/*GPIO*/
	{LED_GREEN,				GPIOD,	GPIO_Pin_12, 	RCC_AHB1Periph_GPIOD,	EXTI_Line12,	EXTI_PortSourceGPIOD,	EXTI_PinSource12,	EXTI15_10_IRQn},
	{LED_ORANGE,			GPIOD,	GPIO_Pin_13, 	RCC_AHB1Periph_GPIOD,	EXTI_Line13,	EXTI_PortSourceGPIOD,	EXTI_PinSource13,	EXTI15_10_IRQn},
	{LED_RED,				GPIOD,	GPIO_Pin_14,	RCC_AHB1Periph_GPIOD,	EXTI_Line14,	EXTI_PortSourceGPIOD,	EXTI_PinSource14,	EXTI15_10_IRQn},
	{LED_BLUE,				GPIOD,	GPIO_Pin_15,	RCC_AHB1Periph_GPIOD,	EXTI_Line15, 	EXTI_PortSourceGPIOD,	EXTI_PinSource15,	EXTI15_10_IRQn},
/*Sensors*/
	{URF1_ECHO,				GPIOE,	GPIO_Pin_0,		RCC_AHB1Periph_GPIOE,	EXTI_Line0, 	EXTI_PortSourceGPIOE,	EXTI_PinSource0,	EXTI0_IRQn},  	//PE0
	{URF1_TRIG,				GPIOE,	GPIO_Pin_1,		RCC_AHB1Periph_GPIOE,	EXTI_Line1, 	EXTI_PortSourceGPIOE,	EXTI_PinSource1,	EXTI1_IRQn},    //PE1
	{URF2_ECHO,				GPIOE,	GPIO_Pin_2,		RCC_AHB1Periph_GPIOE,	EXTI_Line2, 	EXTI_PortSourceGPIOE,	EXTI_PinSource2,	EXTI2_IRQn},  	//PE2
	{URF2_TRIG,				GPIOE,	GPIO_Pin_3,		RCC_AHB1Periph_GPIOE,	EXTI_Line3, 	EXTI_PortSourceGPIOE,	EXTI_PinSource3,	EXTI3_IRQn},    //PE3
	{URF3_ECHO,				GPIOE,	GPIO_Pin_4,		RCC_AHB1Periph_GPIOE,	EXTI_Line4, 	EXTI_PortSourceGPIOE,	EXTI_PinSource4,	EXTI4_IRQn},  	//PE4
	{URF3_TRIG,				GPIOE,	GPIO_Pin_5,		RCC_AHB1Periph_GPIOE,	EXTI_Line5, 	EXTI_PortSourceGPIOE,	EXTI_PinSource5,	EXTI9_5_IRQn},  //PE5
	{URF4_ECHO,				GPIOB,	GPIO_Pin_8,		RCC_AHB1Periph_GPIOB,	EXTI_Line8, 	EXTI_PortSourceGPIOB,	EXTI_PinSource8,	EXTI9_5_IRQn},  //PB8
	{URF4_TRIG,				GPIOB,	GPIO_Pin_9,		RCC_AHB1Periph_GPIOB,	EXTI_Line9, 	EXTI_PortSourceGPIOB,	EXTI_PinSource9,	EXTI9_5_IRQn},  //PB9
/*USART*/
	{UART4_Tx,				GPIOC,	GPIO_Pin_10,	RCC_AHB1Periph_GPIOC,	EXTI_Line10, 	EXTI_PortSourceGPIOC,	GPIO_PinSource10,	EXTI15_10_IRQn},  //PC10
	{UART4_Rx,				GPIOC,	GPIO_Pin_11,	RCC_AHB1Periph_GPIOC,	EXTI_Line11, 	EXTI_PortSourceGPIOC,	GPIO_PinSource11,	EXTI15_10_IRQn},  //PC11
};


/*private function declarations*/
void initOutput(GPIO_IdDef io);
void initInput(GPIO_IdDef io);

/*function definitions*/
void GPIO_init(void)
{
	initOutput(LED_ORANGE);
	initOutput(LED_GREEN);
	initOutput(LED_RED);
	initOutput(LED_BLUE);
}

void initOutput(GPIO_IdDef io)
{
	//Structure for GPIO initialization
	GPIO_InitTypeDef GPIO_InitStructure;

	//Enable clock on pin
	RCC_AHB1PeriphClockCmd(GPIO_table[io].clk, ENABLE);

	/* Configure pin in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_table[io].pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	//Initialize pin
	GPIO_Init(GPIO_table[io].port, &GPIO_InitStructure);
}

void initInput(GPIO_IdDef io)
{
	//Structure for GPIO initialization
	GPIO_InitTypeDef GPIO_InitStructure;

	//Enable clock on pin
	RCC_APB2PeriphClockCmd(GPIO_table[io].clk, ENABLE);

	/* Configure pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
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
	//if ((GPIO_PORT[io]->IDR & GPIO_PIN[io]) != (uint32_t)Bit_RESET)
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
	static unsigned char temp = 0;

	// blink LED
	if (state == 0)
	{
		GPIO_outputOn(LED_RED);
		GPIO_outputOn(LED_BLUE);
		temp++;
		//USART_SendChar(temp);
		state = 1;
	}
	else
	{
		GPIO_outputOff(LED_RED);
		GPIO_outputOff(LED_BLUE);
		state = 0;
	}
}

