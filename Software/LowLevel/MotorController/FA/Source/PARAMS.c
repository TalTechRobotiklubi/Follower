#include "PARAMS.h"
#include "QEI.h"
#include "MODULATION.h"
#include "ANALOG.h"
#include "PID.h"
#include "CAN.h"
#include "RAMP.h"

#define ABS(A)	((A)<(0) ? (-A) : (A))			//!< Calculateas absolute value of a value
float		g_fTestReference;
uint8_t		g_u8UniqueID;
uint8_t		g_u8ControllerID;
//----------------------------------------------------------------------------------------------------------------------------
//Actual values
//----------------------------------------------------------------------------------------------------------------------------
float		g_fSpeedActFilt;           //Output from QEI_LP_FILT module
float		g_fSetpoint;               //Output from MOTOR_CONTROL module
float		g_fPwmDutyCycle;           //Output from MOTOR_CONTROL module
float		g_fCurrent;                //Output from CURR_LP_FILT module
float		g_fTemperature;            //Output from ANALOG module
uint8_t		g_u8MotorDirectionAct;     //Output from QEI module
uint8_t		g_u8StatusWord;            //Output from MOTOR_CONTROL module
//----------------------------------------------------------------------------------------------------------------------------
//Faults
//----------------------------------------------------------------------------------------------------------------------------
uint8_t		g_u8ActiveFault;              //Current active fault
uint8_t		g_u8PreviousFault1;           //
uint8_t		g_u8PreviousFault2;         
uint8_t		g_u8PreviousFault3;
uint8_t		g_u8PreviousFault4;
uint8_t		g_u8PreviousFault5;
//----------------------------------------------------------------------------------------------------------------------------
//PID
//----------------------------------------------------------------------------------------------------------------------------
float		g_fKf;
float		g_fKp;
float		g_fKi;
float		g_fKd;
float		g_fMinimumError;
uint32_t	g_u32PidTimelevel;
float		g_fOutputLimit;
uint8_t		g_u8PidStatusWord;
float		g_fPidOutput;
//----------------------------------------------------------------------------------------------------------------------------
//Ramp
//----------------------------------------------------------------------------------------------------------------------------
float		g_fRampStepUp;
float		g_fRampStepDown;
float		g_fRampSlopeUp;
float		g_fRampSlopeDown;
uint32_t	g_u32RampTimelevel;
uint8_t		g_u8RampStatusWord;
float		g_fRampOut;
//----------------------------------------------------------------------------------------------------------------------------
//Limits
//----------------------------------------------------------------------------------------------------------------------------
float		g_fTemperatureHighLimit;
float		g_fCurrentHighLimit;
float		g_fTemperatureCoefficient;
float		g_fCurrentCoefficient;
uint32_t	g_u32AnalogTimelevel;
uint8_t		g_u8AnalogStatusWord;

//----------------------------------------------------------------------------------------------------------------------------
//ANALOG
//----------------------------------------------------------------------------------------------------------------------------
float		g_fTemperature;
float		g_fCurrent;

//----------------------------------------------------------------------------------------------------------------------------
//Current low-pass filter
//----------------------------------------------------------------------------------------------------------------------------
float		g_fCurrentFiltTimeConstant;
uint32_t	g_u32CurrentFiltTimelevel;
//----------------------------------------------------------------------------------------------------------------------------
//Enocder interface 
//----------------------------------------------------------------------------------------------------------------------------
uint32_t	g_u32EncoderCoefficient;      //Input to QEI module
uint32_t	g_u32ZeroSpeed;               //Input to QEI module
uint8_t     g_u8EncoderIdRun;             //Input to QEI module
uint32_t	g_u32SpeedFiltTimeConstant;  //Input to QEI_LP_FILT module
uint32_t	g_u32SpeedFiltTimelevel;      //Input to QEI_LP_FILT module
float		g_fEncoderSpeedOut;        //Output from QEI module
float       g_fEncoderPhase;           //Phase offset in degrees
float       g_fEncoderDutyCycle;       //Duty cycle of encoder signal in percents
//----------------------------------------------------------------------------------------------------------------------------
//UART
//----------------------------------------------------------------------------------------------------------------------------
float		g_fUartSpeedReference;     //Output from UART module
uint8_t		g_u8UartStatusWord;           //Output from UART module
//----------------------------------------------------------------------------------------------------------------------------
//MODULATION
//----------------------------------------------------------------------------------------------------------------------------
float       g_fPwmDutyCycle;           //Input to MODULATION module
uint8_t     g_u8MotorControlDirection;    //Input to MODULATION module
uint8_t     g_u8ModulationStatusWord;     //Output from MODULATION module
//----------------------------------------------------------------------------------------------------------------------------
//CAN
//----------------------------------------------------------------------------------------------------------------------------
float		g_fCanSpeedReference;      //Output from CAN module
uint8_t		g_u8CanStatusWord;            //Output from CAN module
//----------------------------------------------------------------------------------------------------------------------------
//PID low pass filter
//----------------------------------------------------------------------------------------------------------------------------
float		g_fPidFiltTimeConstant;    //Input to PID_LP_FILT module
uint32_t	g_u32PidFiltTimelevel;        //Input to PID_LP_FILT module
float		g_fPidFiltOutput;          //Output from PID_LP_FILT module



