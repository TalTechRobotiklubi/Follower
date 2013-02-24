#include "GPIO.h"
#include "stm32f10x.h"
#include "DataLayer.h"
//#include "USART.h"


const GPIO_Struct GPIO_table[] = {
	/*id    				port    	pin   			clk						exti		port source				pin source			interrupt*/
	{BUZZER, 				GPIOB, 	GPIO_Pin_0, 	RCC_APB2Periph_GPIOB,	EXTI_Line0,		GPIO_PortSourceGPIOB,	GPIO_PinSource0,	EXTI0_IRQn},
	{LED_RED,				GPIOB,	GPIO_Pin_15, 	RCC_APB2Periph_GPIOB,	EXTI_Line15,	GPIO_PortSourceGPIOB,	GPIO_PinSource15,	EXTI15_10_IRQn},
	{BUTTON_TOP_LEFT,		GPIOC,	GPIO_Pin_14, 	RCC_APB2Periph_GPIOC,	EXTI_Line14,	GPIO_PortSourceGPIOC,	GPIO_PinSource14,	EXTI15_10_IRQn},  //PC14, SW1
	{BUTTON_TOP_RIGHT,		GPIOB,	GPIO_Pin_12,	RCC_APB2Periph_GPIOB,	EXTI_Line12,	GPIO_PortSourceGPIOB,	GPIO_PinSource12,	EXTI15_10_IRQn},  //PB12, SW2
	{BUTTON_BOTTOM_LEFT,	GPIOC,	GPIO_Pin_13,	RCC_APB2Periph_GPIOA,	EXTI_Line13, 	GPIO_PortSourceGPIOC,	GPIO_PinSource13,	EXTI15_10_IRQn},  //PC13, SW4
	{BUTTON_BOTTOM_RIGHT,	GPIOA,	GPIO_Pin_15,	RCC_APB2Periph_GPIOC,	EXTI_Line15,	GPIO_PortSourceGPIOA,	GPIO_PinSource15,	EXTI15_10_IRQn},  //PA15, SW3
	{URF1_TRIG,				GPIOB,	GPIO_Pin_11,	RCC_APB2Periph_GPIOB,	EXTI_Line11, 	GPIO_PortSourceGPIOB,	GPIO_PinSource11,	EXTI15_10_IRQn},  //PB11, PWM2
	{URF1_ECHO,				GPIOB,	GPIO_Pin_10,	RCC_APB2Periph_GPIOB,	EXTI_Line10, 	GPIO_PortSourceGPIOB,	GPIO_PinSource10,	EXTI4_IRQn},  //PA4, AIN4
	{URF2_TRIG,				GPIOB,	GPIO_Pin_11,	RCC_APB2Periph_GPIOB,	EXTI_Line11, 	GPIO_PortSourceGPIOB,	GPIO_PinSource11,	EXTI15_10_IRQn},  //PB11, PWM2
	{URF2_ECHO,				GPIOB,	GPIO_Pin_10,	RCC_APB2Periph_GPIOB,	EXTI_Line10, 	GPIO_PortSourceGPIOB,	GPIO_PinSource10,	EXTI9_5_IRQn},  //PA5, AIN5
	{URF3_TRIG,				GPIOB,	GPIO_Pin_11,	RCC_APB2Periph_GPIOB,	EXTI_Line11, 	GPIO_PortSourceGPIOB,	GPIO_PinSource11,	EXTI15_10_IRQn},  //PB11, PWM2
	{URF3_ECHO,				GPIOB,	GPIO_Pin_10,	RCC_APB2Periph_GPIOB,	EXTI_Line10, 	GPIO_PortSourceGPIOB,	GPIO_PinSource10,	EXTI9_5_IRQn},  //PA6, AIN6
	{URF4_TRIG,				GPIOB,	GPIO_Pin_11,	RCC_APB2Periph_GPIOB,	EXTI_Line11, 	GPIO_PortSourceGPIOB,	GPIO_PinSource11,	EXTI15_10_IRQn},  //PB11, PWM2
	{URF4_ECHO,				GPIOB,	GPIO_Pin_10,	RCC_APB2Periph_GPIOB,	EXTI_Line10, 	GPIO_PortSourceGPIOB,	GPIO_PinSource10,	EXTI9_5_IRQn},  //PA7, AIN7
};


