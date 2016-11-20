#ifndef __USART_H
#define __USART_H

#include "stdint.h"
#include "stm32l1xx.h"

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
void USART_init();
void USART_task(void);

//Interrupt handler
void USART2_IRQHandler(void);

#endif //__USART_H
