/**
  ******************************************************************************
  * @file    power.c
  * @author  TUT Robotics Club NPO
  * @version V1.0
  * @date    08-July-2013
  * @brief   This file contains Power control functions.
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "power.h"
#include "powercfg.h"
#include "util.h"
#include "TaskHandler.h"
//#include "usb_lib.h"
//#include "usb_pwr.h"


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static GPIO_InitTypeDef GPIO_InitStructure;
static DMA_InitTypeDef DMA_InitStructure;
static ADC_InitTypeDef  ADC_InitStructure;
static __IO uint16_t ADC_ConvertedValue[3];
static uint16_t Voltage;
static uint16_t Temperature;
static uint16_t RefVoltage;
static uint32_t PowerOnDelay;
static uint32_t PowerOffDelay;

/* Private functions ---------------------------------------------------------*/
static void ConfigureGPIO(void);
static void ConfigureADC(void);

/**
  * @brief  Configure GPIO pins
  * @retval None
  */
static void ConfigureGPIO(void)
{
   /* PWR_CTRL and CHG_STAT clock enable */
  RCC_AHBPeriphClockCmd(PWR_CTRL_GPIO_CLK |
                        CHG_STAT_GPIO_CLK | UBAT_GPIO_CLK, ENABLE);
  
  /* PWR_CTRL output pin configuration */
  GPIO_InitStructure.GPIO_Pin = PWR_CTRL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;  
  GPIO_Init(PWR_CTRL_GPIO_PORT, &GPIO_InitStructure);
  
  /* CHG_STAT input pin configuration */
  GPIO_InitStructure.GPIO_Pin = CHG_STAT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz; 
  GPIO_Init(CHG_STAT_GPIO_PORT, &GPIO_InitStructure);  
  
  /* UBAT input pin configuration */
  GPIO_InitStructure.GPIO_Pin = UBAT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz; 
  GPIO_Init(UBAT_GPIO_PORT, &GPIO_InitStructure);  
}


/**
  * @brief  Configure ADC
  * @retval None
  */
static void ConfigureADC(void)
{
  /*------------------------ DMA1 configuration ------------------------------*/
  
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* DMA1 channel1 configuration */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue[0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 3;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);  
  
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /*----------------- ADC1 configuration with DMA enabled --------------------*/
  
  /* Enable The HSI (16Mhz) */
  RCC_HSICmd(ENABLE);
  
  /* Check that HSI oscillator is ready */
  while (!RCC_GetFlagStatus(RCC_FLAG_HSIRDY));
  
  /* Enable ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* ADC1 Configuration -----------------------------------------------------*/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = 0;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 3;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  /* Enable temperature sensor and Vref */
  ADC_TempSensorVrefintCmd(ENABLE);
  
  /* ADC1 regular channel configuration */
  ADC_RegularChannelConfig(ADC1, UBAT_ADC_CHANNEL,       1, ADC_SampleTime_24Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 2, ADC_SampleTime_24Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint,    3, ADC_SampleTime_24Cycles);
  
  /* Enable the request after last transfer for DMA Circular mode */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  
  /* Define delay between ADC1 conversions */
  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_7Cycles);
  
  /* Enable ADC1 Power Down during Delay */
  ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE);  
    
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  /* Wait until the ADC1 is ready */
  while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS));
  
  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConv(ADC1);
}


/* Public functions ----------------------------------------------------------*/

void Power_Init(void)
{
  ConfigureGPIO();
  ConfigureADC();
  
  /* Reset variables */
  PowerOnDelay = 200;
  PowerOffDelay = 0;
}

void Power_Task(void)
{  
  /* ADC conversion done ? */
  if (DMA_GetFlagStatus(DMA1_FLAG_TC1))  
  {        
    Voltage     = POWER_VOLTAGE_FORMULA(ADC_ConvertedValue[0]);
    Temperature = ADC_ConvertedValue[1];
    RefVoltage  = ADC_ConvertedValue[2];    
    
    /* Start next ADC1 Software Conversion */
    ADC_SoftwareStartConv(ADC1);
  }  
  
  /* Power on delay reached zero (when wasn't zero before) ? */
  if (Util_Countdown(&PowerOnDelay, TaskHandler_tableOfTasks[TASK_POWER].period, 1))
  {
    GPIO_WriteBit(PWR_CTRL_GPIO_PORT, PWR_CTRL_PIN, Bit_SET);
  }
  
  /* Power off delay reached zero (when wasn't zero before) ? */
  if (Util_Countdown(&PowerOffDelay, TaskHandler_tableOfTasks[TASK_POWER].period, 1))
  {
    //PowerOff();
    GPIO_WriteBit(PWR_CTRL_GPIO_PORT, PWR_CTRL_PIN, Bit_RESET);
  }
}


/**
  * @brief  Setting power status
  * @param  Delay: Delay until power off
  */
void Power_TurnOff(uint32_t Delay)
{  
  /* Take power on delay also into account to ensure off isn't faster than power up */
  PowerOffDelay = Delay + PowerOnDelay;    
}


/**
  * @brief  Getting supply voltage
  * @retval Millivolts
  */
uint16_t Power_GetSupplyVoltage(void)
{
  return Voltage;
}


/**
  * @brief  Is battery being charged
  * @retval true or false
  */
uint8_t Power_IsBatteryCharging(void)
{
	uint8_t status = 0;
  
  if (!GPIO_ReadInputDataBit(CHG_STAT_GPIO_PORT, CHG_STAT_PIN))
  {
    status = 1;
  }
  
  return status;
}


/**
  * @brief  Getting battery charge level
  * @retval Level from 0% to 100%
  */
uint8_t Power_GetBatteryLevel(void)
{
  uint16_t level = Voltage - 3000;
  level = MIN(1200, level);
  level = level / 12;  
  
  return level;
}


/**
  * @brief  Getting temperatur voltage
  * @retval Millidegrees in Celcius
  */
uint16_t Power_GetTemperature(void)
{
  return Temperature;
}



