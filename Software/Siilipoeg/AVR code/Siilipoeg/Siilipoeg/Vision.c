#include "Vision.h"
#include "I2C.h"

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


void findObjectsFromBuffer();

uint8_t priv_receivedData[VISION_BUFFER_SIZE];
VisionObject priv_objects[MAX_NUM_OF_OBJECTS];
uint8_t priv_numOfObjects = 0;




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
			{
				//	setTrackedObjectToDataLayer(&priv_objects[objCount]);
				objCount++;
			}
			state = Sync;
			break;
		}
	}
	priv_numOfObjects = objCount;
}

void Vision_TASK()
{
	nullifyObjects();

	I2CStart();
	I2CWrite((SLAVE_ADDRESS << 1) | 1);
	int i = 0;
	for (; i < VISION_BUFFER_SIZE - 1; i++)
	{
		priv_receivedData[i] = I2CRead(1); // read one byte and request another byte
	}
	priv_receivedData[i] = I2CRead(0);// read one byte and don't request another byte, stop transmission

	I2CStop();
	findObjectsFromBuffer();
}
