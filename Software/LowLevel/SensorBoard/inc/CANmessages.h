#ifndef _CAN_MESSAGES_H_
#define _CAN_MESSAGES_H_


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


/* Message Ids*/
enum CANmessageId
{
	MotorSpeedsId = 0xd0,
	Motor1StatusId = 0xd1,
	Motor2StatusId = 0xd2,
	Motor3StatusId = 0xd3,
	BallControlId = 0xb0
};


#endif
