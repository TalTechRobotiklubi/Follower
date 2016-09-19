#include <iostream>

#include <UnitTest++.h>
#include "projectConfig.h"

extern "C" {
#include "SystemTicks.h"
#include "TaskHandler.h"
#include "usb.h"
#include "CAN.h"
#include "DataLayer.h"
#include "InterfaceConfig.h"
}

TEST(distanceSensorDataIsStoredToDataLayer)
{
	uint8_t expD1 = 12;
	uint8_t expD2 = 65;
	uint8_t actD1 = 0;
	uint8_t actD2 = 0;

	CAN_init();
	DL_init();

	TaskHandler_setPreviousTicks(100);
	systemTicks = CanOffset;
	CAN_setDistanceMessagePending(expD1, expD2);
	TaskHandler_run();

	DL_getData(DLParamDistanceSensor1, &actD1);
	DL_getData(DLParamDistanceSensor2, &actD2);
	CHECK_EQUAL(expD1, actD1);
	CHECK_EQUAL(expD2, actD2);
}

TEST(distanceSensorDataIsForwardedFromCanToUsb)
{
	uint8_t expD1 = 12;
	uint8_t expD2 = 65;

	CAN_init();
	DL_init();

	TaskHandler_setPreviousTicks(100);
	systemTicks = CanOffset;
	CAN_setDistanceMessagePending(expD1, expD2);
	TaskHandler_run();

	systemTicks = DataHandlerOffset;
	TaskHandler_run();

	systemTicks = UsbOffset;
	TaskHandler_run();

	CHECK_EQUAL(1, USB_sentMessageCount());
}