//----------------------------------------------------------------------------------------------------------------------------
//Global structures
//----------------------------------------------------------------------------------------------------------------------------
T_MODULATION        tModulation;
T_QEI				tQei;
T_ANALOG			tAnalog;
T_PID				tPid;
T_CAN				tCan;
T_RAMP				tRamp;
//----------------------------------------------------------------------------------------------------------------------------
//  Paramter table
//----------------------------------------------------------------------------------------------------------------------------
uint32_t* pu32ParameterTable[35][4] = 
{
		// description						type			address			?
{(uint32_t*)&g_u8UniqueID, 				(uint32_t*)U8,	(uint32_t*)136, (uint32_t*)238},
{(uint32_t*)&g_u8ControllerID, 			(uint32_t*)U8, 	(uint32_t*)140, (uint32_t*)230},
{(uint32_t*)&g_u8ActiveFault, 			(uint32_t*)U8,	(uint32_t*)4, 	(uint32_t*)255},
{(uint32_t*)&g_u8PreviousFault1, 		(uint32_t*)U8, 	(uint32_t*)8, 	(uint32_t*)255},
{(uint32_t*)&g_u8PreviousFault2, 		(uint32_t*)U8, 	(uint32_t*)12, 	(uint32_t*)255},
{(uint32_t*)&g_u8PreviousFault3, 		(uint32_t*)U8, 	(uint32_t*)16, 	(uint32_t*)255},
{(uint32_t*)&g_u8PreviousFault4, 		(uint32_t*)U8, 	(uint32_t*)20, 	(uint32_t*)255},
{(uint32_t*)&g_u8PreviousFault5, 		(uint32_t*)U8, 	(uint32_t*)24, 	(uint32_t*)255},
{(uint32_t*)&g_fKf, 					(uint32_t*)F, 	(uint32_t*)28, 	(uint32_t*)130},
{(uint32_t*)&g_fKp, 					(uint32_t*)F, 	(uint32_t*)32, 	(uint32_t*)134},
{(uint32_t*)&g_fKi, 					(uint32_t*)F, 	(uint32_t*)36, 	(uint32_t*)138},
{(uint32_t*)&g_fKd, 					(uint32_t*)F, 	(uint32_t*)40, 	(uint32_t*)142},
{(uint32_t*)&g_fMinimumError, 			(uint32_t*)F, 	(uint32_t*)44, 	(uint32_t*)146},
{(uint32_t*)&g_u32PidTimelevel, 		(uint32_t*)U32, (uint32_t*)48, 	(uint32_t*)150},
{(uint32_t*)&g_fOutputLimit, 			(uint32_t*)F, 	(uint32_t*)52, 	(uint32_t*)154},
{(uint32_t*)&g_fPidFiltTimeConstant, 	(uint32_t*)F, 	(uint32_t*)56, 	(uint32_t*)158},
{(uint32_t*)&g_u32PidFiltTimelevel, 	(uint32_t*)U32, (uint32_t*)60, 	(uint32_t*)162},
{(uint32_t*)&g_fRampStepUp, 			(uint32_t*)F, 	(uint32_t*)64, 	(uint32_t*)166},
{(uint32_t*)&g_fRampStepDown, 			(uint32_t*)F, 	(uint32_t*)68, 	(uint32_t*)170},
{(uint32_t*)&g_fRampSlopeUp, 			(uint32_t*)F, 	(uint32_t*)72, 	(uint32_t*)174},
{(uint32_t*)&g_fRampSlopeDown, 			(uint32_t*)F, 	(uint32_t*)76, 	(uint32_t*)178},
{(uint32_t*)&g_u32RampTimelevel, 		(uint32_t*)U32, (uint32_t*)80, 	(uint32_t*)182},
{(uint32_t*)&g_fTemperatureHighLimit, 	(uint32_t*)F, 	(uint32_t*)84, 	(uint32_t*)186},
{(uint32_t*)&g_fCurrentHighLimit, 		(uint32_t*)F, 	(uint32_t*)88, 	(uint32_t*)190},
{(uint32_t*)&g_fTemperatureCoefficient, (uint32_t*)F, 	(uint32_t*)92, 	(uint32_t*)194},
{(uint32_t*)&g_fCurrentCoefficient, 	(uint32_t*)F, 	(uint32_t*)96, 	(uint32_t*)198},
{(uint32_t*)&g_u32AnalogTimelevel, 		(uint32_t*)U32, (uint32_t*)100, (uint32_t*)202},
{(uint32_t*)&g_fCurrentFiltTimeConstant,(uint32_t*)F, 	(uint32_t*)104, (uint32_t*)206},
{(uint32_t*)&g_u32CurrentFiltTimelevel, (uint32_t*)U32, (uint32_t*)108, (uint32_t*)210},
{(uint32_t*)&g_u32EncoderCoefficient, 	(uint32_t*)U32, (uint32_t*)112, (uint32_t*)214},
{(uint32_t*)&g_u32ZeroSpeed, 			(uint32_t*)U32, (uint32_t*)116, (uint32_t*)218},
{(uint32_t*)&g_u32SpeedFiltTimeConstant,(uint32_t*)U32, (uint32_t*)120, (uint32_t*)222},
{(uint32_t*)&g_u32SpeedFiltTimelevel, 	(uint32_t*)U32, (uint32_t*)124, (uint32_t*)226},
{(uint32_t*)&g_fEncoderPhase, 			(uint32_t*)F, 	(uint32_t*)128, (uint32_t*)230},
{(uint32_t*)&g_fEncoderDutyCycle, 		(uint32_t*)F, 	(uint32_t*)132, (uint32_t*)234}
};

