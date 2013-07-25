#ifndef _QEI_H_
#define _QEI_H_

//----------------------------------------------------------------------------------------------------------------------------
// INCLUDE FILES
//----------------------------------------------------------------------------------------------------------------------------
#include "stm32f10x.h"

extern uint16_t u16Ticks;
//----------------------------------------------------------------------------------------------------------------------------
// DATA STRUCTURES
//----------------------------------------------------------------------------------------------------------------------------
typedef struct 
{
  uint32_t*		pu32EncoderCoeficcient;				//Encoder coeficcient
  uint32_t*		pu32ZeroSpeed;						//Timer value at which motor speed is considered zero
  float*        fEncoderDuty;					//Enoder signal duty cycle
  
  float			fMotorSpeedAct;					//Actual motor speed (absolute value)
  uint8_t		u8MotorDir;							//Motor direction, where 1 is CCW rotation and 0 is CW rotation
  uint8_t       u8QeiStatusWord;					//Status word of QEI module
}T_QEI;

//----------------------------------------------------------------------------------------------------------------------------
// GLOBAL FUNCTION DECLARATIONS
//----------------------------------------------------------------------------------------------------------------------------

/*****************************************************************************************************************************
* Configure all the necessary hardware peripherals for quadrature encoder decoding and speed calculation.                    *
*****************************************************************************************************************************/
void vQeiHardwareInit(void);

/*****************************************************************************************************************************
* QEI module cyclic function. To be executed at 2ms intervals.                                                               *
*****************************************************************************************************************************/
void vQei(T_QEI* tQei);


#endif /*_QEI_H_*/
