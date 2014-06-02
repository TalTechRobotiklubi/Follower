#include "Sensor.h"
#include "stm32f4xx.h"
#include "DataLayer.h"
#include "GPIO.h"
#include "Timer.h"

typedef enum
{
	SENSOR_INPUT_IDLE,
	SENSOR_INPUT_RISED,
	SENSOR_INPUT_FALLED,
	SENSOR_INPUT_ERROR
}
SensorInputStatus_Typedef;


/*private variables*/
EXTI_InitTypeDef   			EXTI_InitStructure;

SensorInputStatus_Typedef 	priv_sensorStat[8];
uint16_t 					priv_sensorValue[8];
/*SensorInputStatus_Typedef 	priv_sensor2Stat = SENSOR_INPUT_IDLE;
SensorInputStatus_Typedef 	priv_sensor3Stat = SENSOR_INPUT_IDLE;
SensorInputStatus_Typedef 	priv_sensor4Stat = SENSOR_INPUT_IDLE;
uint16_t 					priv_sensorValue[0] = 0;
uint16_t 					priv_sensorValue[1] = 0;
uint16_t 					priv_sensorValue[2] = 0;
uint16_t 					priv_sensorValue[3] = 0;*/

/*private function declarations*/
void initDistanceSensor(GPIO_IdDef io);
//void enableSensorInterrupt(GPIO_IdDef io);
void disableSensorInterrupt(GPIO_IdDef io);

/*function definitions*/
void Sensor_init(void)
{
	/* init variables*/
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		priv_sensorStat[i] = SENSOR_INPUT_IDLE;
		priv_sensorValue[i] = 0;
	}
	/*URF sensor initialization done when starting measurement*/
}

//
void initDistanceSensor(GPIO_IdDef io)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOx clock */
	RCC_AHB1PeriphClockCmd(GPIO_table[io].clk, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure pin as floating input */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_table[io].pin;
	GPIO_Init(GPIO_table[io].port, &GPIO_InitStructure);

	/* Connect EXTIxx Line to pin */
	SYSCFG_EXTILineConfig(GPIO_table[io].portSource, GPIO_table[io].pinSource);

	/* Configure EXTIxx line */
	EXTI_InitStructure.EXTI_Line = GPIO_table[io].exti;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = GPIO_table[io].interrupt;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * disable Exti interrupt for sensor IO
 */
void disableSensorInterrupt(GPIO_IdDef io)
{
	EXTI_InitStructure.EXTI_Line = GPIO_table[io].exti;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/**
* @brief  This function handles External line 15 - 10 interrupt request for URF1 echo pin PC14.
* @param  None
* @retval None
*/
void EXTI15_10_IRQHandler(void)
{
	/*handle URF1 echo interrupts*/
	if(EXTI_GetITStatus(GPIO_table[URF1_ECHO].exti) != RESET)
	{

		if (GPIO_inputValue(URF1_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER2_ID);
			priv_sensorStat[0] = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensorStat[0] == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER2_ID);
				// read measurement result
				priv_sensorValue[0] = Timer_getTimerValue(TIMER2_ID);
				/*disable pin detection*/
				//EXTI_DeInit();
				disableSensorInterrupt(URF1_ECHO);

				priv_sensorStat[0] = SENSOR_INPUT_FALLED;
			}
			else
			{
				priv_sensorStat[0] = SENSOR_INPUT_ERROR;
			}

		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF1_ECHO].exti);
	}
}