//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------
void vParametersInit(void)
{
  int i=0;
  uint16_t u16Data = 0;
  uint32_t u32Data = 0;
  for(i=0;i<35;i++)
  {
    u16Data = 0;
	u32Data = 0;
	if((uint32_t)pu32ParameterTable[i][1] == U8)
	{
	  read((uint32_t)pu32ParameterTable[i][2],&u16Data);
	  vParameterWrite((uint32_t)pu32ParameterTable[i][0],(uint8_t)u16Data,U8);
	}
	if((uint32_t)pu32ParameterTable[i][1] == U32
	   || (uint32_t)pu32ParameterTable[i][1] == F)
	{
	  read(((uint32_t)pu32ParameterTable[i][2]),&u16Data);
	  u32Data = u16Data;
          u32Data = u32Data << 16;
	  read(((uint32_t)pu32ParameterTable[i][2])+2,&u16Data);
	  u32Data |= u16Data;
	  vParameterWrite((uint32_t)pu32ParameterTable[i][0],(uint32_t)u32Data,U32);
	}
	
  }
}
//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------
void vParameterRead(uint32_t u32Address,uint32_t* pu32Value,uint8_t u8Type)
{
	uint32_t*	pu32Pointer;					//Pointer to memory address
	pu32Pointer = (uint32_t*)u32Address;		//Assign value to pointer (e.q memory address to be read)
	if(u8Type == U8)
	{
		*pu32Value = (uint8_t)*pu32Pointer;		//Read value from memory address specified by u32Address
	}
	else
	{
		*pu32Value = (uint32_t)*pu32Pointer;
	}
}
//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------
void vParameterFlashWrite(uint32_t u32Address,uint32_t u32Value,uint8_t u8Type)
{
  if(u8Type == U8)
  {
	write(u32Address,(uint8_t)u32Value);
	int i=0;
	for(i=0; i<35;i++)
	{
	  if((uint32_t)pu32ParameterTable[i][2] == u32Address)
	  {
		vParameterWrite((uint32_t)pu32ParameterTable[i][0],(uint8_t)u32Value,U8);
	  }
	}
  }
 else if(u8Type == U32 || u8Type == F)
  {
        uint32_t val_bot = u32Value & 0x0000FFFF;
        uint32_t val_top = u32Value & 0xFFFF0000;
        val_top = val_top >> 16;
	write(u32Address,(uint16_t)val_top);
	write(u32Address+2,(uint16_t)val_bot);
	int i=0;
	for(i=0; i<35;i++)
	{
	  if((uint32_t)pu32ParameterTable[i][2] == u32Address)
	  {
		vParameterWrite((uint32_t)pu32ParameterTable[i][0],u32Value,U32);
	  }
	}
  }
}
//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------
void vParameterWrite(uint32_t u32Address,uint32_t u32Value,uint8_t u8Type)
{
  	uint32_t*	pu32Pointer;					//Pointer to memory address
	pu32Pointer = (uint32_t*)u32Address;		//Assign value to pointer (e.q memory address to be read)
	
  	if(u8Type == U8)
	{
	 *pu32Pointer = (uint8_t)u32Value;
	}
	if(u8Type == U16)
	{
	  *pu32Pointer = (uint16_t)u32Value;
	}
	if(u8Type == U32 || u8Type == F)
	{
	 *pu32Pointer = (uint32_t)u32Value;
	}
	
}
//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------
void vParameterWriteDefault(uint32_t u32FlashAddr,uint8_t u8ValueLen,uint32_t u32Value)
{
	//Write parameter to default
}
//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------
void vParametersConnectionsInit(void)
{
  	vParametersInit();

    
	//--------------------------------------------------------------------------------------------------------------------
	//  CAN MODULE CONNECTIONS
	//--------------------------------------------------------------------------------------------------------------------
	tCan.pfMotorSpeed = &g_fEncoderSpeedOut;
	tCan.pfMotorCurrent = &g_fTemperature;
	tCan.pfMotorTemp = &g_fCurrent;
	tCan.pu8Fault = &g_u8ActiveFault;
	tCan.pu8MotorDirection = &g_u8MotorDirectionAct;
	//--------------------------------------------------------------------------------------------------------------------
	//  RAMP MODULE CONNECTIONS
	//--------------------------------------------------------------------------------------------------------------------
	tRamp.pfRampIn = &g_fCanSpeedReference;
	tRamp.pfRampStepUp = &g_fRampStepUp;
	tRamp.pfRampStepDown = &g_fRampStepDown;
	tRamp.pfRampSlopeUp = &g_fRampSlopeUp;
	tRamp.pfRampSlopeDown = &g_fRampSlopeDown;
	tRamp.pu32RampTimelevel = &g_u32RampTimelevel;
	
	//--------------------------------------------------------------------------------------------------------------------
	//  MODULATION MODULE CONNECTIONS
	//--------------------------------------------------------------------------------------------------------------------
	tModulation.pfPwmDutyCycle = &g_fPwmDutyCycle;
	tModulation.pu8Direction      = &g_u8MotorControlDirection;
	
	//--------------------------------------------------------------------------------------------------------------------
	//  QEI MODULE CONNECTIONS
	//--------------------------------------------------------------------------------------------------------------------
	tQei.fEncoderDuty = &g_fEncoderDutyCycle;
	
	
    // 
	//PID connections
	//
	tPid.pfSetpoint      = &g_fSetpoint;
	tPid.pfSpeedAct      = &g_fEncoderSpeedOut;			//Actual velocity in RPM
	tPid.pfKf 			= &g_fKf;					//Reference feed forward constant
	tPid.pfKp 			= &g_fKp;					//Proportional gain
	tPid.pfKi 			= &g_fKi;					//Integral gain
	tPid.pfKd 			= &g_fKd;					//Derivative gain
	tPid.pfMinErr 		= &g_fMinimumError;			//Minimum tolerable error
	tPid.pu32Timelevel      = &g_u32PidTimelevel;			//Function execution interval in milliseconds
	tPid.pfOutLim 		= &g_fOutputLimit;			//Output and integral term limit
	

	
}
//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------
void MODULATION_TASK(void)
{
  vModulation(&tModulation);
}

