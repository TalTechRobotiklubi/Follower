#include "usb.h"

static void sendCANmessage(InterfaceMessage* msg);

static InterfaceMessage messages[10];
static uint8_t messageCount = 0;

void USB_init()
{
	messageCount = 0;
}

void USB_task()
{
	InterfaceHandler_transmitData(InterfaceUSB, sendCANmessage);
}

uint8_t USB_sentMessageCount()
{
	return messageCount;
}

InterfaceMessage* USB_sentMessages()
{
	return messages;
}

void sendCANmessage(InterfaceMessage* msg)
{
	messages[messageCount] = *msg;
	++messageCount;
}
