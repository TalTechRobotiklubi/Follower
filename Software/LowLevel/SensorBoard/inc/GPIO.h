#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f10x.h"

typedef enum
{
	INPUT_OFF = 0,
	INPUT_ON = 1
}
GPIO_InputDef;

typedef enum
{
	BUZZER = 0,
	LED_RED = 1,
	BUTTON_TOP_LEFT = 2,
	BUTTON_TOP_RIGHT = 3,
	BUTTON_BOTTOM_LEFT = 4,
	BUTTON_BOTTOM_RIGHT = 5,
	URF1_TRIG,
	URF1_ECHO,
	URF2_TRIG,
	URF2_ECHO,
	URF3_TRIG,
	URF3_ECHO,
	URF4_TRIG,
	URF4_ECHO,
	NUM_OF_GPIOS
}
GPIO_IdDef;

typedef struct
{
	GPIO_IdDef		id;
	GPIO_TypeDef* 	port;
	uint16_t	 	pin;
	uint16_t		clk;
	uint32_t		exti;
	uint8_t			portSource;
	uint8_t			pinSource;
	IRQn_Type		interrupt;
} GPIO_Struct;

//#define GPIOn				2
/*
#define BUZZER_PIN 		    GPIO_Pin_0
#define BUZZER_GPIO_PORT	GPIOB
#define BUZZER_GPIO_CLK	    RCC_APB2Periph_GPIOB

#define LED_RED_PIN 		GPIO_Pin_15
#define LED_RED_GPIO_PORT	GPIOB
#define LED_RED_GPIO_CLK	RCC_APB2Periph_GPIOB

#define BUTTON_TOP_LEFT_PIN        		GPIO_Pin_14  //PC14, SW1
#define BUTTON_TOP_LEFT_GPIO_PORT  		GPIOC
#define BUTTON_TOP_LEFT_GPIO_CLK   		RCC_APB2Periph_GPIOC

#define BUTTON_TOP_RIGHT_PIN        	GPIO_Pin_12  //PB12, SW2
#define BUTTON_TOP_RIGHT_GPIO_PORT  	GPIOB
#define BUTTON_TOP_RIGHT_GPIO_CLK  	   	RCC_APB2Periph_GPIOB

#define BUTTON_BOTTOM_LEFT_PIN         	GPIO_Pin_13  //PC13, SW4
#define BUTTON_BOTTOM_LEFT_GPIO_PORT   	GPIOC
#define BUTTON_BOTTOM_LEFT_GPIO_CLK    	RCC_APB2Periph_GPIOA

#define BUTTON_BOTTOM_RIGHT_PIN        	GPIO_Pin_15  //PA15, SW3
#define BUTTON_BOTTOM_RIGHT_GPIO_PORT  	GPIOA
#define BUTTON_BOTTOM_RIGHT_GPIO_CLK   	RCC_APB2Periph_GPIOC
*/
extern void   GPIO_init(void);
extern void   GPIO_outputOn(GPIO_IdDef io);
extern void   GPIO_outputOff(GPIO_IdDef io);
GPIO_InputDef GPIO_inputValue(GPIO_IdDef io);
extern void   GPIO_TASK_blinkLED(void);
extern void   GPIO_TASK_readButtons(void);

extern const GPIO_Struct GPIO_table[];

#endif /*GPIO_H_*/
