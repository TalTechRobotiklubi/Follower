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

TEST(robotControlDataIsStoredToDataLayer)
{
	int16_t expD1 = 12;
	int16_t expD2 = 65;
	int16_t actD1 = 0;
	int16_t actD2 = 0;

	USB_init();
	DL_init();

	TaskHandler_setPreviousTicks(100);
	systemTicks = UsbOffset;
	USB_setRobotConstrolMessagePending(expD1, expD2);
	TaskHandler_run();

	DL_getData(DLParamRequestTranslationSpeed, &actD1);
	DL_getData(DLParamRequestRotationSpeed, &actD2);
	CHECK_EQUAL(expD1, actD1);
	CHECK_EQUAL(expD2, actD2);
}

TEST(robotControlDataIsForwardedFromUsbToCan)
{
	int16_t expD1 = 12;
	int16_t expD2 = 65;

	USB_init();
	CAN_init();
	DL_init();

	TaskHandler_setPreviousTicks(100);
	systemTicks = UsbOffset;
	USB_setRobotConstrolMessagePending(expD1, expD2);
	TaskHandler_run();

	systemTicks = DataHandlerOffset;
	TaskHandler_run();

	systemTicks = CanOffset;
	TaskHandler_run();

	CHECK_EQUAL(1, CAN_sentMessageCount());
}
