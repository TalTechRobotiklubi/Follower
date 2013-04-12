#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f4xx.h"

typedef enum
{
	INPUT_OFF = 0,
	INPUT_ON = 1
}
GPIO_InputDef;

typedef enum
{
	LED_GREEN = 0,
	LED_ORANGE,
	LED_RED,
	LED_BLUE,
	URF1_ECHO,
	URF1_TRIG,
	URF2_ECHO,
	URF2_TRIG,
	URF3_ECHO,
	URF3_TRIG,
	URF4_ECHO,
	URF4_TRIG,
	UART4_Tx,
	UART4_Rx,
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

extern void   GPIO_init(void);
extern void   GPIO_outputOn(GPIO_IdDef io);
extern void   GPIO_outputOff(GPIO_IdDef io);
GPIO_InputDef GPIO_inputValue(GPIO_IdDef io);
extern void   GPIO_TASK_blinkLED(void);

extern const GPIO_Struct GPIO_table[];

#endif /*GPIO_H_*/
