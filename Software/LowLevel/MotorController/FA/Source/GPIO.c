#include "GPIO.h"
#include "stm32f10x.h"

/*Four user LEDs, LD3 (orange), LD4
(green), LD5 (red) and LD6 (blue
*/
const GPIO_Struct GPIO_table[] = {
	/*id    				port    	pin   			clk						exti		port source				pin source			interrupt*/
/*GPIO*/
	{LED_RED,				GPIOA,	GPIO_Pin_15, 	RCC_APB2Periph_GPIOA,	EXTI_Line15,	GPIO_PortSourceGPIOA,	GPIO_PinSource15,	EXTI15_10_IRQn},
	{LED_GREEN,				GPIOB,	GPIO_Pin_3,		RCC_APB2Periph_GPIOB,	EXTI_Line3,		GPIO_PortSourceGPIOB,	GPIO_PinSource3,	EXTI3_IRQn},
/*UART*/
	{UART_Tx,				GPIOA,	GPIO_Pin_9,		RCC_APB2Periph_GPIOA,	EXTI_Line9, 	GPIO_PortSourceGPIOA,	GPIO_PinSource9,	USART1_IRQn},  //PA9
	//{UART_Rx,				GPIOA,	GPIO_Pin_10,	RCC_APB2Periph_GPIOA,	EXTI_Line10, 	GPIO_PortSourceGPIOA,	GPIO_PinSource10,	USART1_IRQn},  //PA10
/*CAN1*/
	{CAN_Tx,				GPIOA,	GPIO_Pin_12,	RCC_APB2Periph_GPIOA,	EXTI_Line12, 	GPIO_PortSourceGPIOA,	GPIO_PinSource12,	CAN1_RX1_IRQn},  //PA12
	{CAN_Rx,				GPIOA,	GPIO_Pin_11,	RCC_APB2Periph_GPIOA,	EXTI_Line11, 	GPIO_PortSourceGPIOA,	GPIO_PinSource11,	CAN1_RX1_IRQn},  //PA11
};


/*private function declarations*/
void initOutput(GPIO_IdDef io);
void initInput(GPIO_IdDef io);

/*function definitions*/
void GPIO_init(void)
{
	initOutput(LED_GREEN);
	//initOutput(LED_RED);
	//initOutput(UART_Tx);
}

void initOutput(GPIO_IdDef io)
{
	//Structure for GPIO initialization
	GPIO_InitTypeDef GPIO_InitStructure;

	//Enable clock on pin
	RCC_APB2PeriphClockCmd(GPIO_table[io].clk, ENABLE);

	/* Configure pin in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin 	= GPIO_table[io].pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin  = GPIO_table[io].pin;

	//GPIO speed
	//GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_2MHz;

	//Initialize pin
	GPIO_Init(GPIO_table[io].port, &GPIO_InitStructure);
}

void GPIO_outputOn(GPIO_IdDef io)
{
	//set output high
	GPIO_table[io].port->BSRR = GPIO_table[io].pin;
}

void GPIO_outputOff(GPIO_IdDef io)
{
	//set output low
	GPIO_table[io].port->BRR = GPIO_table[io].pin;
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
		GPIO_outputOn(LED_GREEN);
		state = 1;
	}
	else
	{
		GPIO_outputOff(LED_GREEN);
		state = 0;
	}
}

