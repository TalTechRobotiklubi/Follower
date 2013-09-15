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

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */
void init(void);

int main(void)
{
	//Initialize system
	init();

	// initialize GPIOs
	GPIO_init();
	// initialize sensors
	Sensor_init();
	// initialize USART communication
	USART_init(USART2);

	//Turn LEDs on
	GPIO_outputOff(LED_GREEN);
	GPIO_outputOff(LED_ORANGE);
	GPIO_outputOff(LED_RED);
	GPIO_outputOff(LED_BLUE);

	CAN_CAN1Init();
	// init timers with max counting time 40 ms
	Timer_setOneShotCounter(TIMER2_ID, 40000, FALSE);
	Timer_setOneShotCounter(TIMER3_ID, 40000, FALSE);
	Timer_setOneShotCounter(TIMER4_ID, 40000, FALSE);

	//Initialize task handler timer
	TaskHandler_init();
	// run infinitely
	TaskHandler_run();

	//__disable_irq();

	return 0;
}

void init(void)
{
	//Enable AFIO (Alternate Function IO) clock
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//Disable JTAG, but leave SWD enabled
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
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

