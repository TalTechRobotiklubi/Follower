#include "stm32f10x.h"
#include "GPIO.h"
#include "USART.h"
#include "CAN.h"
#include "TaskHandler.h"
#include "Typedef.h"
#include "OLED_LCD.h"
#include "Sensor.h"
#include "Timer.h"

//#define TRUE 1

void init(void);

int main(void)
{
	//Initialize system
	init();

	//Initialize communication
	USART_init();
	CAN_init(TRUE);
	// initialize GPIOs
	GPIO_init();
	// initialize sensors
	Sensor_init();
	// initialize screen
	OLED_Init();
	//Turn LED and buzzer off
	GPIO_outputOff(BUZZER);
	GPIO_outputOff(LED_RED);
	// init timer with max counting time 40 ms
	Timer_setOneShotCounter(TIMER2_ID, 40000, TRUE);
	//Initialize task handler timer
	TaskHandler_init();
	// run infinitely
	TaskHandler_run();

	return 0;
}

void init(void)
{
	//Enable AFIO (Alternate Function IO) clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//Disable JTAG, but leave SWD enabled
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
}
