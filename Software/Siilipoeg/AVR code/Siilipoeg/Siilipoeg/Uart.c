/*
 * Uart.c
 *
 * Created: 24.08.2014 21:22:00
 *  Author: ljgtu
 */

#include <avr/io.h>
#include "Uart.h"

void init_Uart(void)
{
	UBRR1H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR1L = (uint8_t)(BAUD_PRESCALLER);

	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	UCSR1C = ((1<<UCSZ10)|(1<<UCSZ11));
}

unsigned char USART_receive(void)
{
	while(!(UCSR1A & (1<<RXC1)));
	return UDR1;
}

void USART_send( unsigned char data)
{
	while(!(UCSR1A & (1<<UDRE1)));
	UDR1 = data;
}
