/*
 * IMU.c
 *
 *  Created on: Mar 1, 2016
 *      Author: peeter
 */
#include "IMU.h"

#include "USART.h"
#include "DataLayer.h"

#define FieldMaxSize 10
#define Quaternion_Size 4

typedef struct
{
	uint8_t head;
	uint8_t tail;
}
ReceivingDataState;

typedef enum {
  FindEnd,
  Reinit,
  ParseFields
} AnalyzeState;

typedef struct {
  int16_t qs[Quaternion_Size];
  int32_t timestamp;
} ParsedData;

static void buffersInit();
static void clearData(ParsedData* data);
static void clearField(char* field);
static void clear(ParsedData* data, char* field, int* fieldIndex, int* dataIndex);
static void calculateData(char* field, ParsedData* data, int dataIndex);
static void pushDataToDataLayer(ParsedData* data);
static void handleReceivedData();

static unsigned char priv_UART4_RxBuf[USART_RX_BUFFER_SIZE];
static ReceivingDataState priv_UART4_RxState;
static volatile uint8_t priv_UART4_RxOverflow;

void buffersInit()
{
	priv_UART4_RxState.tail = 0;
	priv_UART4_RxState.head = 0;
}

void clearData(ParsedData* data)
{
  data->qs[0] = 0;
  data->qs[1] = 0;
  data->qs[2] = 0;
  data->qs[3] = 0;
  data->timestamp = 0;
};

void clearField(char* field)
{
	int i = 0;
	for (; i < FieldMaxSize; ++i)
		field[i] = 0;
}

void clear(ParsedData* data, char* field, int* fieldIndex, int* dataIndex)
{
  clearData(data);
  clearField(field);
  *fieldIndex = 0;
  *dataIndex = 0;
}

void calculateData(char* field, ParsedData* data, int dataIndex)
{
  int sign = 1;
  int index = 0;
  if (field[index] == 0x2D) {
    sign = -1;
    ++index;
  }

  int32_t value = 0;
  while (index < FieldMaxSize && field[index] >= '0') {
    value *= 10;
    value += (int)field[index] - '0';
    ++index;
  }
  value *= sign;

  data->qs[dataIndex] = value;
  if (dataIndex == Quaternion_Size)
    data->timestamp = value;
}

static void pushDataToDataLayer(ParsedData* data)
{
	float q1 = data->qs[0];
	float q2 = data->qs[1];
	float q3 = data->qs[2];
	float q4 = data->qs[3];
	DL_setData(DLParamImuQ1, &q1);
	DL_setData(DLParamImuQ1, &q2);
	DL_setData(DLParamImuQ1, &q3);
	DL_setData(DLParamImuQ1, &q4);
}

void handleReceivedData()
{
	static char field[FieldMaxSize];
	static int fieldIndex = 0;
	static AnalyzeState state = FindEnd;
	static ParsedData data;
	static int dataIndex = 0;

	uint8_t tempHead;
  	uint8_t tempTail;

  	/*save the buffer tail and head into local variable*/
  	USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);  //disable UART receive interrupts
  	tempHead = priv_UART4_RxState.head;
  	tempTail = priv_UART4_RxState.tail;
  	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);  // enable UART receive interrupts

  	/*Analyze received data*/
  	while (tempTail != tempHead)
  	{
		switch (state) {
		case FindEnd:
			if (priv_UART4_RxBuf[tempTail] == 0x0D)
				state = Reinit;
			break;
		case Reinit:
			if (priv_UART4_RxBuf[tempTail] == 0x0A)
			{
				clear(&data, field, &fieldIndex, &dataIndex);
				state = ParseFields;
			}
			else
				state = FindEnd;
			break;
		case ParseFields:
			if (priv_UART4_RxBuf[tempTail] == 0x0D)
			{
				calculateData(field, &data, dataIndex);
				pushDataToDataLayer(&data);
				state = Reinit;
			}
			else if (priv_UART4_RxBuf[tempTail] == 0x20)
			{
				calculateData(field, &data, dataIndex);
				clearField(field);
				fieldIndex = 0;
				++dataIndex;
			}
			else
			{
				field[fieldIndex] = priv_UART4_RxBuf[tempTail];
				++fieldIndex;
			}
			break;
		}
		/*take next byte*/
		tempTail = tempTail + 1;
		if (tempTail >= USART_RX_BUFFER_SIZE)
		{
			tempTail = 0;
		}
  	}
}

void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		unsigned char rxdata;
		uint8_t tempHead;

		//Read one byte from receive data register
		rxdata = USART_ReceiveData(UART4);
		//Calculate buffer index
		tempHead = (priv_UART4_RxState.head + 1) & USART_RX_BUFFER_MASK;
		priv_UART4_RxState.head = tempHead;

		if(tempHead == priv_UART4_RxState.tail)
		{
			//ERROR: buffer overflow
			priv_UART4_RxOverflow = 1;
		}
		else
		{
			priv_UART4_RxBuf[tempHead] = rxdata;
			priv_UART4_RxOverflow = 0;
		}
	}
}

void IMU_init()
{
	buffersInit();
    USART_init(UART4);
}

void IMU_task()
{
	handleReceivedData();
}

