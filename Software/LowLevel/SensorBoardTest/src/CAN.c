#include "CAN.h"

static void sendCANmessage(InterfaceMessage* msg);

static InterfaceMessage messages[10];
static uint8_t messageCount = 0;

/*function definitions*/
void CAN_init(void)
{
	messageCount = 0;
}

void CAN_task(void)
{
	InterfaceHandler_transmitData(InterfaceCAN, sendCANmessage);
}

uint8_t CAN_sentMessageCount()
{
	return messageCount;
}

InterfaceMessage* CAN_sentMessages()
{
	return messages;
}

void sendCANmessage(InterfaceMessage* msg)
{
	messages[messageCount] = *msg;
	++messageCount;
}

