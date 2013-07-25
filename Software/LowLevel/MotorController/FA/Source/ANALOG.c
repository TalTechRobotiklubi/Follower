#include "ANALOG.h"

uint16_t u16Current;
uint16_t u16Temp;
float fCurrent;
//----------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------
void vAnalogHardwareInit(void)
{
  ADC_InitTypeDef  ADC_InitStruct;
  GPIO_InitTypeDef GPIO_InitStruct;
  
  //--------------------------------------------------------------------------------------------------------------------------
  // Configure PA2 as analog input for current sensing
  //--------------------------------------------------------------------------------------------------------------------------
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA,&GPIO_InitStruct);
  
  //--------------------------------------------------------------------------------------------------------------------------
  // Configure and enable ADC
  //--------------------------------------------------------------------------------------------------------------------------
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  ADC_DeInit(ADC1);
  
  ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStruct.ADC_ScanConvMode = DISABLE;
  ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
  
  ADC_InitStruct.ADC_NbrOfChannel = 2;
  
  ADC_Init(ADC1, &ADC_InitStruct);
  ADC_Cmd(ADC1, ENABLE);
  ADC_TempSensorVrefintCmd(ENABLE);
  //--------------------------------------------------------------------------------------------------------------------------
  // Calibrate the ADC
  //--------------------------------------------------------------------------------------------------------------------------
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
}

//
//
//
void vAnalog(T_ANALOG*	ptInput)
{
	float fTempInVoltage = 0;
	float fCurrentInVoltage = 0;
	float fTemp = 0;
    float fCurrent = 0;
	//uint16_t tempTemp;
	u16Current = u16ReadAdcChannel(2);
	u16Temp = u16ReadAdcChannel(16);
  	
	fTempInVoltage = (u16Temp / 4096) * 3.3;
  	fCurrentInVoltage = (u16Current / 4096) * 3.3;
	
	//
	// Calculate actual temperature according to equation given in manual
	//
	fTemp = ((1.43 - fTempInVoltage)/4.3) + 25;
	ptInput->fTemperatureOut = fTemp;
    
    //
    // Calculate motor current in 
    //
    fCurrent = (fCurrentInVoltage / 0.087951)*10;
    ptInput->fCurrentOut = fCurrent;
}	

//
//
//
uint16_t u16ReadAdcChannel(uint8_t u8Channel)
{
  ADC_RegularChannelConfig(ADC1, u8Channel, 1, ADC_SampleTime_239Cycles5);
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  return ADC_GetConversionValue(ADC1);
}
