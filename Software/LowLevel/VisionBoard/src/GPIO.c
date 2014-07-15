#include "GPIO.h"
#include "stm32f4xx.h"

/*Four user LEDs: green (PD12), orange (PD13), red (PD14) and blue (PD15)
*/
const GPIO_Struct GPIO_table[] = {
	/*id    				port    	pin   			clk						exti		port source				pin source			interrupt*/
/*GPIO*/
	{LED_GREEN,				GPIOD,	GPIO_Pin_12, 	RCC_AHB1Periph_GPIOD,	EXTI_Line12,	EXTI_PortSourceGPIOD,	EXTI_PinSource12,	EXTI15_10_IRQn},
	{LED_ORANGE,			GPIOD,	GPIO_Pin_13, 	RCC_AHB1Periph_GPIOD,	EXTI_Line13,	EXTI_PortSourceGPIOD,	EXTI_PinSource13,	EXTI15_10_IRQn},
	{LED_RED,				GPIOD,	GPIO_Pin_14,	RCC_AHB1Periph_GPIOD,	EXTI_Line14,	EXTI_PortSourceGPIOD,	EXTI_PinSource14,	EXTI15_10_IRQn},
	{LED_BLUE,				GPIOD,	GPIO_Pin_15,	RCC_AHB1Periph_GPIOD,	EXTI_Line15, 	EXTI_PortSourceGPIOD,	EXTI_PinSource15,	EXTI15_10_IRQn},
	{USER_BUTTON,			GPIOA,  GPIO_Pin_0, 	RCC_AHB1Periph_GPIOA,	EXTI_Line0, 	EXTI_PortSourceGPIOA,	EXTI_PinSource0,	EXTI0_IRQn},
/*I2C*/


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

	//Turn LEDs off
	GPIO_outputOff(LED_GREEN);
	GPIO_outputOff(LED_ORANGE);
	GPIO_outputOff(LED_RED);
	GPIO_outputOff(LED_BLUE);
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

	// blink LED
	if (state == 0)
	{
		GPIO_outputOn(LED_RED);
		/*GPIO_outputOn(LED_BLUE);
		GPIO_outputOn(LED_GREEN);
		GPIO_outputOn(LED_ORANGE);*/
		state = 1;
	}
	else
	{
		GPIO_outputOff(LED_RED);
		/*GPIO_outputOff(LED_BLUE);
		GPIO_outputOff(LED_GREEN);
		GPIO_outputOff(LED_ORANGE);*/
		state = 0;
	}
}

