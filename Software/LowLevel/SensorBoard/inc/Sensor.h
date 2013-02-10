#ifndef SENSOR_H_
#define SENSOR_H_

/*
typedef enum
{
	DISTANCE_SENSOR_1 = 0,
	DISTANCE_SENSOR_2 = 1,
	DISTANCE_SENSOR_3 = 2,
	DISTANCE_SENSOR_4 = 3,
	DISTANCE_SENSOR_5 = 4,
	DISTANCE_SENSOR_6 = 5,
	NUM_OF_SENSORS
}
Sensor_Typedef;*/

/*
typedef enum
{
	URF_HC_SR04,
	URF_LV_EZ
}
SensorMark_Typedef;*/

//#define GPIOn				2
/*
#define BUZZER_PIN 		    GPIO_Pin_0
#define BUZZER_GPIO_PORT	GPIOB
#define BUZZER_GPIO_CLK	    RCC_APB2Periph_GPIOB

#define LED_RED_PIN 		GPIO_Pin_15
#define LED_RED_GPIO_PORT	GPIOB
#define LED_RED_GPIO_CLK	RCC_APB2Periph_GPIOB

#define BUTTON_TOP_LEFT_PIN        		GPIO_Pin_14  //PC14, SW1
#define BUTTON_TOP_LEFT_GPIO_PORT  		GPIOC
#define BUTTON_TOP_LEFT_GPIO_CLK   		RCC_APB2Periph_GPIOC

#define BUTTON_TOP_RIGHT_PIN        	GPIO_Pin_12  //PB12, SW2
#define BUTTON_TOP_RIGHT_GPIO_PORT  	GPIOB
#define BUTTON_TOP_RIGHT_GPIO_CLK  	   	RCC_APB2Periph_GPIOB

#define BUTTON_BOTTOM_LEFT_PIN         	GPIO_Pin_13  //PC13, SW4
#define BUTTON_BOTTOM_LEFT_GPIO_PORT   	GPIOC
#define BUTTON_BOTTOM_LEFT_GPIO_CLK    	RCC_APB2Periph_GPIOA

#define BUTTON_BOTTOM_RIGHT_PIN        	GPIO_Pin_15  //PA15, SW3
#define BUTTON_BOTTOM_RIGHT_GPIO_PORT  	GPIOA
#define BUTTON_BOTTOM_RIGHT_GPIO_CLK   	RCC_APB2Periph_GPIOC
*/
extern void Sensor_init(void);

extern void Sensor_TASK_startMeasurement1(void);
extern void Sensor_TASK_startMeasurement2(void);
extern void Sensor_TASK_readDistance1(void);
extern void Sensor_TASK_readDistance2(void);

#endif /*LED_H_*/
