#include "USART.h"

#include "InterfaceHandler.h"

#include "testGlobal.h"

static uint8_t hasNewPidSetupMessage = 0;
static InterfaceMessage pidSetupMessage;
static uint8_t hasNewGyroMessage = 0;
static InterfaceMessage gyroMessage;
static uint8_t hasNewQuaternionsMessage = 0;
static InterfaceMessage quaternionsMessage;

/*function definitions*/
void USART_init(void)
{
}

void USART_task(void)
{
	if (hasNewPidSetupMessage)
	{
		InterfaceHandler_storeReceivedData(&pidSetupMessage);
		hasNewPidSetupMessage = 0;
	}
	if (hasNewGyroMessage)
	{
		InterfaceHandler_storeReceivedData(&gyroMessage);
		hasNewPidSetupMessage = 0;
	}
	if (hasNewQuaternionsMessage)
	{
		InterfaceHandler_storeReceivedData(&quaternionsMessage);
		hasNewQuaternionsMessage = 0;
	}
}

void USART_setPidSetupMessagePending(uint8_t param, float value, uint8_t updating)
{
	SplitFloat data;
	data.fl = value;

	pidSetupMessage.packet = PacketPidSetup;
	pidSetupMessage.period = PacketDescriptorList[PacketPidSetup].period;
	pidSetupMessage.id = PacketDescriptorList[PacketPidSetup].id;
	pidSetupMessage.length = PacketDescriptorList[PacketPidSetup].dlc;
	pidSetupMessage.data[0] = param;
	pidSetupMessage.data[1] = data.d[3];
	pidSetupMessage.data[2] = data.d[2];
	pidSetupMessage.data[3] = data.d[1];
	pidSetupMessage.data[4] = data.d[0];
	pidSetupMessage.data[5] = updating << 7;
	hasNewPidSetupMessage = 1;
}

void USART_setGyroMessagePending(int16_t yaw, int16_t pitch, int16_t roll)
{
	SplitInt16 y, p, r;
	y.i = yaw;
	p.i = pitch;
	r.i = roll;
	gyroMessage.packet = PacketGyro;
	gyroMessage.period = PacketDescriptorList[PacketGyro].period;
	gyroMessage.id = PacketDescriptorList[PacketGyro].id;
	gyroMessage.length = PacketDescriptorList[PacketGyro].dlc;
	gyroMessage.data[0] = y.d[1];
	gyroMessage.data[1] = y.d[0];
	gyroMessage.data[2] = p.d[1];
	gyroMessage.data[3] = p.d[0];
	gyroMessage.data[4] = r.d[1];
	gyroMessage.data[5] = r.d[0];
	hasNewGyroMessage = 1;
}

void USART_setQuaternionsMessagePending(uint16_t qw, uint16_t qx, uint16_t qy, uint16_t qz)
{
	SplitUint16 qw_, qx_, qy_, qz_;
	qw_.u = qw;
	qx_.u = qx;
	qy_.u = qy;
	qz_.u = qz;
	quaternionsMessage.packet = PacketQuaternions;
	quaternionsMessage.period = PacketDescriptorList[PacketQuaternions].period;
	quaternionsMessage.id = PacketDescriptorList[PacketQuaternions].id;
	quaternionsMessage.length = PacketDescriptorList[PacketQuaternions].dlc;
	quaternionsMessage.data[0] = qw_.d[1];
	quaternionsMessage.data[1] = qw_.d[0];
	quaternionsMessage.data[2] = qx_.d[1];
	quaternionsMessage.data[3] = qx_.d[0];
	quaternionsMessage.data[4] = qy_.d[1];
	quaternionsMessage.data[5] = qy_.d[0];
	quaternionsMessage.data[6] = qz_.d[1];
	quaternionsMessage.data[7] = qz_.d[0];
	hasNewQuaternionsMessage = 1;
}
