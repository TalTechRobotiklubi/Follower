/**
  ******************************************************************************
  * @file    powercfg.h
  * @author  TUT Robotics Club NPO
  * @version V1.0
  * @date    08-July-2013
  * @brief   This file contains definitions for Power configuration.
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __POWERCFG_H
#define __POWERCFG_H
   
/* Includes ------------------------------------------------------------------*/

/* Public define -------------------------------------------------------------*/
#define ADC1_DR_ADDRESS       ((uint32_t)0x40012458)

/* Public macro --------------------------------------------------------------*/
#define PWR_CTRL_PIN          GPIO_Pin_11
#define PWR_CTRL_GPIO_PORT    GPIOB
#define PWR_CTRL_GPIO_CLK     RCC_AHBPeriph_GPIOB

#define CHG_STAT_PIN          GPIO_Pin_9
#define CHG_STAT_GPIO_PORT    GPIOC
#define CHG_STAT_GPIO_CLK     RCC_AHBPeriph_GPIOC

#define UBAT_PIN              GPIO_Pin_1
#define UBAT_GPIO_PORT        GPIOB
#define UBAT_GPIO_CLK         RCC_AHBPeriph_GPIOB
#define UBAT_ADC_CHANNEL      ADC_Channel_9

#define POWER_VOLTAGE_FORMULA(adc) ((adc * 3300 * 2) / 4095)

#endif
