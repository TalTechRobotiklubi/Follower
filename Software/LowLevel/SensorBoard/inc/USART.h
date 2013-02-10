#ifndef __USART_H
#define __USART_H

#include "stdint.h"

//USART hardware definitions
#define USART                   USART1
#define USART_GPIO              GPIOA
#define USART_CLK               RCC_APB2Periph_USART1
#define USART_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USART_RxPin             GPIO_Pin_10
#define USART_TxPin             GPIO_Pin_9
#define USART_IRQn              USART1_IRQn
#define USART_IRQHandler        USART1_IRQHandler

//USART buffer definitions
#define USART_RX_BUFFER_SIZE 200
#define USART_TX_BUFFER_SIZE 128
#define USART_RX_BUFFER_MASK (USART_RX_BUFFER_SIZE - 1)
#define USART_TX_BUFFER_MASK (USART_TX_BUFFER_SIZE - 1)

//#if(USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK)
//	#error RX buffer size is not a power of 2
//#endif
#if(USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK)
	#error TX buffer size is not a power of 2
#endif

extern int16_t value;
//Initialization functions
extern void USART_init(void);
extern void USART_TASK(void);

//Functions for receiving and transmitting data
extern uint8_t USART_DataInRxBuffer(void);
extern void    USART_SendChar(unsigned char data_char);
extern void    USART_SendString(unsigned char *data_string);
extern void    USART_SendInt(int data_int);
extern void    USART_ReceiveInt(void);

//Interrupt handler
void USART1_IRQHandler(void);


#endif //__USART_H
