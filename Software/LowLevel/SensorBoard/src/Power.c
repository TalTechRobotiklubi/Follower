#include "Power.h"
#include "stm32f4xx.h"
#include "DataLayer.h"

#define POWER_I2C                      	I2C3
#define POWER_I2C_CLK                  	RCC_APB1Periph_I2C3
#define POWER_I2C_SCL_GPIO_CLOCK       	RCC_AHB1Periph_GPIOA
#define POWER_I2C_SDA_GPIO_CLOCK       	RCC_AHB1Periph_GPIOC
#define POWER_I2C_GPIO_AF              	GPIO_AF_I2C3
#define POWER_I2C_SCL_PORT			  	GPIOA
#define POWER_I2C_SDA_PORT			  	GPIOC
#define POWER_I2C_SCL_PIN              	GPIO_Pin_8
#define POWER_I2C_SDA_PIN              	GPIO_Pin_9
#define POWER_I2C_SCL_GPIO_PinSource     GPIO_PinSource8
#define POWER_I2C_SDA_GPIO_PinSource     GPIO_PinSource9
#define I2C_SPEED                       100000

#define SLAVE_ADDRESS 					((uint8_t)0x54)  // value shouldn't exceed 0x7F
#define POWER_BUFFER_SIZE				50


void driverInit();
void driverIoInit();
void driverAfInit();
void driverStart(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void driverWrite(I2C_TypeDef* I2Cx, uint8_t data);
uint8_t driverReadAck(I2C_TypeDef* I2Cx);
uint8_t driverReadNack(I2C_TypeDef* I2Cx);
void driverStop(I2C_TypeDef* I2Cx);


uint8_t priv_receivedData[POWER_BUFFER_SIZE];


void Power_init()
{
	driverInit();
}


// I2C driver init
void driverInit()
{
	driverIoInit();
	driverAfInit();
}


void driverIoInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable I2C GPIO clocks */

	RCC_AHB1PeriphClockCmd(POWER_I2C_SCL_GPIO_CLOCK, ENABLE);
	RCC_AHB1PeriphClockCmd(POWER_I2C_SDA_GPIO_CLOCK, ENABLE);

	/* POWER_I2C SCL and SDA pins configuration -------------------------------------*/
	GPIO_InitStructure.GPIO_Pin = POWER_I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(POWER_I2C_SCL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = POWER_I2C_SDA_PIN;
	GPIO_Init(POWER_I2C_SDA_PORT, &GPIO_InitStructure);

	/* Connect pins to I2C peripheral */
	GPIO_PinAFConfig(POWER_I2C_SCL_PORT, POWER_I2C_SCL_GPIO_PinSource, POWER_I2C_GPIO_AF);
	GPIO_PinAFConfig(POWER_I2C_SDA_PORT, POWER_I2C_SDA_GPIO_PinSource, POWER_I2C_GPIO_AF);
}


void driverAfInit()
{
	I2C_InitTypeDef I2C_InitStructure;

	/* Enable the POWER_I2C peripheral clock */
	RCC_APB1PeriphClockCmd(POWER_I2C_CLK, ENABLE);

	/* POWER_I2C peripheral configuration */
	I2C_DeInit(POWER_I2C);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;   // not relavant in master mode
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
	I2C_Init(POWER_I2C, &I2C_InitStructure);

	/* Enable the I2C peripheral */
	I2C_Cmd(POWER_I2C, ENABLE);
}


/* This function issues a start condition and
* transmits the slave address + R/W bit
*
* Parameters:
* I2Cx --> the I2C peripheral e.g. I2C1
* address --> the 7 bit slave address
* direction --> the tranmission direction can be:
* I2C_Direction_Tranmitter for Master transmitter mode
* I2C_Direction_Receiver for Master receiver
*/
void driverStart(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
{
	// wait until I2Cx is not busy anymore
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

	// Send I2Cx START condition
	I2C_GenerateSTART(I2Cx, ENABLE);

	// wait for I2Cx EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

	// Send slave Address for write
	I2C_Send7bitAddress(I2Cx, address, direction);

	/* wait for I2Cx EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
	if(direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if(direction == I2C_Direction_Receiver){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}


/* This function transmits one byte to the slave device
 * Parameters:
 * I2Cx --> the I2C peripheral e.g. I2C1
 * data --> the data byte to be transmitted
 */
void driverWrite(I2C_TypeDef* I2Cx, uint8_t data)
{
	I2C_SendData(I2Cx, data);
	// wait for I2Cx EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}


/* This function reads one byte from the slave device
 * and acknowledges the byte (requests another byte)
 */
uint8_t driverReadAck(I2C_TypeDef* I2Cx)
{
	// enable acknowledge of received data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}

/* This function reads one byte from the slave device
 * and doesn't acknowledge the recieved data
 */
uint8_t driverReadNack(I2C_TypeDef* I2Cx)
{
	// disable acknowledge of received data
	// nack also generates stop condition after last byte received
	// see reference manual for more info
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	I2C_GenerateSTOP(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}

/* This funtion issues a stop condition and therefore
 * releases the bus
 */
void driverStop(I2C_TypeDef* I2Cx)
{
	// Send I2C1 STOP Condition
	I2C_GenerateSTOP(I2Cx, ENABLE);
}



void Vision_TASK()
{

    driverStart(POWER_I2C, SLAVE_ADDRESS << 1, I2C_Direction_Receiver); // start a transmission in Master receiver mode
    int i = 0;
    for (; i < POWER_BUFFER_SIZE - 1; i++)
    {
    	priv_receivedData[i] = driverReadAck(POWER_I2C); // read one byte and request another byte
    }
    priv_receivedData[i] = driverReadNack(POWER_I2C); // read one byte and don't request another byte, stop transmission

}
