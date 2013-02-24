#include "Sensor.h"
#include "stm32f10x.h"
#include "DataLayer.h"
#include "GPIO.h"
#include "Timer.h"

typedef enum
{
	SENSOR_INPUT_IDLE,
	SENSOR_INPUT_RISED,
	SENSOR_INPUT_FALLED
}
SensorInputStatus_Typedef;


/*private variables*/
SensorInputStatus_Typedef 	priv_sensor1Stat = SENSOR_INPUT_IDLE;
SensorInputStatus_Typedef 	priv_sensor2Stat = SENSOR_INPUT_IDLE;
SensorInputStatus_Typedef 	priv_sensor3Stat = SENSOR_INPUT_IDLE;
SensorInputStatus_Typedef 	priv_sensor4Stat = SENSOR_INPUT_IDLE;
uint16_t 					priv_sensor1Value = 0;
uint16_t 					priv_sensor2Value = 0;
uint16_t 					priv_sensor3Value = 0;
uint16_t 					priv_sensor4Value = 0;

/*private function declarations*/
void initDistanceSensor(GPIO_IdDef io);

/*function definitions*/
void Sensor_init(void)
{
	//initDistanceSensor(DISTANCE_SENOR_1);
}

//
void initDistanceSensor(GPIO_IdDef io)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOB clock */
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(GPIO_table[io].clk, ENABLE);

	/* Configure pin as floating input */
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_table[io].pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIO_table[io].port, &GPIO_InitStructure);

	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Connect EXTIxx Line to pin */
	//GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	GPIO_EXTILineConfig(GPIO_table[io].portSource, GPIO_table[io].pinSource);

	/* Configure EXTIxx line */
	//EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_Line = GPIO_table[io].exti;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Interrupt to the lowest priority */
	//NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannel = GPIO_table[io].interrupt;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
* @brief  This function handles External line 4 interrupt request.
* @param  None
* @retval None
*/
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(GPIO_table[URF4_ECHO].exti) != RESET)
	{

		if (GPIO_inputValue(URF4_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER2_ID);
			priv_sensor4Stat = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensor4Stat == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER2_ID);
				// read measurement result
				priv_sensor4Value = Timer_getTimerValue(TIMER2_ID);
				/*disable pin detection*/
				EXTI_DeInit();
			}
			priv_sensor4Stat = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF4_ECHO].exti);
	}
}

/**
* @brief  This function handles External lines 9 to 5 interrupt request.
* @param  None
* @retval None
*/
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(GPIO_table[URF1_ECHO].exti) != RESET)
    {

		if (GPIO_inputValue(URF1_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER2_ID);
			priv_sensor1Stat = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensor1Stat == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER2_ID);
				// read measurement result
				priv_sensor1Value = Timer_getTimerValue(TIMER2_ID);
				/*disable pin detection*/
				EXTI_DeInit();
			}
			priv_sensor1Stat = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF1_ECHO].exti);
    }

	if(EXTI_GetITStatus(GPIO_table[URF2_ECHO].exti) != RESET)
	{

		if (GPIO_inputValue(URF2_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER2_ID);
			priv_sensor2Stat = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensor2Stat == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER2_ID);
				// read measurement result
				priv_sensor2Value = Timer_getTimerValue(TIMER2_ID);
				/*disable pin detection*/
				EXTI_DeInit();
			}
			priv_sensor2Stat = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF2_ECHO].exti);
	}

	if(EXTI_GetITStatus(GPIO_table[URF3_ECHO].exti) != RESET)
	{

		if (GPIO_inputValue(URF3_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER2_ID);
			priv_sensor3Stat = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensor3Stat == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER2_ID);
				// read measurement result
				priv_sensor3Value = Timer_getTimerValue(TIMER2_ID);
				/*disable pin detection*/
				EXTI_DeInit();
			}
			priv_sensor3Stat = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF3_ECHO].exti);
	}
}

void Sensor_TASK_startMeasurement1(void)
{
	priv_sensor1Stat = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF1_TRIG);
	Timer_startTimer(TIMER2_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER2_ID) < 20);
	Timer_stopTimer(TIMER2_ID);
	// set trigger output low
	GPIO_outputOff(URF1_TRIG);
	// enable input pin for sensor 2
	initDistanceSensor(URF1_ECHO);
}

void Sensor_TASK_readDistance1(void)
{
	uint16_t distance = 0;
	// measurement has taken too long
	if (priv_sensor1Stat == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER2_ID);
		priv_sensor1Stat = SENSOR_INPUT_IDLE;
		priv_sensor1Value = 0;
	}
	// disable pin detection anyway
	EXTI_DeInit();
	/*write distance result in cm to DL*/
	distance = priv_sensor1Value / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor1, &distance);
}

void Sensor_TASK_startMeasurement2(void)
{
	priv_sensor2Stat = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF2_TRIG);
	Timer_startTimer(TIMER2_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER2_ID) < 20);
	Timer_stopTimer(TIMER2_ID);
	// set trigger output low
	GPIO_outputOff(URF2_TRIG);
	// enable input pin for sensor 2
	initDistanceSensor(URF2_ECHO);
}

void Sensor_TASK_readDistance2(void)
{
	uint16_t distance = 0;
	// measurement has taken too long
	if (priv_sensor2Stat == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER2_ID);
		priv_sensor2Stat = SENSOR_INPUT_IDLE;
		priv_sensor2Value = 0;
	}
	// disable pin detection anyway
	EXTI_DeInit();
	/*write distance result in cm to DL*/
	distance = priv_sensor2Value / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor2, &distance);
}

void Sensor_TASK_startMeasurement3(void)
{
	priv_sensor3Stat = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF3_TRIG);
	Timer_startTimer(TIMER2_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER2_ID) < 20);
	Timer_stopTimer(TIMER2_ID);
	// set trigger output low
	GPIO_outputOff(URF3_TRIG);
	// enable input pin for sensor 2
	initDistanceSensor(URF3_ECHO);
}

void Sensor_TASK_readDistance3(void)
{
	uint16_t distance = 0;
	// measurement has taken too long
	if (priv_sensor3Stat == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER2_ID);
		priv_sensor3Stat = SENSOR_INPUT_IDLE;
		priv_sensor3Value = 0;
	}
	// disable pin detection anyway
	EXTI_DeInit();
	/*write distance result in cm to DL*/
	distance = priv_sensor3Value / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor3, &distance);
}

void Sensor_TASK_startMeasurement4(void)
{
	priv_sensor4Stat = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF4_TRIG);
	Timer_startTimer(TIMER2_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER2_ID) < 20);
	Timer_stopTimer(TIMER2_ID);
	// set trigger output low
	GPIO_outputOff(URF4_TRIG);
	// enable input pin for sensor 2
	initDistanceSensor(URF4_ECHO);
}

void Sensor_TASK_readDistance4(void)
{
	uint16_t distance = 0;
	// measurement has taken too long
	if (priv_sensor4Stat == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER2_ID);
		priv_sensor4Stat = SENSOR_INPUT_IDLE;
		priv_sensor4Value = 0;
	}
	// disable pin detection anyway
	EXTI_DeInit();
	/*write distance result in cm to DL*/
	distance = priv_sensor4Value / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor4, &distance);
}
