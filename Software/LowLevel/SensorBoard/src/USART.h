#ifndef __USART_H
#define __USART_H

#include "stdint.h"
#include "stm32f4xx.h"

//USART buffer definitions
#define USART_RX_BUFFER_SIZE 128
#define USART_TX_BUFFER_SIZE 128
#define USART_RX_BUFFER_MASK (USART_RX_BUFFER_SIZE - 1)
#define USART_TX_BUFFER_MASK (USART_TX_BUFFER_SIZE - 1)

#if(USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK)
	#error RX buffer size is not a power of 2
#endif
#if(USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK)
	#error TX buffer size is not a power of 2
#endif


//Initialization functions
void USART_initUSART2();
void USART_init(USART_TypeDef *uartX);
void USART_TASK(void);

//Interrupt handler
void USART2_IRQHandler(void);

//Functions for receiving and transmitting data
uint8_t USART_DataInRxBuffer(void);
void 	USART_SendChar(USART_TypeDef *uartX, unsigned char data_char);
void 	USART_SendString(USART_TypeDef *uartX, unsigned char *data_string);
void 	USART_ReceiveInt(void);


#endif //__USART_H