//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------
void QEI_TASK(void)
{
  vQei(&tQei);
  g_fEncoderSpeedOut = tQei.fMotorSpeedAct;
  if(tQei.fMotorSpeedAct > 0)
  {
	  if((tQei.fMotorSpeedAct) > 0.95 * g_fSetpoint ||
		 ((tQei.fMotorSpeedAct) < 1.05 * g_fSetpoint))
	  {
		  GPIOB->BSRR = GPIO_Pin_3;
	  }
	  else
		  GPIOB->BRR = GPIO_Pin_3;
  }
  
  g_u8MotorDirectionAct = tQei.u8MotorDir;
}

//
//
//
void ANALOG_TASK(void)
{
  vAnalog(&tAnalog);
  g_fTemperature = tAnalog.fTemperatureOut;
  g_fCurrent = tAnalog.fCurrentOut;
}

//
//
//
void PID_TASK(void)
{
  vPid(&tPid);
 
    g_fPwmDutyCycle = ABS(tPid.fPidOut);

  if(tPid.fPidOut > 0)
    g_u8MotorControlDirection = 0;
  else
     g_u8MotorControlDirection = 1;
}

//
//
//
void CAN_TASK(void)
{
  vCan(&tCan);
  g_fCanSpeedReference = tCan.fCanSpeedReference;
}

//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------
void RAMP_TASK(void)
{
  vRamp(&tRamp);
  g_fSetpoint = tRamp.fRampOut;
}
