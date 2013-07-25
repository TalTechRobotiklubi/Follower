#include "stm32f10x.h"
#include "PARAMS.h"
#include "EEPROM.h"
#include "TASK_HANDLER.h"
#include "QEI.h"
#include "MODULATION.h"
#include "ANALOG.h"
#include "CAN.h"
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
	EE_init();
 	vParametersConnectionsInit();
	vCanHardwareInit();
	vModulationHardwareInit();
    vAnalogHardwareInit();
    vQeiHardwareInit();
  	TaskHandler_init();


  	//Structure for GPIO initialization
  	GPIO_InitTypeDef GPIO_InitStructure;

  	//Enable clock on pin
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

  	//Configure pin
  	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_3;
  	//Configure pin as push-pull output
  	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;
  	//GPIO speed 50MHz
  	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_10MHz;

  	//Initialize pin
  	GPIO_Init(GPIOB,&GPIO_InitStructure);


	//Enable clock on pin
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

  	//Configure pin
  	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_15;
  	//Configure pin as push-pull output
  	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;
  	//GPIO speed 50MHz
  	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_10MHz;

  	//Initialize pin
  	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIOA->BRR = GPIO_Pin_15;
	TaskHandler_run();
  	while(1)
  	{

  	}
}

