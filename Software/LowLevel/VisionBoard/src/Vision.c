#include "Vision.h"
#include "stm32f4xx.h"

#define PIXY_I2C                      	I2C3
#define PIXY_I2C_CLK                  	RCC_APB1Periph_I2C3
#define PIXY_I2C_SCL_GPIO_CLOCK       	RCC_AHB1Periph_GPIOA
#define PIXY_I2C_SDA_GPIO_CLOCK       	RCC_AHB1Periph_GPIOC
#define PIXY_I2C_GPIO_AF              	GPIO_AF_I2C3
#define PIXY_I2C_SCL_PORT			  	GPIOA
#define PIXY_I2C_SDA_PORT			  	GPIOC
#define PIXY_I2C_SCL_PIN              	GPIO_Pin_8
#define PIXY_I2C_SDA_PIN              	GPIO_Pin_9
#define PIXY_I2C_SCL_GPIO_PinSource     GPIO_PinSource8
#define PIXY_I2C_SDA_GPIO_PinSource     GPIO_PinSource9
#define I2C_SPEED                       100000

#define SLAVE_ADDRESS 					((uint8_t)0x54)  // value shouldn't exceed 0x7F
#define VISION_BUFFER_SIZE				50
#define MAX_NUM_OF_OBJECTS				10

typedef enum
{
	Sync,
	CheckSum,
	Signature,
	Xcord,
	Ycord,
	Height,
	Width
}
BufferAnalyzeState;

void driverInit();
void driverIoInit();
void driverAfInit();
void driverStart(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void driverWrite(I2C_TypeDef* I2Cx, uint8_t data);
uint8_t driverReadAck(I2C_TypeDef* I2Cx);
uint8_t driverReadNack(I2C_TypeDef* I2Cx);
void driverStop(I2C_TypeDef* I2Cx);
void findObjectsFromBuffer();

uint8_t priv_receivedData[VISION_BUFFER_SIZE];
VisionObject priv_objects[MAX_NUM_OF_OBJECTS];
uint8_t priv_numOfObjects = 0;

void Vision_init()
{
	driverInit();
}


// I2C driver init
void driverInit()
{
	driverIoInit();
	driverAfInit();
}


VisionObject* Vision_getObjects()
{
	return priv_objects;
}


uint8_t Vision_numOfFoundObjects()
{
	return priv_numOfObjects;
}


void nullifyObjects()
{
	int i = 0;
	for ( ; i < MAX_NUM_OF_OBJECTS; i++)
	{
		priv_objects[i].x = 0;
		priv_objects[i].y = 0;
		priv_objects[i].height = 0;
		priv_objects[i].width = 0;
	}
}


void driverIoInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable I2C GPIO clocks */

	RCC_AHB1PeriphClockCmd(PIXY_I2C_SCL_GPIO_CLOCK, ENABLE);
	RCC_AHB1PeriphClockCmd(PIXY_I2C_SDA_GPIO_CLOCK, ENABLE);

	/* PIXY_I2C SCL and SDA pins configuration -------------------------------------*/
	GPIO_InitStructure.GPIO_Pin = PIXY_I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(PIXY_I2C_SCL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIXY_I2C_SDA_PIN;
	GPIO_Init(PIXY_I2C_SDA_PORT, &GPIO_InitStructure);

	/* Connect pins to I2C peripheral */
	GPIO_PinAFConfig(PIXY_I2C_SCL_PORT, PIXY_I2C_SCL_GPIO_PinSource, PIXY_I2C_GPIO_AF);
	GPIO_PinAFConfig(PIXY_I2C_SDA_PORT, PIXY_I2C_SDA_GPIO_PinSource, PIXY_I2C_GPIO_AF);
}


void driverAfInit()
{
	I2C_InitTypeDef I2C_InitStructure;

	/* Enable the PIXY_I2C peripheral clock */
	RCC_APB1PeriphClockCmd(PIXY_I2C_CLK, ENABLE);

	/* PIXY_I2C peripheral configuration */
	I2C_DeInit(PIXY_I2C);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;   // not relavant in master mode
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
	I2C_Init(PIXY_I2C, &I2C_InitStructure);

	/* Enable the I2C peripheral */
	I2C_Cmd(PIXY_I2C, ENABLE);
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


// note that data buffer contains word with sent least-significant byte first
void findObjectsFromBuffer()
{
	BufferAnalyzeState state = Sync;
	uint16_t checksum = 0;
	uint16_t calcChecksum = 0;
	uint8_t objCount = 0;

	int i = 0;
	for ( ; i < VISION_BUFFER_SIZE; i = i + 2)
	{
		switch (state)
		{
		case Sync:
			if (priv_receivedData[i] == 0x55 && priv_receivedData[i + 1] == 0xaa)
				state = CheckSum;
			break;
		case CheckSum:
			if (priv_receivedData[i] != 0x55 && priv_receivedData[i + 1] != 0xaa)
			{
				checksum = priv_receivedData[i] + (priv_receivedData[i + 1] << 8);
				state = Signature;
			}
			break;
		case Signature:
			if (priv_receivedData[i] == 1 && priv_receivedData[i + 1] == 0)
			{
				calcChecksum = priv_receivedData[i] + (priv_receivedData[i + 1] << 8);
				state = Xcord;
			}
			else
				state = Sync;
			break;
		case Xcord:
			calcChecksum += priv_receivedData[i] + (priv_receivedData[i + 1] << 8);
			priv_objects[objCount].x = priv_receivedData[i] + (priv_receivedData[i + 1] << 8);
			state = Ycord;
			break;
		case Ycord:
			calcChecksum += priv_receivedData[i] + (priv_receivedData[i + 1] << 8);
			priv_objects[objCount].y = priv_receivedData[i] + (priv_receivedData[i + 1] << 8);
			state = Height;
			break;
		case Height:
			calcChecksum += priv_receivedData[i] + (priv_receivedData[i + 1] << 8);
			priv_objects[objCount].height = priv_receivedData[i] + (priv_receivedData[i + 1] << 8);
			state = Width;
			break;
		case Width:
			calcChecksum += priv_receivedData[i] + (priv_receivedData[i + 1] << 8);
			priv_objects[objCount].width = priv_receivedData[i] + (priv_receivedData[i + 1] << 8);

			if (checksum == calcChecksum)
				objCount++;
			state = Sync;
			break;
		}
	}
	priv_numOfObjects = objCount;
}


void Vision_TASK()
{
	nullifyObjects();
    driverStart(PIXY_I2C, SLAVE_ADDRESS << 1, I2C_Direction_Receiver); // start a transmission in Master receiver mode
    int i = 0;
    for (; i < VISION_BUFFER_SIZE - 1; i++)
    {
    	priv_receivedData[i] = driverReadAck(PIXY_I2C); // read one byte and request another byte
    }
    priv_receivedData[i] = driverReadNack(PIXY_I2C); // read one byte and don't request another byte, stop transmission
    findObjectsFromBuffer();
}
