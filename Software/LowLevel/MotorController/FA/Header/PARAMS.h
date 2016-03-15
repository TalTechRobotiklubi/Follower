#pragma once
#ifndef PARAMS_H_
#define PARAMS_H_

#if 0
#include "stm32f10x.h"
#include "QEI.h"
#include "MODULATION.h"
#include "ANALOG.h"
//Includes of all blocks
/*


#include "MOTOR_CONTROL.h"
*/

#define U8 		0
#define U16		1
#define U32 	2
#define F    	3


extern T_MODULATION                    tModulation;
extern T_QEI				tQei;
extern uint8_t		g_u8ControllerID;
//Actual values
extern float		g_fSpeedActFilt;
extern float		g_fSetpoint;
extern float		g_fPwmDutyCycle;
extern float		g_fCurrent;
extern float		g_fTemperature;
extern char			g_bMotorDirection;
extern uint8_t		g_u8StatusWord;
//Faults
extern uint8_t		g_u8ActiveFault;
extern uint8_t		g_u8PreviousFault1;
extern uint8_t		g_u8PreviousFault2;
extern uint8_t		g_u8PreviousFault3;
extern uint8_t		g_u8PreviousFault4;
extern uint8_t		g_u8PreviousFault5;
//Limits
extern float		g_fTemperatureHighLimit;
extern float		g_fCurrentHighLimit;
extern float		g_fTemperatureCoefficient;
extern float		g_fCurrentCoefficient;
extern uint32_t		g_u32AnalogTimelevel;
extern uint8_t		g_u8AnalogStatusWord;
//Current low-pass filter
extern float		g_fCurrentFiltTimeConstant;
extern uint32_t		g_u32CurrentFiltTimelevel;
//Enocder interface 
//Parameters
extern uint32_t		g_u32EncoderCoefficient;
extern uint32_t		g_u32ZeroSpeed;
extern float		g_fSpeedFiltTimeConstant;
extern uint32_t		g_u32SpeedFiltTimelevel;
extern float		g_fEncoderSpeedOut;
extern float		g_fUartSpeedReference;     //Output from UART module

extern uint8_t         		g_u8MotorControlDirection;           //Input to MODULATION module
extern uint8_t				g_u8UartStatusWord;
void vParametersConnectionsInit(void);
void vParametersInit(void);
void vParameterFlashWrite(uint32_t u32Address,uint32_t u32Value,uint8_t u8Type);

extern void UART_TASK(void);
extern void MODULATION_TASK(void);
extern void QEI_TASK(void);
extern void QEI_FILT_TASK(void);
extern void ANALOG_TASK(void);
extern void PID_TASK(void);
extern void CAN_TASK(void);
extern void RAMP_TASK(void);
#endif
#endif
