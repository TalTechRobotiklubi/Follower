#include "stm32f10x.h"
#include "TASK_HANDLER.h"
#include "QEI.h"
#include "MODULATION.h"
#include "ANALOG.h"
#include "CAN.h"
#include "GPIO.h"

void init(void)
{
	//Enable AFIO (Alternate Function IO) clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//Disable JTAG, but leave SWD enabled
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
}

int main()
{
  	init();
	vModulationHardwareInit();
    vAnalogHardwareInit();
    QEI_init();

  	TaskHandler_init();
	TaskHandler_run();
	return 0;
}

