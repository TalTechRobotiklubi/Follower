//----------------------------------------------------------------------------------------------------------------------------
// INCLUDE FILES
//----------------------------------------------------------------------------------------------------------------------------
#include "QEI.h"
#include "stm32f10x.h"
#include "PARAMS.h"

//----------------------------------------------------------------------------------------------------------------------------
//	LOCAL DEFINITIONS AND CONSTANTS
//----------------------------------------------------------------------------------------------------------------------------
#define ABS(A)	((A)<(0) ? (-A) : (A))			//!< Calculateas absolute value of a value
#define OLD_A 			3						//!< Old pin stte of channel A is the 4th bit
#define OLD_B 			2						//!< Old pin state of channel B is the 3rd bit
#define A				1						//!< Current state of channel A is the 2nd bit
#define B				0						//!< Current state of channel B is the 1st bit

#define ID_RUN_DONE  	0						//!< Bit indicating wheter ID run is completed or not
#define ID_RUN_ACTIVE 	1						//!< Bit indicating wheter ID run is active or not
#define CHA_FAULT		2						//!< Bit indicating wheter channel A has a fault
#define CHB_FAULT		3						//!< Bit indicating wheter channel B has a fault
//----------------------------------------------------------------------------------------------------------------------------
//	LOCAL VARIABLES
//----------------------------------------------------------------------------------------------------------------------------
uint8_t u8IDRUN						= 0;
int32_t i32Count 					= 0;
uint8_t u8QeiPinStatus 				= 0;
int8_t i8LookupTable[16] 			= {0,-1,+1,0,+1,0,0,-1,-1,0,0,+1,0,+1,-1,0};
uint8_t u8MotorDir					= 0;
uint32_t u32TimerValue 				= 0;
uint16_t u16Count 					= 0;
uint32_t u32CHA_Falling_Edge_Timer 	= 0;
uint32_t u32CHA_Rising_Edge_Timer	= 0;
uint16_t u16CHA_Rising_Edge_count 	= 0;

uint32_t u32CurrentTimer 			= 0;
float fDutyCycle					= 0;
float fCurrentVelocity				= 0;
float testVel						= 0;
int16_t i16CHACount					= 0;
int16_t i16CHBCount					= 0;
uint16_t u16ErrorTimer				= 0;
uint16_t u16CumulativeErrors		= 0;

uint16_t u16Ticks					= 0;	//Ticks to be sent to PC
//----------------------------------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES
//----------------------------------------------------------------------------------------------------------------------------
void vQeiProcessIRQ(uint32_t u32TIM1,uint32_t u32TIM3);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);

//----------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------

/*****************************************************************************************************************************
* See header file for detailed function description                                                                          *
*****************************************************************************************************************************/
void vQeiHardwareInit(void)
{
	//--------------------------------------------------------------------------------------------------------------------
	// Local data structures neccessary for initalizing peripherals
	//--------------------------------------------------------------------------------------------------------------------
	GPIO_InitTypeDef	    	GPIO_InitStruct;
	NVIC_InitTypeDef	    	NVIC_InitStruct;
	EXTI_InitTypeDef            EXTI_InitStructure;
	TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef	    	TIM_OCInitStruct;
		
	//--------------------------------------------------------------------------------------------------------------------
	// Initalize Encoder input pins
	//--------------------------------------------------------------------------------------------------------------------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;				//Configure encoder pins
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;				//Set pins as input
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;					//Set GPIO speed at 50MHz
	GPIO_Init(GPIOB,&GPIO_InitStruct);								//Initialize encoder pins 
	
	//--------------------------------------------------------------------------------------------------------------------
	// EXTI4 (channel A) initialization
	//--------------------------------------------------------------------------------------------------------------------
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode =  EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTI_InitStructure);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	//--------------------------------------------------------------------------------------------------------------------
	// EXTI5 (channel B) initialization
	//--------------------------------------------------------------------------------------------------------------------
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode =  EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTI_InitStructure);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
        
	//--------------------------------------------------------------------------------------------------------------------
	// Configure timer 3 as master timer (has MSB part of timer value). Timer 3 is used as clock divider for Timer 1.
	//--------------------------------------------------------------------------------------------------------------------
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0xFFFF;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStruct);
	
	//--------------------------------------------------------------------------------------------------------------------
	// Configure timer 1 as slave timer (has LSB part of timer value).
	//--------------------------------------------------------------------------------------------------------------------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
	TIM_OC1Init(TIM1, &TIM_OCInitStruct);
	
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_External1);
	TIM_SelectInputTrigger(TIM1, TIM_TS_ITR2);
	
	//--------------------------------------------------------------------------------------------------------------------
	// Enable timers
	//--------------------------------------------------------------------------------------------------------------------
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