/*private function declarations*/
void initOutput(GPIO_IdDef io);
void initInput(GPIO_IdDef io);

/*function definitions*/
void GPIO_init(void)
{
	initOutput(BUZZER);
	initOutput(LED_RED);
	initOutput(URF1_TRIG);
	initOutput(URF2_TRIG);
	initOutput(URF3_TRIG);
	initOutput(URF4_TRIG);
	initInput(BUTTON_TOP_LEFT);
	initInput(BUTTON_TOP_RIGHT);
	initInput(BUTTON_BOTTOM_LEFT);
	initInput(BUTTON_BOTTOM_RIGHT);
}

void initOutput(GPIO_IdDef io)
{
	//Structure for GPIO initialization
	GPIO_InitTypeDef GPIO_InitStructure;

	//Enable clock on pin
	RCC_APB2PeriphClockCmd(GPIO_table[io].clk, ENABLE);

	//Configure pin
	GPIO_InitStructure.GPIO_Pin 	= GPIO_table[io].pin;
	//Configure pin as push-pull output
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;
	//GPIO speed 50MHz
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_10MHz;

	//Initialize pin
	GPIO_Init(GPIO_table[io].port, &GPIO_InitStructure);
}

void initInput(GPIO_IdDef io)
{
	//Structure for GPIO initialization
	GPIO_InitTypeDef GPIO_InitStructure;

	//Enable clock on pin
	RCC_APB2PeriphClockCmd(GPIO_table[io].clk, ENABLE);

	//Configure pin
	GPIO_InitStructure.GPIO_Pin 	= GPIO_table[io].pin;
	//Configure pin as floating input
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	//GPIO speed
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_2MHz;

	//Initialize pin
	GPIO_Init(GPIO_table[io].port, &GPIO_InitStructure);
}

void GPIO_outputOn(GPIO_IdDef io)
{
	//set output high
	GPIO_table[io].port->BSRR = GPIO_table[io].pin;
	//GPIO_PORT[io]->BSRR = GPIO_PIN[io];
}

void GPIO_outputOff(GPIO_IdDef io)
{
	//set output low
	GPIO_table[io].port->BRR = GPIO_table[io].pin;
	//GPIO_PORT[io]->BRR = GPIO_PIN[io];
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
		temp++;
		//USART_SendChar(temp);
		state = 1;
	}
	else
	{
		GPIO_outputOff(LED_RED);
		state = 0;
	}
}

void GPIO_TASK_readButtons(void)
{
	static uint8_t data = 0;
	static uint8_t buttonTLState = 0;
	static uint8_t buttonTRState = 0;
	static uint8_t buttonBLState = 0;
	static uint8_t buttonBRState = 0;

	/* use 8 bit storage (1 byte) to save last 8 button states. Every time last sample is shifted out
	 * and new one is set*/
	buttonTLState = ((buttonTLState << 1) & 0xFE) | (uint8_t)(GPIO_inputValue(BUTTON_TOP_LEFT));
	buttonTRState = ((buttonTRState << 1) & 0xFE) | (uint8_t)(GPIO_inputValue(BUTTON_TOP_RIGHT));
	buttonBLState = ((buttonBLState << 1) & 0xFE) | (uint8_t)(GPIO_inputValue(BUTTON_BOTTOM_LEFT));
	buttonBRState = ((buttonBRState << 1) & 0xFE) | (uint8_t)(GPIO_inputValue(BUTTON_BOTTOM_RIGHT));

	/*if at least 6 bits are 1 then button is pressed*/
	if ((buttonTLState == 0xFF) || (buttonTLState == 0x7F) || (buttonTLState == 0x3F))
	{
		data = 1;
	}
	else
	{
		data = 0;
	}
	DL_setData(DLParamButtonTopLeft, &data);

	if (buttonTRState == 0xFF) data = 1; else data = 0;
	DL_setData(DLParamButtonTopRight, &data);

	if (buttonBLState == 0xFF) data = 1; else data = 0;
	DL_setData(DLParamButtonBottomLeft, &data);

	if (buttonBRState == 0xFF) data = 1; else data = 0;
	DL_setData(DLParamButtonBottomRight, &data);
}
