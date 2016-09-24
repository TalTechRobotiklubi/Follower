#include "usb.h"
#include "projectConfig.h"

static void sendUSBmessage(InterfaceMessage* msg);

static InterfaceMessage messages[10];
static uint8_t messageCount = 0;

static uint8_t hasNewRobotControlMessage = 0;
static InterfaceMessage robotControlMessage;

void USB_init()
{
	messageCount = 0;
}

void USB_task()
{
	if (hasNewRobotControlMessage)
	{
		InterfaceHandler_storeReceivedData(&robotControlMessage);
		hasNewRobotControlMessage = 0;
	}
	InterfaceHandler_transmitData(InterfaceUSB, sendUSBmessage);
}

uint8_t USB_sentMessageCount()
{
	return messageCount;
}

InterfaceMessage* USB_sentMessages()
{
	return messages;
}

void sendUSBmessage(InterfaceMessage* msg)
{
	messages[messageCount] = *msg;
	++messageCount;
}

void USB_setRobotConstrolMessagePending(int16_t translationSpeed, int16_t rotationSpeed)
{
	SplitInt16 tSpeed;
	tSpeed.i = translationSpeed;
	SplitInt16 wSpeed;
	wSpeed.i = rotationSpeed;
	robotControlMessage.interface = InterfaceUSB;
	robotControlMessage.packet = &PacketDescriptorList[PacketRobotControl];
	robotControlMessage.id = PacketDescriptorList[PacketRobotControl].id;
	robotControlMessage.length = PacketDescriptorList[PacketRobotControl].dlc;
	robotControlMessage.data[0] = tSpeed.d[1];
	robotControlMessage.data[1] = tSpeed.d[0];
	robotControlMessage.data[2] = wSpeed.d[1];
	robotControlMessage.data[3] = wSpeed.d[0];
	robotControlMessage.data[4] = 0;
	robotControlMessage.data[5] = 0;
	robotControlMessage.data[6] = 0;
	robotControlMessage.data[7] = 0;
	hasNewRobotControlMessage = 1;
}
