/* Includes */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "GPIO.h"
#include "TaskHandler.h"
#include "TypeDef.h"
#include "Timer.h"
#include "Sensor.h"
#include "USART.h"
#include "CAN.h"


int main(void)
{
	// init timers with max counting time 40 ms
	Timer_setOneShotCounter(TIMER2_ID, 40000, FALSE);
	Timer_setOneShotCounter(TIMER3_ID, 40000, FALSE);
	//Timer_setOneShotCounter(TIMER4_ID, 40000, FALSE);

	//Initialize task handler timer
	TaskHandler_init();
	// run infinitely
	TaskHandler_run();

	//__disable_irq();

	return 0;
}


/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}

