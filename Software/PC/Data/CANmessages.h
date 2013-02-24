#ifndef _CAN_MESSAGES_H_
#define _CAN_MESSAGES_H_

#define PC_QT_APPLICATION


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
	uint8_t data[8];
} CANmessage;

/*General UART CAN message struct*/
typedef struct
{
	CANmessage canMessage;
	uint16_t crc;
} UART_CANmessage;

#if 0
/* Message union*/
typedef union 
{
	struct
	{
        uint8_t preambula;
        UART_CANmessage uartCanMsg;
    } Message;
	uint8_t fields[13];
} MessageUnion;



/* Motor Speeds message*/
typedef union 
{
	struct
	{
		int16_t		Motor1RequestSpeed;
		int16_t		Motor2RequestSpeed;
		int16_t		Motor3RequestSpeed;
	} MsgSignals;
	uint8_t data[6];
} MotorSpeeds;

typedef struct 
{
	uint8_t id;
	uint8_t dlc;
	MotorSpeeds data;
	uint16_t crc;
} CAN_MotorSpeedsMessage;

/* Motor 1 Status message*/
typedef union
{
	struct
	{
		int16_t		Motor1ActualSpeed;
		uint8_t		Motor1CurrentDraw;
		uint8_t		Motor1DriverTemp;
		uint8_t		Motor1BridgeAFault : 1;
		uint8_t		Motor1BridgeBFault : 1;
		uint8_t		usused : 6;
	} MsgSignals;
	uint8_t data[5];
} Motor1Status;

typedef struct
{
	uint8_t id;
	uint8_t dlc;
	Motor1Status data;
	uint16_t crc;
} CAN_Motor1StatusMessage;

/* Motor 2 Status message*/
typedef union
{
	struct
	{
		int16_t		Motor2ActualSpeed;
		uint8_t		Motor2CurrentDraw;
		uint8_t		Motor2DriverTemp;
		uint8_t		Motor2BridgeAFault : 1;
		uint8_t		Motor2BridgeBFault : 1;
		uint8_t		usused : 6;
	} MsgSignals;
	uint8_t data[5];
} Motor2Status;

typedef struct
{
	uint8_t id;
	uint8_t dlc;
	Motor2Status data;
	uint16_t crc;
} CAN_Motor2StatusMessage;

/* Motor 3 Status message*/
typedef union
{
	struct
	{
		int16_t		Motor3ActualSpeed;
		uint8_t		Motor3CurrentDraw;
		uint8_t		Motor3DriverTemp;
		uint8_t		Motor3BridgeAFault : 1;
		uint8_t		Motor3BridgeBFault : 1;
		uint8_t		usused : 6;
	} MsgSignals;
	uint8_t data[5];
} Motor3Status;

typedef struct
{
	uint8_t id;
	uint8_t dlc;
	Motor3Status data;
	uint16_t crc;
} CAN_Motor3StatusMessage;

/* Ball Control message*/
typedef union
{
	struct
	{
		// 1st byte
        uint8_t     BallKickTime1 : 6;
		uint8_t		BallCharge : 1;
		uint8_t		BallKick : 1;
        // 2nd byte
		uint8_t     BallDribblerPower1 : 4;
        uint8_t     BallKickTime2 : 4;		
		// 3rd byte
        uint8_t		usused : 5;
        uint8_t 	BallDribblerPower2 : 3;
	} MsgSignals;
	uint8_t data[3];
} BallControl;

typedef struct
{
	uint8_t id;
	uint8_t dlc;
	BallControl data;
	uint16_t crc;
} CAN_BallControlMessage;

/* Ball Status message*/
typedef union
{
	struct
	{
		// 1st byte
        uint8_t     unused : 5;
        uint8_t     BallCapacitorCharged : 1;
		uint8_t		BallInSight : 1;
		uint8_t		BallContact : 1;
	} MsgSignals;
	uint8_t data[1];
} BallStatus;

typedef struct
{
	uint8_t id;
	uint8_t dlc;
	BallStatus data;
	uint16_t crc;
} CAN_BallStatusMessage;

/* Message Ids*/
enum CANmessageId
{
	MotorSpeedsId = 0xd0,
	Motor1StatusId = 0xd1,
	Motor2StatusId = 0xd2,
	Motor3StatusId = 0xd3,
	BallControlId = 0xb0,
    BallStatusId = 0xb1
};

#define CAN_NUMBER_OF_RX_MESSAGES  4
#define CAN_NUMBER_OF_TX_MESSAGES  2
// define array indexes for all Rx messages
#define MOTOR_1_STATUS_INDEX  0
#define MOTOR_2_STATUS_INDEX  1
#define MOTOR_3_STATUS_INDEX  2
#define BALL_STATUS_INDEX     3
// define array indexes for all Tx messages
#define MOTOR_SPEEDS_INDEX 0
#define BALL_CONTROL_INDEX 1

#endif

#endif