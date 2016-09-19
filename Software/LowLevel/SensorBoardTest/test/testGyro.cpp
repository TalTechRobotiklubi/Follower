#include <iostream>

#include <UnitTest++.h>
#include "projectConfig.h"

extern "C" {
#include "SystemTicks.h"
#include "TaskHandler.h"
#include "USART.h"
#include "DataLayer.h"
}

TEST(gyroDataIsInvalidByDefault)
{
	uint16_t yaw = 0;
	uint16_t pitch = 0;
	uint16_t roll = 0;

	CHECK(!DL_getData(DLParamGyroX, &yaw));
	CHECK(!DL_getData(DLParamGyroY, &pitch));
	CHECK(!DL_getData(DLParamGyroZ, &roll));
}

TEST(gyroMessageDataIsSetToDataLayer)
{
	int16_t expYaw = 23;
	int16_t expPitch = 34;
	int16_t expRoll = 45;
	int16_t actYaw = 0;
	int16_t actPitch = 0;
	int16_t actRoll = 0;

	TaskHandler_setPreviousTicks(100);
	systemTicks = UsbOffset;
	USART_setGyroMessagePending(expYaw, expPitch, expRoll);
	TaskHandler_run();

	DL_getData(DLParamGyroX, &actYaw);
	DL_getData(DLParamGyroY, &actPitch);
	DL_getData(DLParamGyroZ, &actRoll);
	CHECK_EQUAL(expYaw, actYaw);
	CHECK_EQUAL(expPitch, actPitch);
	CHECK_EQUAL(expRoll, actRoll);
}

