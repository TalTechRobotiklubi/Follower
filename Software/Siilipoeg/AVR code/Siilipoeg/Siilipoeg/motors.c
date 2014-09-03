#include <avr/io.h>


void init_motors(void)//algseadistab mootorid
{
	// motor A PB4 PB5
	// motor B PB6 PC6

	// set all motor pins to Outtput
	DDRB |= _BV(PB4)|_BV(PB5)|_BV(PB6); //(1<<PB6)
	DDRC |= _BV(PC6);

	// setting default outputs
	PORTB &= ~(_BV(PB4)|_BV(PB5)|_BV(PB6));
	PORTC &= ~(_BV(PC6));

	//init timer1
	//Set A on Compare Match (11)
	TCCR1A |= _BV(COM0A1)|_BV(COM0A0);
	//Set B compare match (11)
	TCCR1A |= _BV(COM0B1)|_BV(COM0B0);
	// count up to 0XFF (11)
	TCCR1A |= _BV(WGM11);
	// timer clock (010 clk/8);
	TCCR1B |= _BV(CS11) | _BV(WGM13) | _BV(WGM12);

	ICR1 = 1025;

	OCR1A = 1025;
	OCR1B = 1025;
}

void setmotor_speed(int left,int right)// määrab ära mootorite kiirused skaalas -1025..1025
{
	if (right < 0 )
	{
		PORTC &= ~(_BV(PC6));
		OCR1B = 1025 + right;
	}
	else
	{
		PORTC |= _BV(PC6);
		OCR1B = right;
	}

	if (left < 0 )
	{
		PORTB &= ~(_BV(PB4));
		OCR1A = 1025 + left;
	}
	else
	{
		PORTB |= _BV(PB4);
		OCR1A = left;
	}

}