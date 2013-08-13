#pragma once
#ifndef _ANALOG_H_
#define _ANALOG_H_

#include "stm32f10x.h"

typedef struct
{
	float*	pfTemperatureHighLimit;
	float*	pfCurrentHighLimit;
	
	float	fTemperatureOut;
	float	fCurrentOut;
	uint8_t u8AnalogStatus;
}T_ANALOG;

void vAnalogHardwareInit(void);
void vAnalog(T_ANALOG*	ptInput);
uint16_t u16ReadAdcChannel(uint8_t u8Channel);
#endif //_ANALOG_H_
