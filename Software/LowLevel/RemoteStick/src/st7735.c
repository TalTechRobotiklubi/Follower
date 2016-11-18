/**
  ******************************************************************************
  * @file    st7735.c
  * @author  TUT Robotics Club NPO
  * @version V1.0
  * @date    28-June-2013
  * @brief   This file contains ST7735 LCD driver control functions.
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "st7735.h"
#include "st7735cfg.h"
#include "util.h"

/* Private macros -------------------------------------------------------------*/
/* Private constants -----------------------------------------------------------*/
static const uint32_t DisplayWidth  = 159;
static const uint32_t DisplayHeight = 127;

/* Private types -------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void Delay(uint32_t Delay);
static void Reset(void);
static void CSActive(void);
static void CSDeactive(void);
static void WriteBus(uint8_t Data);
static void WriteData(uint8_t Data);
static void WriteCommand(uint8_t Command);
static void Program(void);
static void SetXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void SetPixel(uint16_t Color);

/* Private variables ---------------------------------------------------------*/
static GPIO_InitTypeDef GPIO_InitStructure;
static SPI_InitTypeDef  SPI_InitStructure;  

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Small blocking delay
  * @param  Delay: Milliseconds
  */
static void Delay(uint32_t Delay)
{
  uint32_t i;
  
  while (Delay > 0)
  {
    Delay--;
    for (i = 0; i < 8000; i++) {}
  }
}

/**
  * @brief  LCD external reset
  * @retval None
  */
static void Reset(void)
{
  /* Reset active-inactive*/  
  GPIO_WriteBit(LCD_RST_GPIO_PORT, LCD_RST_PIN, Bit_RESET);
  Delay(10);
  GPIO_WriteBit(LCD_RST_GPIO_PORT, LCD_RST_PIN, Bit_SET);
  Delay(10);
}

/**
  * @brief  SPI bus configuration
  * @retval None
  */
