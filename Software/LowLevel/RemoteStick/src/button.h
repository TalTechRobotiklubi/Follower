/**
  ******************************************************************************
  * @file    button.h
  * @author  TUT Robotics Club NPO
  * @version V1.0
  * @date    17-November-2016
  * @brief   This file contains definitions for Buttons control functions.
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_H
#define __BUTTON_H

#include "GPIO.h"

/* Exported functions ------------------------------------------------------- */
void Button_Init(void);
void Button_Task(void);
uint8_t Button_IsDown(GPIO_IdDef button);
uint8_t Button_IsPressed(GPIO_IdDef button);
uint8_t Button_IsReleased(GPIO_IdDef button);

#endif
