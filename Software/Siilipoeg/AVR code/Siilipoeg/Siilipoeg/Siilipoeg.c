/*
 * Siilikeudus.c
 *
 */

#include <avr/io.h>
#include <util/delay.h>

#include "I2C.h"
#include "Vision.h"
#include "motors.h"

int main(void)
{
	int packets = 0;

	init_motors();
	InitI2C();

	PORTD &= ~(_BV(PD0)|_BV(PD0));

	DDRB |= 1;

	setmotor_speed(500,500);

	while(1)
	{
		PORTB |= 1;

		Vision_TASK();

		packets += Vision_numOfFoundObjects();

		if (packets & 1)
		{
			PORTB &= ~1;
		}

		_delay_ms(20);
	}
}