/*****************************************************************************************************************************
* Encoder channel A pin change interrupt                                                                          			 *
*****************************************************************************************************************************/
void EXTI4_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line4);
	u16Ticks++;
	i16CHACount++;
	//Remember old values of encoder inputs
	u8QeiPinStatus = u8QeiPinStatus << 2;
	u8QeiPinStatus = u8QeiPinStatus & 0x0F;
	//Save new values of encoder inputs
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4))
	{
		u8QeiPinStatus |= (1 << A);
	}
	else
	{
	  u8QeiPinStatus &= ~(1 << A);
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5))
	{
		u8QeiPinStatus |= (1 << B);
	}
	else
	{
	  u8QeiPinStatus &= ~(1 << B);
	}   
	uint32_t timer1 = TIM1->CNT;
	uint32_t timer3 = TIM3->CNT;
	vQeiProcessIRQ(timer1,timer3);
}

/*****************************************************************************************************************************
* Encoder channel A pin change interrupt				                                                                     *
*****************************************************************************************************************************/
void EXTI9_5_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line5);
	
	i16CHBCount++;
	
	u8QeiPinStatus = u8QeiPinStatus << 2;
	u8QeiPinStatus = u8QeiPinStatus & 0x0F;
	
	//Save new values of encoder inputs
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4))
	{
		u8QeiPinStatus |= (1 << A);
	}
	else
	{
	  u8QeiPinStatus &= ~(1 << A);
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5))
	{
		u8QeiPinStatus |= (1 << B);
	}
	else
	{
	  u8QeiPinStatus &= ~(1 << B);
	}   
        
	uint32_t timer1 = TIM1->CNT;
	uint32_t timer3 = TIM3->CNT;
	vQeiProcessIRQ(timer1,timer3);
}