/**
* @brief  This function handles External lines 9 to 5 interrupt request for URF3 echo pin PE6.
* @param  None
* @retval None
*/
void EXTI9_5_IRQHandler(void)
{
	/*handle URF2 echo interrupts*/
	if(EXTI_GetITStatus(GPIO_table[URF2_ECHO].exti) != RESET)
	{

		if (GPIO_inputValue(URF2_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER2_ID);
			priv_sensorStat[1] = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensorStat[1] == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER2_ID);
				// read measurement result
				priv_sensorValue[1] = Timer_getTimerValue(TIMER2_ID);
				/*disable pin detection*/
				//EXTI_DeInit();
				disableSensorInterrupt(URF2_ECHO);
			}
			priv_sensorStat[1] = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF2_ECHO].exti);
	}

	/*handle URF5 echo interrupts*/
	if(EXTI_GetITStatus(GPIO_table[URF5_ECHO].exti) != RESET)
	{

		if (GPIO_inputValue(URF5_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER3_ID);
			priv_sensorStat[4] = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensorStat[4] == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER3_ID);
				// read measurement result
				priv_sensorValue[4] = Timer_getTimerValue(TIMER3_ID);
				/*disable pin detection*/
				disableSensorInterrupt(URF5_ECHO);
			}
			priv_sensorStat[4] = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF5_ECHO].exti);
	}

	/*handle URF7 echo interrupts*/
	if(EXTI_GetITStatus(GPIO_table[URF7_ECHO].exti) != RESET)
	{

		if (GPIO_inputValue(URF7_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER3_ID);
			priv_sensorStat[6] = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensorStat[6] == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER3_ID);
				// read measurement result
				priv_sensorValue[6] = Timer_getTimerValue(TIMER3_ID);
				/*disable pin detection*/
				disableSensorInterrupt(URF7_ECHO);
			}
			priv_sensorStat[6] = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF7_ECHO].exti);
	}
	/*handle URF8 echo interrupts*/
	if(EXTI_GetITStatus(GPIO_table[URF8_ECHO].exti) != RESET)
	{

		if (GPIO_inputValue(URF8_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER3_ID);
			priv_sensorStat[7] = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensorStat[7] == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER3_ID);
				// read measurement result
				priv_sensorValue[7] = Timer_getTimerValue(TIMER3_ID);
				/*disable pin detection*/
				disableSensorInterrupt(URF8_ECHO);
			}
			priv_sensorStat[7] = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF8_ECHO].exti);
	}
}


/**
* @brief  This function handles External line 4 interrupt request for URF3 echo pin PE4.
* @param  None
* @retval None
*/
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(GPIO_table[URF3_ECHO].exti) != RESET)
	{

		if (GPIO_inputValue(URF3_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER2_ID);
			priv_sensorStat[2] = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensorStat[2] == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER2_ID);
				// read measurement result
				priv_sensorValue[2] = Timer_getTimerValue(TIMER2_ID);
				/*disable pin detection*/
				//EXTI_DeInit();
				disableSensorInterrupt(URF3_ECHO);
			}
			priv_sensorStat[2] = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF3_ECHO].exti);
	}
}

/**
* @brief  This function handles External line 2 interrupt request for URF4 echo pin PE2.
* @param  None
* @retval None
*/
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(GPIO_table[URF4_ECHO].exti) != RESET)
    {

		if (GPIO_inputValue(URF4_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER2_ID);
			priv_sensorStat[3] = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensorStat[3] == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER2_ID);
				// read measurement result
				priv_sensorValue[3] = Timer_getTimerValue(TIMER2_ID);
				/*disable pin detection*/
				//EXTI_DeInit();
				disableSensorInterrupt(URF4_ECHO);
			}
			priv_sensorStat[3] = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF4_ECHO].exti);
    }

}

/**
* @brief  This function handles External line 1 interrupt request for URF6 echo pin PB1.
* @param  None
* @retval None
*/
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(GPIO_table[URF6_ECHO].exti) != RESET)
	{

		if (GPIO_inputValue(URF6_ECHO) == INPUT_ON)
		{
			// rising edge, start to measure
			Timer_startTimer(TIMER3_ID);
			priv_sensorStat[5] = SENSOR_INPUT_RISED;
		}
		else
		{
			// falling edge, check that rising was previously
			if (priv_sensorStat[5] == SENSOR_INPUT_RISED)
			{
				/*stop timer*/
				Timer_stopTimer(TIMER3_ID);
				// read measurement result
				priv_sensorValue[5] = Timer_getTimerValue(TIMER3_ID);
				/*disable pin detection*/
				disableSensorInterrupt(URF6_ECHO);
			}
			priv_sensorStat[5] = SENSOR_INPUT_FALLED;
		}

		/* Clear the  EXTI line pending bit */
		EXTI_ClearITPendingBit(GPIO_table[URF6_ECHO].exti);
	}
}