static void ConfigSPI(void)
{
  /* Enable the SPI peripheral */
  RCC_APB1PeriphClockCmd(LCD_SPI_CLK, ENABLE);

  /* Enable the LCD_RS and LCD_RST clocks */
  RCC_AHBPeriphClockCmd(LCD_RST_GPIO_CLK | LCD_RS_GPIO_CLK, ENABLE);
  
  /* Enable SCK, MOSI, MISO and NSS GPIO clocks */
  RCC_AHBPeriphClockCmd(LCD_SPI_SCK_GPIO_CLK | LCD_SPI_MISO_GPIO_CLK |
                        LCD_SPI_MOSI_GPIO_CLK | LCD_SPI_NSS_GPIO_CLK , ENABLE);  
  
  /* LCD_RS and LCD_RST configuration */  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;  
  
  /* LCD RST pin configuration */
  GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;
  GPIO_Init(LCD_RST_GPIO_PORT, &GPIO_InitStructure);  
  
  /* LCD RS pin configuration */
  GPIO_InitStructure.GPIO_Pin = LCD_RS_PIN;
  GPIO_Init(LCD_RS_GPIO_PORT, &GPIO_InitStructure);    

  /* SPI pins setup */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = LCD_SPI_SCK_PIN;
  GPIO_Init(LCD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin = LCD_SPI_MOSI_PIN;
  GPIO_Init(LCD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */  
  GPIO_InitStructure.GPIO_Pin = LCD_SPI_MISO_PIN;  
  GPIO_Init(LCD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
  /* SPI NSS pin configuration */
  GPIO_InitStructure.GPIO_Pin = LCD_SPI_NSS_PIN;
  GPIO_Init(LCD_SPI_NSS_GPIO_PORT, &GPIO_InitStructure);  
         
  /* SPI pin mappings */
  GPIO_PinAFConfig(LCD_SPI_NSS_GPIO_PORT, LCD_SPI_NSS_SOURCE, LCD_SPI_NSS_AF);  
  GPIO_PinAFConfig(LCD_SPI_SCK_GPIO_PORT, LCD_SPI_SCK_SOURCE, LCD_SPI_SCK_AF);
  GPIO_PinAFConfig(LCD_SPI_MOSI_GPIO_PORT, LCD_SPI_MOSI_SOURCE, LCD_SPI_MOSI_AF);
  GPIO_PinAFConfig(LCD_SPI_MISO_GPIO_PORT, LCD_SPI_MISO_SOURCE, LCD_SPI_MISO_AF);  

  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(LCD_SPI);
  SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial     = 7;
  
  /* Initializes the SPI communication */  
  SPI_Init(LCD_SPI, &SPI_InitStructure);
  
  /* Enable the SPI peripheral */
  SPI_Cmd(LCD_SPI, ENABLE);
  
  /* Enable CS commanding */
  SPI_SSOutputCmd(LCD_SPI, ENABLE);  
}

/**
  * @brief  Activate chip selection
  * @retval None
  */
static void CSActive(void)
{
  SPI_NSSInternalSoftwareConfig(LCD_SPI, SPI_NSSInternalSoft_Reset);
}

/**
  * @brief  Deactivate chip selection
  * @retval None
  */
static void CSDeactive(void)
{
  SPI_NSSInternalSoftwareConfig(LCD_SPI, SPI_NSSInternalSoft_Set);
}

/**
  * @brief  Writing on SPI bus
  * @param  Data: 8-bit data to send on TX
  * @retval None
  */
static void WriteBus(uint8_t Data)
{
  SPI_I2S_SendData(LCD_SPI, Data);  
  while (!SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE));    
}

/**
  * @brief  Writing data
  * @param  Command: 8-bit data
  * @retval None
  */
static void WriteData(uint8_t Data)
{    
  LCD_RS_GPIO_PORT->BSRRL = LCD_RS_PIN;
  WriteBus(Data);
}

/**
  * @brief  Writing command
  * @param  Command: 8-bit command
  * @retval None
  */
static void WriteCommand(uint8_t Command)
{
  LCD_RS_GPIO_PORT->BSRRH = LCD_RS_PIN;
  WriteBus(Command);
}

/**
  * @brief  Programming driver
  * @retval None
  */
static void Program(void)
{
  CSActive();  
  
  // Sleep exit
  WriteCommand(0x11);  
  Delay(10);  

  // ST7735R Frame Rate
  WriteCommand(0xB1); 
  WriteData(0x01);
  WriteData(0x2C);
  WriteData(0x2D); 
  WriteCommand(0xB2); 
  WriteData(0x01);
  WriteData(0x2C);
  WriteData(0x2D); 
  WriteCommand(0xB3); 
  WriteData(0x01);
  WriteData(0x2C);
  WriteData(0x2D); 
  WriteData(0x01);
  WriteData(0x2C);
  WriteData(0x2D); 

  WriteCommand(0xB4); //Column inversion 
  WriteData(0x07); 

  // ST7735R Power Sequence
  WriteCommand(0xC0); 
  WriteData(0xA2);
  WriteData(0x02);
  WriteData(0x84); 
  WriteCommand(0xC1);
  WriteData(0xC5); 
  WriteCommand(0xC2); 
  WriteData(0x0A);
  WriteData(0x00); 
  WriteCommand(0xC3); 
  WriteData(0x8A);
  WriteData(0x2A); 
  WriteCommand(0xC4); 
  WriteData(0x8A);
  WriteData(0xEE); 

  WriteCommand(0xC5); //VCOM 
  WriteData(0x0E); 

  WriteCommand(0x36); //MX, MY, RGB mode 
  WriteData(0xC8); 

  // ST7735R Gamma Sequence
  WriteCommand(0xe0); 
  WriteData(0x0f);
  WriteData(0x1a); 
  WriteData(0x0f);
  WriteData(0x18); 
  WriteData(0x2f);
  WriteData(0x28); 
  WriteData(0x20);
  WriteData(0x22); 
  WriteData(0x1f);
  WriteData(0x1b); 
  WriteData(0x23);
  WriteData(0x37);
  WriteData(0x00); 

  WriteData(0x07); 
  WriteData(0x02);
  WriteData(0x10); 
  WriteCommand(0xe1); 
  WriteData(0x0f);
  WriteData(0x1b); 
  WriteData(0x0f);
  WriteData(0x17); 
  WriteData(0x33);
  WriteData(0x2c); 
  WriteData(0x29);
  WriteData(0x2e); 
  WriteData(0x30);
  WriteData(0x30); 
  WriteData(0x39);
  WriteData(0x3f); 
  WriteData(0x00);
  WriteData(0x07); 
  WriteData(0x03);
  WriteData(0x10);  

  WriteCommand(0x2a);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x7f);
  WriteCommand(0x2b);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x9f);

  // Enable test command  
  WriteCommand(0xF0);
  WriteData(0x01); 
  
  // Disable RAM power save mode 
  WriteCommand(0xF6);
  WriteData(0x00); 

  // 65k mode 
  WriteCommand(0x3A); 
  WriteData(0x05); 
  
  // Display on
  WriteCommand(0x29);
  
  CSDeactive();
}

