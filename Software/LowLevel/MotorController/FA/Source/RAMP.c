//----------------------------------------------------------------------------------------------------------------------------
// INCLUDE FILES
//----------------------------------------------------------------------------------------------------------------------------
#include "RAMP.h"

//----------------------------------------------------------------------------------------------------------------------------
//	LOCAL DEFINITIONS AND CONSTANTS
//----------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------
//	LOCAL VARIABLES
//----------------------------------------------------------------------------------------------------------------------------
float fCalcSlopeUp = 0;
float fCalcSlopeDown = 0;
float fVPOS = 0;
//----------------------------------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES
//----------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------
void vRamp(T_RAMP* tRamp)
{
	//Calculate slope
	/*iq16CalcSlopeUp = _IQ16div(_IQ16abs(*tRamp->piq16RampIn),*tRamp->piq16RampSlopeUp);
	iq16CalcSlopeDown = _IQ16div(_IQ16abs(*tRamp->piq16RampIn),*tRamp->piq16RampSlopeDown);
	if(*tRamp->piq16RampIn != 0)
	{
		if((*tRamp->piq16RampIn > tRamp->iq16RampOut))
		{
			if(iq16CalcSlopeUp > 1)
			{
				iq16VPOS += iq16CalcSlopeUp;
				if(*tRamp->piq16RampIn > (iq16VPOS + *tRamp->piq16RampStepUp))
				{
					tRamp->iq16RampOut = iq16VPOS + *tRamp->piq16RampStepUp;
				}
				else
				{
					tRamp->iq16RampOut = *tRamp->piq16RampIn;
				}
			}
			else
			{
				tRamp->iq16RampOut = *tRamp->piq16RampIn;
			}
		}
		if(*tRamp->piq16RampIn < tRamp->iq16RampOut)
		{
			if(iq16CalcSlopeDown > 0)
			{
				iq16VPOS -= iq16CalcSlopeDown;
				if(*tRamp->piq16RampIn < (iq16VPOS - *tRamp->piq16RampStepDown))
				{
					tRamp->iq16RampOut = iq16VPOS - *tRamp->piq16RampStepDown;
				}
				else
				{
					tRamp->iq16RampOut = *tRamp->piq16RampIn;
				}
			}
			else
				{
					tRamp->iq16RampOut = *tRamp->piq16RampIn;
				}
		}
	}
	else
	{
		tRamp->iq16RampOut = *tRamp->piq16RampIn;
		iq16VPOS = 0;
	}*/
    
    tRamp->fRampOut = *tRamp->pfRampIn;
}
	
