#include "Acceleration.h"
#include "stm32f4_discovery_lis302dl.h"
#include "TaskHandler.h"
#include "DataLayer.h"

/*private variables*/
static int8_t priv_XOffset;
static int8_t priv_YOffset;
static int8_t priv_ZOffset;

/*private function declarations*/


/*function definitions*/

void Acceleration_init(void)
{
	uint8_t ctrl = 0;
	uint8_t buffer[6];
	uint16_t delayStart;

	LIS302DL_InitTypeDef  LIS302DL_InitStruct;
	LIS302DL_InterruptConfigTypeDef LIS302DL_InterruptStruct;

	/* SysTick end of count event each 10ms */
	//SysTick_Config(SystemCoreClock/ 100);

	/* Set configuration of LIS302DL*/
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE; //| LIS302DL_Z_ENABLE;
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	LIS302DL_Init(&LIS302DL_InitStruct);

	/* Set configuration of Internal High Pass Filter of LIS302DL*/
	LIS302DL_InterruptStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_LATCHED;
	//LIS302DL_InterruptStruct.SingleClick_Axes = LIS302DL_CLICKINTERRUPT_Z_ENABLE;
	//LIS302DL_InterruptStruct.DoubleClick_Axes = LIS302DL_DOUBLECLICKINTERRUPT_Z_ENABLE;
	LIS302DL_InterruptConfig(&LIS302DL_InterruptStruct);

	/* Required delay for the MEMS Accelerometer: Turn-on time = 3/Output data Rate
															 = 3/100 = 30ms */
	//delay 30 ms;
	delayStart = taskHandler_systemTicks;
	while ((delayStart + 30) < taskHandler_systemTicks);

	/* Configure Interrupt control register: enable Click interrupt1 */
	//ctrl = 0x07;
	//LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG3_ADDR, 1);

	/* Enable Interrupt generation on click/double click on Z axis */
	//ctrl = 0x70;
	//LIS302DL_Write(&ctrl, LIS302DL_CLICK_CFG_REG_ADDR, 1);

	/* Configure Click Threshold on X/Y axis (10 x 0.5g) */
	ctrl = 0xAA;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_THSY_X_REG_ADDR, 1);

	/* Configure Click Threshold on Z axis (10 x 0.5g) */
	//ctrl = 0x0A;
	//LIS302DL_Write(&ctrl, LIS302DL_CLICK_THSZ_REG_ADDR, 1);

	/* Configure Time Limit */
	ctrl = 0x03;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_TIMELIMIT_REG_ADDR, 1);

	/* Configure Latency */
	ctrl = 0x7F;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_LATENCY_REG_ADDR, 1);

	/* Configure Click Window */
	ctrl = 0x7F;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_WINDOW_REG_ADDR, 1);

	LIS302DL_Read(buffer, LIS302DL_OUT_X_ADDR, 6);

	priv_XOffset = buffer[0];
	priv_YOffset = buffer[2];
	priv_ZOffset = buffer[4];
}


void Acceleration_TASK(void)
{
	uint8_t buffer[6];

	/* read value and remove offset */
	LIS302DL_Read(buffer, LIS302DL_OUT_X_ADDR, 6);
	buffer[0] -= priv_XOffset;
	buffer[2] -= priv_YOffset;
	buffer[4] -= priv_ZOffset;

	DL_setData(DLParamAccelerationX, &buffer[0]);
	DL_setData(DLParamAccelerationY, &buffer[2]);
	DL_setData(DLParamAccelerationZ, &buffer[4]);
}