/**
  * @brief  Setting area of drawing.
  * @param  x1: Left X coordinate
  * @param  x2: Right X coordinate
  * @param  y1: Top Y coordinate
  * @param  y2: Bottom Y coordinate
  */
static void SetXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  if (LCD_ORIENTATION == ST7735_ORIENTATION_LANDSCAPE)
  {
    SWAP(uint16_t, x1, y1);
    SWAP(uint16_t, x2, y2)
    y1 = DisplayWidth - y1;
    y2 = DisplayWidth - y2;
    SWAP(uint16_t, y1, y2)
  }
  
  WriteCommand(0x2a);
  WriteData(x1 >> 8);
  WriteData(x1);
  WriteData(x2 >> 8);
  WriteData(x2);
  WriteCommand(0x2b);
  WriteData(y1 >> 8);
  WriteData(y1);
  WriteData(y2 >> 8);
  WriteData(y2);
  WriteCommand(0x2c);     
}

/**
  * @brief  Writing pixel color
  * @param  Color: Color of the pixel
  */
static void SetPixel(uint16_t Color)
{
  WriteData(Color >> 8);
  WriteData(Color & 0xFF);
}


/* Public functions ---------------------------------------------------------*/

/**
  * @brief  ST7735 controller initialization.  
  * @retval None
  */
void ST7735_Init(void)
{
  ConfigSPI();
  Reset();
  Program();  
}

/**
  * @brief  LCD periodic task
  * @param  Period: Task execution period in milliseconds  
  */
void ST7735_Task()
{
}

/**
  * @brief  Getting display width
  * @retval Display width in pixels
  */
uint16_t ST7735_GetWidth(void)
{
  return DisplayWidth;
}

/**
  * @brief  Getting display height
  * @retval Display height in pixels
  */
uint16_t ST7735_GetHeight(void)
{
  return DisplayHeight;
}


/**
  * @brief  Drawing one pixel 
  */
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  CSActive();  
  SetXY(x, y, x, y);
  SetPixel(color);  
  CSDeactive();  
}

/**
  * @brief  Drawing horizontal line
  */
void ST7735_DrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color)
{  
  uint16_t i;
  
  if (x1 < x0)
  {
    SWAP(uint16_t, x0, x1);
  }
  
  CSActive();      
  SetXY(x0, y, x1, y);  
  for (i = x0; i <= x1; i++)
  {
    SetPixel(color);    
  }  
  CSDeactive();    
}

/**
  * @brief  Drawing vertical line
  */
void ST7735_DrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
  uint16_t i;

  if (y1 < y0)
  {
    SWAP(uint16_t, y0, y1);
  }
  
  CSActive();  
  SetXY(x, y0, x, y1);    
  for (i = y0; i <= y1; i++)
  {
    SetPixel(color);    
  }  
  CSDeactive();  
}

/**
  * @brief  Filling whole screen
  */
void ST7735_Fill(uint16_t color)
{
  uint32_t i;  
    
  CSActive();  
  SetXY(0, 0, DisplayWidth, DisplayHeight);
  for (i = 0; i < ((DisplayWidth + 1) * (DisplayHeight + 1)); i++)
  {  
    SetPixel(color);
  }  
  CSDeactive();  
}

