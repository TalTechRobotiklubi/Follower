#include "CAN.h"

static void sendCANmessage(InterfaceMessage* msg);

static InterfaceMessage messages[10];
static uint8_t messageCount = 0;

static uint8_t hasNewDistanceMessage = 0;
static InterfaceMessage distanceMessage;


void CAN_init(void)
{
	messageCount = 0;
}

void CAN_task(void)
{
	if (hasNewDistanceMessage)
	{
		InterfaceHandler_storeReceivedData(&distanceMessage);
		hasNewDistanceMessage = 0;
	}
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

void CAN_setDistanceMessagePending(uint8_t dist1, uint8_t dist2)
{
	distanceMessage.packet = &PacketDescriptorList[PacketDistanceSensors];
	distanceMessage.period = PacketDescriptorList[PacketDistanceSensors].period;
	distanceMessage.id = PacketDescriptorList[PacketDistanceSensors].id;
	distanceMessage.length = PacketDescriptorList[PacketDistanceSensors].dlc;
	distanceMessage.data[0] = dist1;
	distanceMessage.data[1] = dist2;
	distanceMessage.data[2] = 0;
	distanceMessage.data[3] = 0;
	distanceMessage.data[4] = 0;
	distanceMessage.data[5] = 0;
	distanceMessage.data[6] = 0;
	distanceMessage.data[7] = 0;
	hasNewDistanceMessage = 1;
}