/*****************************************************************************************************************************
* Processes information from interrupt handler. Calculates current velocity.                                                 *
*****************************************************************************************************************************/
void vQeiProcessIRQ(uint32_t u32TIM1,uint32_t u32TIM3)
{
	i32Count += i8LookupTable[u8QeiPinStatus];  

	//------------------------------------------------------------------------------------------------------------------------
	// Get the direction of rotation. 1 means counterclockwise rotation, 0 means clockwise direction.
	//------------------------------------------------------------------------------------------------------------------------
	if(i8LookupTable[u8QeiPinStatus] == 1)
	{
		u8MotorDir = 1;
	}
	else if(i8LookupTable[u8QeiPinStatus] == -1)
	{
		u8MotorDir = 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	// Encoder ID run. Detect the phase offset and duty cycle of encoder input signal
	//------------------------------------------------------------------------------------------------------------------------
	if(u8IDRUN == 1)
	{
		//--------------------------------------------------------------------------------------------------------------------
		// If rising edge on channel A has occurred, record timer value and calculate phase offset
		//--------------------------------------------------------------------------------------------------------------------
		if(!(u8QeiPinStatus & (1<<OLD_A)) && (u8QeiPinStatus & (1<<A)))
		{
			u32CHA_Rising_Edge_Timer = u32TIM1;
			u32CHA_Rising_Edge_Timer = u32CHA_Rising_Edge_Timer << 16;
			u32CHA_Rising_Edge_Timer |= u32TIM3;
			fDutyCycle += (float)(((float)u32CHA_Falling_Edge_Timer / (float)u32CHA_Rising_Edge_Timer) * (float)100);
			u16CHA_Rising_Edge_count++;

			TIM1->CNT = 0;
			TIM3->CNT = 0;
		}

		//--------------------------------------------------------------------------------------------------------------------
		// If falling edge on channel A has occurred, record timer value.
		//--------------------------------------------------------------------------------------------------------------------
		if((u8QeiPinStatus & (1<<OLD_A)) && !(u8QeiPinStatus & (1<<A)))
		{
			u32CHA_Falling_Edge_Timer = u32TIM1;
			u32CHA_Falling_Edge_Timer = u32CHA_Falling_Edge_Timer << 16;
			u32CHA_Falling_Edge_Timer |= u32TIM3;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	// If encoder ID run complete
	//------------------------------------------------------------------------------------------------------------------------
	else
	{
		u32TimerValue = u32TIM1;
		u32TimerValue = u32TimerValue << 16;
		u32TimerValue |= u32TIM3;
		//--------------------------------------------------------------------------------------------------------------------
		// Calculate velocity
		//--------------------------------------------------------------------------------------------------------------------
		if(!(u8QeiPinStatus & (1<<OLD_A)) && (u8QeiPinStatus & (1<<A)))
		{
			//Rising edge of channel A. Measured time is longer. Multiply by fDutyCycle/50;
			fCurrentVelocity += u32TimerValue * (fDutyCycle/50);
			u16Count++;
			//Get current timer value
			u32CurrentTimer = u32TIM1;
			u32CurrentTimer = u32TimerValue << 16;
			u32CurrentTimer |= u32TIM3;
			//Compenstate for the time been on the interrupt
			TIM1->CNT = 0;
			TIM3->CNT = u32CurrentTimer - u32TimerValue;
		}
		else if((u8QeiPinStatus & (1<<OLD_A)) && !(u8QeiPinStatus & (1<<A)))
		{
			//Falling edge of channel A. Measured time is shorter. Divide by fDutyCycle/50;
			fCurrentVelocity += u32TimerValue / (fDutyCycle/50);
			u16Count++;
			u32CurrentTimer = u32TIM1;
			u32CurrentTimer = u32TimerValue << 16;
			u32CurrentTimer |= u32TIM3;
			TIM1->CNT = 0;
			TIM3->CNT = u32CurrentTimer - u32TimerValue;
		}
	}  
}

/*****************************************************************************************************************************
* See header file for detailed function description                                                                          *
*****************************************************************************************************************************/
void vQei(T_QEI* tQei)
{
	static uint16_t u16Time = 0;
	
	if(fDutyCycle == 0)
	{
		fDutyCycle = *tQei->fEncoderDuty;
	}
	///-------------------------------------------------------------------------------------------------------------------------
	// If duty cyle of encoder signal is 0, encoder ID run hasn't been performed and must be executed
	//--------------------------------------------------------------------------------------------------------------------------
	if(*tQei->fEncoderDuty <= 0)
	{
		tQei->u8QeiStatusWord = 0;
		u8IDRUN = 0;
	}
	else
	{
		tQei->u8QeiStatusWord = 1;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	// Encoder ID run not done, do it
	//--------------------------------------------------------------------------------------------------------------------------
	if(!(tQei->u8QeiStatusWord & (1<<ID_RUN_DONE)))
	{
		g_u8MotorControlDirection = 1;
		g_fPwmDutyCycle = 10;
		u8IDRUN = 1;
		if(u16Time >= 5000)
		{
			fDutyCycle = fDutyCycle / u16CHA_Rising_Edge_count;
	 		//-----------------------------------------------------------------------------------------------------------------------
	 		// Write parameters to flash
	 		//-----------------------------------------------------------------------------------------------------------------------
	 		vParameterFlashWrite(132,(uint32_t)(fDutyCycle*65536),F);
	 		fDutyCycle = 0;
	 		u16CHA_Rising_Edge_count = 0;
	 		//-----------------------------------------------------------------------------------------------------------------------
	 		// Stop motor
	 		//-----------------------------------------------------------------------------------------------------------------------
	 		g_fPwmDutyCycle = 0;
	 		u8IDRUN = 0;
	 		u16Time = 0;
	 		tQei->u8QeiStatusWord =1;
		}
		u16Time++;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	// Encoder ID run has been performed so calculation of speed can be done
	//--------------------------------------------------------------------------------------------------------------------------
	else
	{
		if(u16ErrorTimer > 100)
		{
			uint16_t u16Compart = ABS(i16CHBCount - i16CHACount);
			if(u16Compart > 50)
			{
				u16CumulativeErrors++;
				//GPIOA->BSRR = GPIO_Pin_15;
			}
			else
			{
				u16CumulativeErrors = 0;
			}
			if(i16CHACount == 0 && g_fUartSpeedReference != 0)
			{
				u16CumulativeErrors++;
			}
				
			i16CHACount = 0;
			i16CHBCount = 0;
			u16ErrorTimer = 0;
		}
		if(u16CumulativeErrors > 3)
		{
			GPIOA->BSRR = GPIO_Pin_15;
		}
		else
		{
			GPIOA->BRR = GPIO_Pin_15;
		}
		u16ErrorTimer++;
		if(fCurrentVelocity != 0 && u16Count >= 3)
		{
			testVel = 90000000/(fCurrentVelocity/u16Count);
			fCurrentVelocity = 0;
			u16Count = 0;
			u16Time = 0;
		}
		if(u16Time >= 10 )
		{
	 		testVel = 0;
	  		fCurrentVelocity = 0;
	 		u16Count = 0;
	  		u16Time = 0;
			
		}
		u16Time++;
		
	}
	
	//
	// Setting of outputs
	//
	if(u8MotorDir == 0)
	{
		tQei->fMotorSpeedAct = -testVel;
	}
	else
	{
		tQei->fMotorSpeedAct = testVel;
	}
	tQei->u8MotorDir = u8MotorDir;
}
