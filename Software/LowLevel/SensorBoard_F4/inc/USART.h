#ifndef __USART_H
#define __USART_H

#include "stdint.h"

//USART hardware definitions
#define UART4_NUM             	4
/*#define USART4_GPIO              GPIOC
#define USART4_CLK               RCC_APB1Periph_USART4
#define USART4_GPIO_CLK          RCC_APB1Periph_GPIOC
#define USART4_RxPin             GPIO_Pin_11
#define USART4_TxPin             GPIO_Pin_10
#define USART4_IRQn              USART4_IRQn
#define USART4_IRQHandler        USART4_IRQHandler*/

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

extern int16_t value;
//Initialization functions
void USART_UART4_init(void);
void USART_TASK_sendDistances(void);
//Interrupt handler
void UART4_IRQHandler(void);

//Functions for receiving and transmitting data
uint8_t USART_DataInRxBuffer(void);
void 	USART_SendChar(unsigned char data_char);
void 	USART_SendString(unsigned char *data_string);
void    USART_SendInt(int data_int);
void 	USART_ReceiveInt(void);


#endif //__USART_H
