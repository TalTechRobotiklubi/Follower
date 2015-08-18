#ifndef _CAN_MESSAGES_H_
#define _CAN_MESSAGES_H_

#define PC_QT_APPLICATION

#define DATA_BUFFER_SIZE 32

#ifdef PC_QT_APPLICATION
#include "qglobal.h"
/*type redifinitions*/
	typedef qint8	int8_t;
	typedef quint8  uint8_t;
	typedef qint16  int16_t;
	typedef quint16 uint16_t;
	//typedef bool	boolean_t;
#endif

/*General CAN message struct*/
typedef struct 
{
	uint8_t id;
	uint8_t dlc;
    uint8_t data[DATA_BUFFER_SIZE];
} CANmessage;

/*General UART CAN message struct*/
typedef struct
{
	CANmessage canMessage;
	uint16_t crc;
} UART_CANmessage;

#endif
