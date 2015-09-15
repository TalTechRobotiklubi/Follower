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
/*Sensors*/
	{URF1_ECHO,				GPIOC,	GPIO_Pin_14,	RCC_AHB1Periph_GPIOC,	EXTI_Line14, 	EXTI_PortSourceGPIOC,	EXTI_PinSource14,	EXTI15_10_IRQn},  //PC14
	{URF1_TRIG,				GPIOC,	GPIO_Pin_15,	RCC_AHB1Periph_GPIOC,	EXTI_Line15, 	EXTI_PortSourceGPIOC,	EXTI_PinSource15,	EXTI15_10_IRQn},  //PC15
	{URF2_ECHO,				GPIOE,	GPIO_Pin_6,		RCC_AHB1Periph_GPIOE,	EXTI_Line6, 	EXTI_PortSourceGPIOE,	EXTI_PinSource6,	EXTI9_5_IRQn},    //PE6
	{URF2_TRIG,				GPIOC,	GPIO_Pin_13,	RCC_AHB1Periph_GPIOC,	EXTI_Line13, 	EXTI_PortSourceGPIOC,	EXTI_PinSource13,	EXTI15_10_IRQn},  //PC13
	{URF3_ECHO,				GPIOE,	GPIO_Pin_4,		RCC_AHB1Periph_GPIOE,	EXTI_Line4, 	EXTI_PortSourceGPIOE,	EXTI_PinSource4,	EXTI4_IRQn},  	//PE4
	{URF3_TRIG,				GPIOE,	GPIO_Pin_5,		RCC_AHB1Periph_GPIOE,	EXTI_Line5, 	EXTI_PortSourceGPIOE,	EXTI_PinSource5,	EXTI9_5_IRQn},  //PE5
	{URF4_ECHO,				GPIOE,	GPIO_Pin_2,		RCC_AHB1Periph_GPIOE,	EXTI_Line2, 	EXTI_PortSourceGPIOE,	EXTI_PinSource2,	EXTI2_IRQn},    //PE2
	{URF4_TRIG,				GPIOB,	GPIO_Pin_8,		RCC_AHB1Periph_GPIOB,	EXTI_Line8, 	EXTI_PortSourceGPIOB,	EXTI_PinSource8,	EXTI9_5_IRQn},  //PB8
	{URF5_ECHO,				GPIOC,	GPIO_Pin_5,		RCC_AHB1Periph_GPIOC,	EXTI_Line5, 	EXTI_PortSourceGPIOC,	EXTI_PinSource5,	EXTI9_5_IRQn},  //PC5
	{URF5_TRIG,				GPIOC,	GPIO_Pin_4,		RCC_AHB1Periph_GPIOC,	EXTI_Line4, 	EXTI_PortSourceGPIOC,	EXTI_PinSource4,	EXTI4_IRQn},  //PC4
	{URF6_ECHO,				GPIOB,	GPIO_Pin_1,		RCC_AHB1Periph_GPIOB,	EXTI_Line1, 	EXTI_PortSourceGPIOB,	EXTI_PinSource1,	EXTI1_IRQn},    //PB1
	{URF6_TRIG,				GPIOB,	GPIO_Pin_0,		RCC_AHB1Periph_GPIOB,	EXTI_Line0, 	EXTI_PortSourceGPIOB,	EXTI_PinSource0,	EXTI0_IRQn},  //PB0
	{URF7_ECHO,				GPIOE,	GPIO_Pin_7,		RCC_AHB1Periph_GPIOE,	EXTI_Line7, 	EXTI_PortSourceGPIOE,	EXTI_PinSource7,	EXTI9_5_IRQn},  //PE7
	{URF7_TRIG,				GPIOE,	GPIO_Pin_8,		RCC_AHB1Periph_GPIOE,	EXTI_Line8, 	EXTI_PortSourceGPIOE,	EXTI_PinSource8,	EXTI9_5_IRQn},  //PE8
	{URF8_ECHO,				GPIOE,	GPIO_Pin_9,		RCC_AHB1Periph_GPIOE,	EXTI_Line9, 	EXTI_PortSourceGPIOE,	EXTI_PinSource9,	EXTI9_5_IRQn},    //PE9
	{URF8_TRIG,				GPIOE,	GPIO_Pin_10,	RCC_AHB1Periph_GPIOE,	EXTI_Line10, 	EXTI_PortSourceGPIOE,	EXTI_PinSource10,	EXTI15_10_IRQn},  //PE10
/*UART4*/
	{UART4_Tx,				GPIOC,	GPIO_Pin_10,	RCC_AHB1Periph_GPIOC,	EXTI_Line10, 	EXTI_PortSourceGPIOC,	GPIO_PinSource10,	UART4_IRQn},  //PC10
	{UART4_Rx,				GPIOC,	GPIO_Pin_11,	RCC_AHB1Periph_GPIOC,	EXTI_Line11, 	EXTI_PortSourceGPIOC,	GPIO_PinSource11,	UART4_IRQn},  //PC11
/*CAN1*/
	{CAN1_Tx,				GPIOD,	GPIO_Pin_1,		RCC_AHB1Periph_GPIOD,	EXTI_Line1, 	EXTI_PortSourceGPIOD,	GPIO_PinSource1,	CAN1_TX_IRQn},  //PD1
	{CAN1_Rx,				GPIOD,	GPIO_Pin_0,		RCC_AHB1Periph_GPIOD,	EXTI_Line0, 	EXTI_PortSourceGPIOD,	GPIO_PinSource0,	CAN1_RX0_IRQn},  //PD0
/*USART2*/
	{USART2_Tx,				GPIOA,	GPIO_Pin_2,		RCC_AHB1Periph_GPIOA,	EXTI_Line2, 	EXTI_PortSourceGPIOA,	GPIO_PinSource2,	USART2_IRQn},  //PC10
	{USART2_Rx,				GPIOA,	GPIO_Pin_3,		RCC_AHB1Periph_GPIOA,	EXTI_Line3, 	EXTI_PortSourceGPIOA,	GPIO_PinSource3,	USART2_IRQn},  //PC11
/*I2C (Power)*/
	{I2C3_SCL,				GPIOA,  GPIO_Pin_8, 	RCC_AHB1Periph_GPIOA,   EXTI_Line8,     EXTI_PortSourceGPIOA,	EXTI_PinSource8,	I2C3_EV_IRQn},
	{I2C3_SDA,				GPIOC,  GPIO_Pin_9, 	RCC_AHB1Periph_GPIOC,   EXTI_Line9,     EXTI_PortSourceGPIOC,	EXTI_PinSource9,	I2C3_EV_IRQn},
	/*Accelerator - GPIO init done in acceleration module, look also stm32f4_discovery_lis302dl.h file*/
	// LIS302DL_SPI_SCK_PIN  - PA5
	// LIS302DL_SPI_MISO_PIN - PA6
	// LIS302DL_SPI_MOSI_PIN - PA7
	// LIS302DL_SPI_CS_PIN   - PE3
	// LIS302DL_SPI_INT1_PIN - PE0
	// LIS302DL_SPI_INT2_PIN - PE1

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
	initOutput(URF1_TRIG);
	initOutput(URF2_TRIG);
	initOutput(URF3_TRIG);
	initOutput(URF4_TRIG);
	initOutput(URF5_TRIG);
	initOutput(URF6_TRIG);
	initOutput(URF7_TRIG);
	initOutput(URF8_TRIG);
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

