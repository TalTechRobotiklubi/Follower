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

TEST(quaternionsDataIsInvalidByDefault)
{
	int16_t qw = 0;
	int16_t qx = 0;
	int16_t qy = 0;
	int16_t qz = 0;

	CHECK(!DL_getData(DLParamQw, &qw));
	CHECK(!DL_getData(DLParamQx, &qx));
	CHECK(!DL_getData(DLParamQy, &qy));
	CHECK(!DL_getData(DLParamQz, &qz));
}

TEST(quaternionsMessageDataIsSetToDataLayer)
{
	int16_t expQw = 0;
	int16_t expQx = 0;
	int16_t expQy = 0;
	int16_t expQz = 0;
	int16_t actQw = 0;
	int16_t actQx = 0;
	int16_t actQy = 0;
	int16_t actQz = 0;

	TaskHandler_setPreviousTicks(100);
	systemTicks = UsartOffset;
	USART_setQuaternionsMessagePending(expQw, expQx, expQy, expQz);
	TaskHandler_run();

	DL_getData(DLParamQw, &actQw);
	DL_getData(DLParamQx, &actQx);
	DL_getData(DLParamQy, &actQy);
	DL_getData(DLParamQz, &actQz);
	CHECK_EQUAL(expQw, actQw);
	CHECK_EQUAL(expQx, actQx);
	CHECK_EQUAL(expQy, actQy);
	CHECK_EQUAL(expQz, actQz);
}

TEST(quaterionsDataIsSentOutPeriodicallyViaCan)
{
	int16_t expQw = 312;

	CAN_init();
	DL_init();
	DL_setData(DLParamQw, &expQw);

	TaskHandler_setPreviousTicks(100);
	systemTicks = DataHandlerOffset;
	TaskHandler_run();

	InterfaceTransmitPacket transmitPacket = InterfaceList[InterfaceCAN].transmitPacketList[QuaterionsPacketTransmitIndex];
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
	CHECK_EQUAL(2, cnt);  // +1 is distance sensor message

	InterfaceMessage msg = CAN_sentMessages()[1];
	CHECK_EQUAL(242, msg.id);
	SplitInt16 s;
	s.d[1] = msg.data[0];
	s.d[0] = msg.data[1];
	CHECK_EQUAL(expQw, s.i);
}
