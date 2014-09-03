#include <avr/io.h>

void InitI2C(void)
{
	//set SCL to 100kHz
	TWSR = 0x00; // set prescaler to 1
	TWBR = 0x48; // SCL Freq = F_CPU/16 + 2 * TWBR * prescaler

	//enable TWI
	TWCR = (1<<TWEN);
}

// send start signal

void I2CStart(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
	while ((TWCR & (1<<TWINT)) == 0);
}
//send stop signal

void I2CStop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}


void I2CWrite(unsigned char  data)
{
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);

	// wait for byte to be sent
	while ((TWCR & (1<<TWINT)) == 0);
}

unsigned char  TWIReadACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}
//read byte with NACK
unsigned char  TWIReadNACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

unsigned char I2CRead(unsigned char ack)
{

	if (ack)
	{
		return TWIReadACK();
	}
	else
	{
		return TWIReadNACK();
	}

	return 0;
}


