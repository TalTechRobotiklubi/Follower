#ifndef _RAMP_H_
#define _RAMP_H_

#include "stm32f10x.h"

typedef struct
{
	float*		pfRampIn;
	float*		pfRampStepUp;
	float*		pfRampStepDown;
	float*		pfRampSlopeUp;
	float*		pfRampSlopeDown;
	uint32_t*	pu32RampTimelevel;
	
	uint8_t		u8RampStatusWord;
	float		fRampOut;
}T_RAMP;

/*****************************************************************************************************************************
* RAMP module cyclic function. To be executed at 2ms intervals.                                                               *
*****************************************************************************************************************************/
void vRamp(T_RAMP* tRamp);
#endif //_RAMP_H_
