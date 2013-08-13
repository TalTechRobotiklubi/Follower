#include "MODULATION.h"

#define MODULATING          0
#define POSITIVE_DIRECTION  1
TIM_OCInitTypeDef	    TIM_OCInitStruct;

float fPwmScaling = 192;

//----------------------------------------------------------------------------------------------------------------------------
// Configure peripherals necessary for modulation
//----------------------------------------------------------------------------------------------------------------------------
void vModulationHardwareInit()
{
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct;
  GPIO_InitTypeDef          GPIO_InitStruct;
  
  uint16_t u16PrescalerValue = 0;
  
  //--------------------------------------------------------------------------------------------------------------------------
  // Configure PA3 (PWM output), as push-pull type output
  //--------------------------------------------------------------------------------------------------------------------------
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA,&GPIO_InitStruct);
  GPIOA->BRR = GPIO_Pin_3;
  
  //--------------------------------------------------------------------------------------------------------------------------
  // Configure PA4 and PA6 as outputs to VNH2SP30 directional inputs
  //--------------------------------------------------------------------------------------------------------------------------
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA,&GPIO_InitStruct);
  GPIOA->BRR = GPIO_Pin_4;
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA,&GPIO_InitStruct);
  GPIOA->BRR = GPIO_Pin_6;
  //--------------------------------------------------------------------------------------------------------------------------
  // Configure PA7 as output to VNH2SP30 enable input
  //--------------------------------------------------------------------------------------------------------------------------
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA,&GPIO_InitStruct);
  GPIOA->BRR = GPIO_Pin_7;
  
  //--------------------------------------------------------------------------------------------------------------------------
  // Configure Timer 2 to generate PWM with 10kHz frequency (from 48MHz)
  //--------------------------------------------------------------------------------------------------------------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  u16PrescalerValue = (SystemCoreClock / 48000000) - 1;
  TIM_TimeBaseInitStruct.TIM_Period = 4800;
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
  TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
  TIM_PrescalerConfig(TIM2,u16PrescalerValue, TIM_PSCReloadMode_Immediate);
  TIM_OCStructInit( &TIM_OCInitStruct );
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_Pulse = 0;
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC4Init(TIM2,&TIM_OCInitStruct);
  TIM_CtrlPWMOutputs(TIM2,ENABLE);
}

//----------------------------------------------------------------------------------------------------------------------------
// Cyclic function for modulation. To be executed every 2 milliseconds.
//----------------------------------------------------------------------------------------------------------------------------
void vModulation(T_MODULATION *tModulation)
{
  //--------------------------------------------------------------------------------------------------------------------------
  //If modulation is disabled and duty cycle is greater than 0, enable modulation.
  //--------------------------------------------------------------------------------------------------------------------------
  if(!(tModulation->u8ModulationStatusWord & (1<<MODULATING)))
  {
    //vModulationEnable();
    if((*tModulation->pfPwmDutyCycle) > 0)
    {
      vModulationEnable();
      tModulation->u8ModulationStatusWord |= (1<<MODULATING);
    }
  }
  //--------------------------------------------------------------------------------------------------------------------------
  //If modulation is enabled and duty cycle is less or equal to 0, disable modulation.
  //--------------------------------------------------------------------------------------------------------------------------
  else
  {
    if(*tModulation->pfPwmDutyCycle <= 0)
    {
      vModulationDisable();
      tModulation->u8ModulationStatusWord &= ~(1<<MODULATING);
    }
  }
  
  //--------------------------------------------------------------------------------------------------------------------------
  // If modulation is enabled, set PWM duty cycle. Otherwise reset to PWM duty cycle to 0.
  //--------------------------------------------------------------------------------------------------------------------------
  if((tModulation->u8ModulationStatusWord & (1<<MODULATING)))
  {    
    //--------------------------------------------------------------------------------------------------------------------------
    // Set PWM duty cycle. Duty cycle comes in percents, so scaling must be applied
    //--------------------------------------------------------------------------------------------------------------------------
    //test =_IQ16toF(*tModulation->piq16PwmDutyCycle);
    TIM2->CCR4 = *tModulation->pfPwmDutyCycle * fPwmScaling;
    //TIM2->CCR4 = 0;
   
    //--------------------------------------------------------------------------------------------------------------------------
    // Set motor rotation direction
    //--------------------------------------------------------------------------------------------------------------------------
    if(*tModulation->pu8Direction == POSITIVE_DIRECTION)
    {
      GPIOA->BSRR = GPIO_Pin_4;
      GPIOA->BRR = GPIO_Pin_6;
    }
    else
    {
      GPIOA->BRR = GPIO_Pin_4;
      GPIOA->BSRR = GPIO_Pin_6;
    }
  }
  else
  {
    TIM2->CCR4 = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------------
// Enable modulation
//----------------------------------------------------------------------------------------------------------------------------
void vModulationEnable()
{
  GPIOA->BSRR = GPIO_Pin_7;
  TIM_ARRPreloadConfig(TIM2,ENABLE);
  TIM_Cmd(TIM2,ENABLE);
}

//----------------------------------------------------------------------------------------------------------------------------
// Disable modulation
//----------------------------------------------------------------------------------------------------------------------------
void vModulationDisable()
{
  GPIOA->BRR = GPIO_Pin_7;
  TIM_ARRPreloadConfig(TIM2,DISABLE);
  TIM_Cmd(TIM2,DISABLE);
}
