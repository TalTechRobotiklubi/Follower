/**
  ******************************************************************************
  * @file    st7735cfg.h
  * @author  TUT Robotics Club NPO
  * @version V1.0
  * @date    10-June-2013
  * @brief   This file contains definitions for ST7735 LCD driver configuration.
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ST7735CFG_H
#define __ST7735CFG_H

/* Includes ------------------------------------------------------------------*/
#include "st7735.h"
   
/* Public macros ------------------------------------------------------------ */

/* General */
#define LCD_ORIENTATION                     ST7735_ORIENTATION_LANDSCAPE
   
/* SPI interface */
#define LCD_SPI                             SPI2
#define LCD_SPI_CLK                         RCC_APB1Periph_SPI2
#define LCD_SPI_IRQn                        SPI2_IRQn
#define LCD_SPI_IRQHandler                  SPI2_IRQHandler

#define LCD_SPI_SCK_PIN                     GPIO_Pin_13
#define LCD_SPI_SCK_GPIO_PORT               GPIOB
#define LCD_SPI_SCK_GPIO_CLK                RCC_AHBPeriph_GPIOB
#define LCD_SPI_SCK_SOURCE                  GPIO_PinSource13
#define LCD_SPI_SCK_AF                      GPIO_AF_SPI2

#define LCD_SPI_MISO_PIN                    GPIO_Pin_14
#define LCD_SPI_MISO_GPIO_PORT              GPIOB
#define LCD_SPI_MISO_GPIO_CLK               RCC_AHBPeriph_GPIOB
#define LCD_SPI_MISO_SOURCE                 GPIO_PinSource14
#define LCD_SPI_MISO_AF                     GPIO_AF_SPI2

#define LCD_SPI_MOSI_PIN                    GPIO_Pin_15
#define LCD_SPI_MOSI_GPIO_PORT              GPIOB
#define LCD_SPI_MOSI_GPIO_CLK               RCC_AHBPeriph_GPIOB
#define LCD_SPI_MOSI_SOURCE                 GPIO_PinSource15
#define LCD_SPI_MOSI_AF                     GPIO_AF_SPI2

#define LCD_SPI_NSS_PIN                     GPIO_Pin_12
#define LCD_SPI_NSS_GPIO_PORT               GPIOB
#define LCD_SPI_NSS_GPIO_CLK                RCC_AHBPeriph_GPIOB
#define LCD_SPI_NSS_SOURCE                  GPIO_PinSource12
#define LCD_SPI_NSS_AF                      GPIO_AF_SPI2

/* Reset pin */
#define LCD_RST_PIN                         GPIO_Pin_7
#define LCD_RST_GPIO_PORT                   GPIOC
#define LCD_RST_GPIO_CLK                    RCC_AHBPeriph_GPIOC

/* R/S pin */
#define LCD_RS_PIN                          GPIO_Pin_6
#define LCD_RS_GPIO_PORT                    GPIOC
#define LCD_RS_GPIO_CLK                     RCC_AHBPeriph_GPIOC


#endif