void Sensor_TASK_startMeasurement1(void)
{
	priv_sensorStat[0] = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF1_TRIG);
	Timer_startTimer(TIMER2_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER2_ID) < 10);

	Timer_stopTimer(TIMER2_ID);
	// set trigger output low
	GPIO_outputOff(URF1_TRIG);
	// enable input pin for sensor 1
	initDistanceSensor(URF1_ECHO);
}

void Sensor_TASK_readDistance1(void)
{
	uint16_t distance = 0;

	// disable pin detection anyway
	disableSensorInterrupt(URF1_ECHO);
	Timer_stopTimer(TIMER2_ID);

	switch(priv_sensorStat[0])
	{
	case SENSOR_INPUT_IDLE:  // no sensor connected
		distance = 0;
		break;
	case SENSOR_INPUT_FALLED: //ok, write distance result in cm to DL
		distance = priv_sensorValue[0] / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
		if (distance > 255)
			distance = 255;
		break;
	case SENSOR_INPUT_RISED: // error in measure, so max value
	case SENSOR_INPUT_ERROR:
		distance = 255;
		break;
	}
	DL_setData(DLParamDistanceSensor1, &distance);
}

void Sensor_TASK_startMeasurement2(void)
{
	priv_sensorStat[1] = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF2_TRIG);
	Timer_startTimer(TIMER2_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER2_ID) < 10);
	Timer_stopTimer(TIMER2_ID);
	// set trigger output low
	GPIO_outputOff(URF2_TRIG);
	// enable input pin for sensor 2
	initDistanceSensor(URF2_ECHO);
}

void Sensor_TASK_readDistance2(void)
{
	uint16_t distance = 0;
	// measurement has taken too long, set max distance
	if (priv_sensorStat[1] == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER2_ID);
		priv_sensorStat[1] = SENSOR_INPUT_IDLE;
		priv_sensorValue[1] = 255;
	}
	// disable pin detection anyway
	disableSensorInterrupt(URF2_ECHO);

	/*write distance result in cm to DL*/
	distance = priv_sensorValue[1] / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor2, &distance);
}

void Sensor_TASK_startMeasurement3(void)
{
	priv_sensorStat[2] = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF3_TRIG);
	Timer_startTimer(TIMER2_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER2_ID) < 10);
	Timer_stopTimer(TIMER2_ID);
	// set trigger output low
	GPIO_outputOff(URF3_TRIG);
	// enable input pin for sensor 3
	initDistanceSensor(URF3_ECHO);
}

void Sensor_TASK_readDistance3(void)
{
	uint16_t distance = 0;
	// measurement has taken too long, set max distance
	if (priv_sensorStat[2] == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER2_ID);
		priv_sensorStat[2] = SENSOR_INPUT_IDLE;
		priv_sensorValue[2] = 255;
	}
	// disable pin detection anyway
	disableSensorInterrupt(URF3_ECHO);
	/*write distance result in cm to DL*/
	distance = priv_sensorValue[2] / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor3, &distance);
}

void Sensor_TASK_startMeasurement4(void)
{
	priv_sensorStat[3] = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF4_TRIG);
	Timer_startTimer(TIMER2_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER2_ID) < 10);
	Timer_stopTimer(TIMER2_ID);
	// set trigger output low
	GPIO_outputOff(URF4_TRIG);
	// enable input pin for sensor 4
	initDistanceSensor(URF4_ECHO);
}

void Sensor_TASK_readDistance4(void)
{
	uint16_t distance = 0;
	// measurement has taken too long, set max distance
	if (priv_sensorStat[3] == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER2_ID);
		priv_sensorStat[3] = SENSOR_INPUT_IDLE;
		priv_sensorValue[3] = 255;
	}
	// disable pin detection anyway
	disableSensorInterrupt(URF4_ECHO);
	/*write distance result in cm to DL*/
	distance = priv_sensorValue[3] / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor4, &distance);
}


