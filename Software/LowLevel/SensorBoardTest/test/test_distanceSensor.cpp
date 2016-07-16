#include <iostream>

#include <UnitTest++.h>
#include "testGlobal.h"

extern "C" {
#include "SystemTicks.h"
#include "TaskHandler.h"
#include "USART.h"
#include "CAN.h"
#include "DataLayer.h"
#include "InterfaceConfig.h"
}

TEST(distanceSensorDataIsSentOutViaCan)
{
	uint8_t expD1 = 12;
	uint8_t expD2 = 23;

	CAN_init();
	DL_init();
	DL_setData(DLParamDistanceSensor1, &expD1);
	DL_setData(DLParamDistanceSensor2, &expD2);

	TaskHandler_setPreviousTicks(100);
	systemTicks = DataHandlerOffset;
	TaskHandler_run();

	InterfaceTransmitPacket transmitPacket = InterfaceList[InterfaceCAN].transmitPacketList[DistanceSensorPacketTransmitIndex];
	int16_t packetPeriod = transmitPacket.period;
	int16_t dataHandlerPeriod = TaskHandler_tableOfTasks[TASK_DATAHANDLER].period;
	for (int i = 1; i < packetPeriod / dataHandlerPeriod; ++i)
	{
		systemTicks += dataHandlerPeriod;
		TaskHandler_run();
	}
	systemTicks += CanOffset - DataHandlerOffset;
	TaskHandler_run();

	uint8_t cnt = CAN_sentMessageCount();
	CHECK_EQUAL(2, cnt);  // +1 is quaterions message

	InterfaceMessage msg = CAN_sentMessages()[0];
	CHECK_EQUAL(240, msg.id);
	CHECK_EQUAL(expD1, msg.data[0]);
	CHECK_EQUAL(expD2, msg.data[1]);
}

