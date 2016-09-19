#include <iostream>

#include <UnitTest++.h>
#include "projectConfig.h"

extern "C" {
#include "SystemTicks.h"
#include "TaskHandler.h"
#include "GPIO.h"
#include "USART.h"
#include "DataLayer.h"
}

TEST(pidSetupDataIsInvalidByDefault)
{
	uint8_t actParam = 0;
	float actVal = 0.0;
	Boolean actUpdating = FALSE;

	CHECK(!DL_getData(DLParamPidParameter, &actParam));
	CHECK(!DL_getData(DLParamPidValue, &actVal));
	CHECK(!DL_getData(DLParamPidUpdating, &actUpdating));
}

TEST(PidSetupMessageDataIsSetToDataLayer)
{
	uint8_t expParam = 3;
	float expVal = 34.0;
	Boolean expUpdating = TRUE;
	uint8_t actParam = 0;
	float actVal = 0.0;
	Boolean actUpdating = FALSE;

	TaskHandler_setPreviousTicks(100);
	systemTicks = UsbOffset;
	USART_setPidSetupMessagePending(expParam, expVal, expUpdating);
	TaskHandler_run();

	DL_getData(DLParamPidParameter, &actParam);
	DL_getData(DLParamPidValue, &actVal);
	DL_getData(DLParamPidUpdating, &actUpdating);
	CHECK_EQUAL(expParam, actParam);
	CHECK_EQUAL(expVal, actVal);
	CHECK_EQUAL(expUpdating, actUpdating);
}

TEST(AsyncPidSetupDataIsValidOnlyOnce)
{
	uint8_t expParam = 3;
	float expVal = 34.0;
	Boolean expUpdating = TRUE;
	uint8_t actParam = 0;
	float actVal = 0.0;
	Boolean actUpdating = FALSE;

	TaskHandler_setPreviousTicks(100);
	systemTicks = UsbOffset;
	USART_setPidSetupMessagePending(expParam, expVal, expUpdating);
	TaskHandler_run();

	CHECK(DL_getData(DLParamPidParameter, &actParam));
	CHECK(DL_getData(DLParamPidValue, &actVal));
	CHECK(DL_getData(DLParamPidUpdating, &actUpdating));
	CHECK(!DL_getData(DLParamPidParameter, &actParam));
	CHECK(!DL_getData(DLParamPidValue, &actVal));
	CHECK(!DL_getData(DLParamPidUpdating, &actUpdating));
}
