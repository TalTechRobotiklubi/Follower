/**
  ******************************************************************************
  * @file    button.c
  * @author  TUT Robotics Club NPO
  * @version V1.0
  * @date    17-November-2016
  * @brief   This file contains Buttons control functions.
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "button.h"
#include "GPIO.h"

#define BUTTON_COUNT 4
#define ACTIVE_MASK  0x0F

/* Private variables ---------------------------------------------------------*/
static uint8_t Samples[BUTTON_COUNT];
static uint8_t Statuses[BUTTON_COUNT];
static uint8_t PressedEvents[BUTTON_COUNT];
static uint8_t ReleasedEvents[BUTTON_COUNT];

static void initButton(GPIO_IdDef io)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* GPIO configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;

    /* Enable clock */
    RCC_AHBPeriphClockCmd(GPIO_table[io].clk, ENABLE);
    
    /* GPIO configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_table[io].pin;
    GPIO_Init(GPIO_table[io].port, &GPIO_InitStructure);
}

static void readButton(GPIO_IdDef io, int i)
{
	/* Takes samples */
	if (GPIO_ReadInputDataBit(GPIO_table[io].port, GPIO_table[io].pin) == Bit_RESET)
	  Samples[i] = (Samples[i] << 1) | 1;
	else
	  Samples[i] <<= 1;

	/* All samples show pressing ? */
	if ((Samples[i] & ACTIVE_MASK) == ACTIVE_MASK)
	{
	  /* Pressed event? */
	  if (Statuses[i] == 0)
		PressedEvents[i] = 1;
	  /* Down */
	  Statuses[i] = 1;
	}
	/* All samples show unpressed ? */
	else if ((Samples[i] & ACTIVE_MASK) == 0)
	{
	  /* Released event? */
	  if (Statuses[i] == 1)
		ReleasedEvents[i] = 1;
	  /* Up */
	  Statuses[i] = 0;
	}
}

uint8_t findIndex(GPIO_IdDef button)
{
	switch(button)
	{
	case BTN_START:
		return 0;
	case BTN_STOP:
		return 1;
	case BTN_LEFT:
		return 2;
	case BTN_RIGHT:
		return 3;
	default:
		return 0;
	}
}

void Button_Init(void)
{
	uint8_t i;
	initButton(BTN_LEFT);
	initButton(BTN_RIGHT);
	initButton(BTN_START);
	initButton(BTN_STOP);
	for (i = 0; i < BUTTON_COUNT; ++i)
    {
		/* Reset variables */
		Samples[i] = 0x55;
		Statuses[i] = 0xFF;
		PressedEvents[i] = 0;
		ReleasedEvents[i] = 0;
    }
}



void Button_Task()
{
	readButton(BTN_START, findIndex(BTN_START));
	readButton(BTN_STOP, findIndex(BTN_STOP));
	readButton(BTN_LEFT, findIndex(BTN_LEFT));
	readButton(BTN_RIGHT, findIndex(BTN_RIGHT));
}

uint8_t Button_IsDown(GPIO_IdDef button)
{    
	return ((Statuses[findIndex(button)] == 1) ? 1 : 0);
}

uint8_t Button_IsPressed(GPIO_IdDef button)
{  
	uint8_t status = PressedEvents[findIndex(button)];
	PressedEvents[findIndex(button)] = 0;
	return status;
}

uint8_t Button_IsReleased(GPIO_IdDef button)
{  
	uint8_t status = ReleasedEvents[findIndex(button)];
	ReleasedEvents[findIndex(button)] = 0;
	return status;
}