void Sensor_TASK_startMeasurement5(void)
{
	priv_sensorStat[4] = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF5_TRIG);
	Timer_startTimer(TIMER3_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER3_ID) < 10);

	Timer_stopTimer(TIMER3_ID);
	// set trigger output low
	GPIO_outputOff(URF5_TRIG);
	// enable input pin for sensor 5
	initDistanceSensor(URF5_ECHO);
}

void Sensor_TASK_readDistance5(void)
{
	uint16_t distance = 0;
	// measurement has taken too long, set max distance
	if (priv_sensorStat[4] == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER3_ID);
		priv_sensorStat[4] = SENSOR_INPUT_IDLE;
		priv_sensorValue[4] = 255;
	}
	// disable pin detection anyway
	disableSensorInterrupt(URF5_ECHO);
	/*write distance result in cm to DL*/
	distance = priv_sensorValue[4] / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor5, &distance);
}

void Sensor_TASK_startMeasurement6(void)
{
	priv_sensorStat[5] = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF6_TRIG);
	Timer_startTimer(TIMER3_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER3_ID) < 10);
	Timer_stopTimer(TIMER3_ID);
	// set trigger output low
	GPIO_outputOff(URF6_TRIG);
	// enable input pin for sensor 6
	initDistanceSensor(URF6_ECHO);
}

void Sensor_TASK_readDistance6(void)
{
	uint16_t distance = 0;
	// measurement has taken too long, set max distance
	if (priv_sensorStat[5] == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER3_ID);
		priv_sensorStat[5] = SENSOR_INPUT_IDLE;
		priv_sensorValue[5] = 255;
	}
	// disable pin detection anyway
	disableSensorInterrupt(URF6_ECHO);
	/*write distance result in cm to DL*/
	distance = priv_sensorValue[5] / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor6, &distance);
}

void Sensor_TASK_startMeasurement7(void)
{
	priv_sensorStat[6] = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF7_TRIG);
	Timer_startTimer(TIMER3_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER3_ID) < 10);
	Timer_stopTimer(TIMER3_ID);
	// set trigger output low
	GPIO_outputOff(URF7_TRIG);
	// enable input pin for sensor 7
	initDistanceSensor(URF7_ECHO);
}

void Sensor_TASK_readDistance7(void)
{
	uint16_t distance = 0;
	// measurement has taken too long, set max distance
	if (priv_sensorStat[6] == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER3_ID);
		priv_sensorStat[6] = SENSOR_INPUT_IDLE;
		priv_sensorValue[6] = 255;
	}
	// disable pin detection anyway
	disableSensorInterrupt(URF7_ECHO);
	/*write distance result in cm to DL*/
	distance = priv_sensorValue[6] / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor7, &distance);
}

void Sensor_TASK_startMeasurement8(void)
{
	priv_sensorStat[7] = SENSOR_INPUT_IDLE;
	// set trigger output high
	GPIO_outputOn(URF8_TRIG);
	Timer_startTimer(TIMER3_ID);
	// keep output high for 20 us
	while (Timer_getTimerValue(TIMER3_ID) < 10);
	Timer_stopTimer(TIMER3_ID);
	// set trigger output low
	GPIO_outputOff(URF8_TRIG);
	// enable input pin for sensor 4
	initDistanceSensor(URF8_ECHO);
}

void Sensor_TASK_readDistance8(void)
{
	uint16_t distance = 0;
	// measurement has taken too long, set max distance
	if (priv_sensorStat[7] == SENSOR_INPUT_RISED)
	{
		Timer_stopTimer(TIMER3_ID);
		priv_sensorStat[7] = SENSOR_INPUT_IDLE;
		priv_sensorValue[7] = 255;
	}
	// disable pin detection anyway
	disableSensorInterrupt(URF8_ECHO);
	/*write distance result in cm to DL*/
	distance = priv_sensorValue[7] / 58;  // 147 us equals 1 inch -> 147 / 2,54 = 57,8
	if (distance > 255)
		distance = 255;
	DL_setData(DLParamDistanceSensor8, &distance);
}
