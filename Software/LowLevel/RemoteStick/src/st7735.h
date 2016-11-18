/**
  ******************************************************************************
  * @file    st7735h.h
  * @author  TUT Robotics Club NPO
  * @version V1.0
  * @date    10-June-2013
  * @brief   This file contains definitions for ST7735 LCD driver functions.
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ST7735_H
#define __ST7735_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Public macros -------------------------------------------------------------*/
#define ST7735_ORIENTATION_PORTRAIT      0
#define ST7735_ORIENTATION_LANDSCAPE     1
   
/* Exported functions ------------------------------------------------------- */
extern void ST7735_Init(void);
extern void ST7735_Task(void);
extern uint16_t ST7735_GetWidth(void);
extern uint16_t ST7735_GetHeight(void);
extern void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
extern void ST7735_DrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color);
extern void ST7735_DrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color);
extern void ST7735_Fill(uint16_t color);

#ifdef __cplusplus
}
#endif

#endif
