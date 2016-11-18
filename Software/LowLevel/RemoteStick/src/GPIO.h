#ifndef GPIO_H_
#define GPIO_H_

#include "stm32l1xx.h"

typedef enum
{
	INPUT_OFF = 0,
	INPUT_ON = 1
}
GPIO_InputDef;

typedef enum
{
	BTN_LEFT,
	BTN_RIGHT,
	BTN_START,
	BTN_STOP,
	LED,
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
