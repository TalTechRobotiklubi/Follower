/**
  ******************************************************************************
  * @file    logic.c
  * @author  TUT Robotics Club NPO
  * @version V1.0
  * @date    18-August-2013
  * @brief   This file contains Buttons control functions.
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "power.h"
#include "button.h"
#include "math.h"
#include "logic.h"
#include "util.h"
#include "TaskHandler.h"
#include "drawing.h"
#include "fonts/verdana9.h"
#include "USART.h"
#include "DataLayer.h"

#define MENU_X 0
#define MENU_Y 20
#define MENU_ITEM_WIDTH 145
#define MENU_ITEM_HEIGHT 16
#define MENU_ITEM_COLOR COLOR_GREEN
#define MENU_ITEM_SELECTED_COLOR COLOR_BLUE
#define MENU_ARROW_SIZE 8

#define MENU_REMOTE_CONTROL 0
#define MENU_EMERGENCY 1

typedef struct
{
	int16_t x0;
	int16_t y0;
	int16_t x1;
	int16_t y1;
} MenuItem;

static uint32_t Uptime = 0;
static uint32_t ShutDownDelay;

const static MenuItem menuItems[] = {
	//  x0 ,              y,                     x1,                   			y1
	{MENU_X, 			        MENU_Y, MENU_X + MENU_ITEM_WIDTH, 					 MENU_ITEM_HEIGHT + MENU_Y},
	{MENU_X, MENU_ITEM_HEIGHT + MENU_Y, MENU_X + MENU_ITEM_WIDTH, MENU_ITEM_HEIGHT + MENU_ITEM_HEIGHT + MENU_Y}
};

static int16_t numOfMenuItems = sizeof(menuItems)/sizeof(menuItems[0]);

static void drawUiContent();
static void drawMenuItem(int index, char* txt, uint16_t color);
static void markMenuItemSelected(int index);
static void markMenuItemsDeactivated();
/* Private functions ---------------------------------------------------------*/

void drawUiContent()
{
	drawString(3, 3, COLOR_WHITE, &verdana9ptFontInfo, "Follower");
	drawMenuItem(MENU_EMERGENCY, "Emergency switch", MENU_ITEM_COLOR);
	drawMenuItem(MENU_REMOTE_CONTROL, "Remote control", MENU_ITEM_SELECTED_COLOR);
}

void drawMenuItem(int index, char* txt, uint16_t color)
{
	drawRectangle(menuItems[index].x0, menuItems[index].y0, menuItems[index].x1, menuItems[index].y1, color);
	drawString(menuItems[index].x0 + 3, menuItems[index].y0 + 2, COLOR_WHITE, &verdana9ptFontInfo, txt);
}

void markMenuItemSelected(int index)
{
	int i;
	if (index >= numOfMenuItems || index < 0)
		return;

	for (i = 0; i < numOfMenuItems; ++i)
		drawRectangle(menuItems[i].x0, menuItems[i].y0, menuItems[i].x1, menuItems[i].y1, MENU_ITEM_COLOR);
	drawRectangle(menuItems[index].x0, menuItems[index].y0, menuItems[index].x1,
			 	  menuItems[index].y1, MENU_ITEM_SELECTED_COLOR);
}

void markMenuItemActivated(int index)
{
	if (index >= numOfMenuItems || index < 0)
		return;

	drawArrow(menuItems[index].x1 + MENU_ARROW_SIZE + 3, menuItems[index].y1 - MENU_ITEM_HEIGHT / 2, MENU_ARROW_SIZE, DRAW_DIRECTION_RIGHT, COLOR_WHITE);
}

void markMenuItemsDeactivated()
{
	int i;
	for (i = 0; i < numOfMenuItems; ++i)
		drawArrow(menuItems[i].x1 + MENU_ARROW_SIZE + 3, menuItems[i].y1 - MENU_ITEM_HEIGHT / 2, MENU_ARROW_SIZE, DRAW_DIRECTION_RIGHT, COLOR_BLACK);
}

/**
  * @brief  Logic initialization.  
  * @retval None
  */
void Logic_Init(void)
{    
	drawFill(COLOR_BLACK);
	drawRectangle(0, 0, 159, 127, COLOR_GREEN);
	drawUiContent();
}


/**
  * @brief  Display clock in the screen corner  
  */
void DisplayClock(uint32_t Period)
{
  const uint16_t ClockLocationX = 118;
  const uint16_t ClockLocationY = 3;
  const uint16_t ClockWidth     = 32;
  const uint16_t ClockHeight    = 10;  
  char text[10];
  uint32_t Seconds = Uptime / 1000;
  static uint32_t PrevSeconds = 0xFFFFFF;
  
  if (Seconds >= 3600) Seconds = 3600 - 1;
  
  if (Seconds != PrevSeconds)
  {        
      uint32_t min = Seconds / 60;
      uint32_t sec = Seconds % 60;
	  // do not know why sprintf does not work
      text[0] = (char)min/10 + 48;
      text[1] = (char)min%10 + 48;
      text[2] = ':';
      text[3] = (char)sec/10 + 48;
      text[4] = (char)sec%10 + 48;
      text[5] = 0;
	  drawRectangleFilled(ClockLocationX, ClockLocationY, ClockLocationX + ClockWidth, ClockLocationY + ClockHeight, COLOR_BLACK);
	  drawString(ClockLocationX, ClockLocationY, COLOR_GRAY_225, &verdana9ptFontInfo, text);
      PrevSeconds = Seconds;
  }
}

/**
  * @brief  Display battery indicator on the screen corner  
  */
void DisplayBattery(uint32_t Period)
{
  const uint16_t BatteryLocationX = 150;
  const uint16_t BatteryLocationY = 3;
  const uint16_t BatteryWidth     = 6;
  const uint16_t BatteryHeight    = 10;  
  static uint8_t charging_level;
  static uint32_t charging_timer;  
  uint8_t battery_level;
  uint8_t isCharging;
  uint16_t y;
    
  /* Get battery status */
  isCharging = Power_IsBatteryCharging();
  battery_level = Power_GetBatteryLevel() / BatteryHeight;
  
  /* Display charging or display status */
  if (isCharging)
  {
    for (y = 0; y <= BatteryHeight; y++)
    {
      drawLine(BatteryLocationX, BatteryLocationY + BatteryHeight - y,
        BatteryLocationX + BatteryWidth, BatteryLocationY + BatteryHeight - y,
        ((y > charging_level) ? COLOR_BLACK : COLOR_RED));
    }
    
    /* Move charging indicator every 200ms */
    charging_timer += Period;
    if (charging_timer > 200)
    {
      charging_timer -= 200;    
      charging_level++;
      if (charging_level > BatteryHeight)
      {
        charging_level = 0;
      }
    }
  }  
  else
  {
    for (y = 0; y <= BatteryHeight; y++)
    {
      drawLine(BatteryLocationX, BatteryLocationY + BatteryHeight - y,
        BatteryLocationX + BatteryWidth, BatteryLocationY + BatteryHeight - y,
        ((y > battery_level) ? COLOR_BLACK : COLOR_GREEN));
    }
  }
}

#if 0
void DisplayStuff(void)
{
  char text[10];
  uint16_t ubat = Power_GetSupplyVoltage();
  uint16_t temp = Power_GetTemperature();
  
  sprintf(text, "%d mV", ubat);
  drawRectangleFilled(126, 21, 158, 35, COLOR_BLACK);
  drawString(126, 23, COLOR_GRAY_225, &verdana9ptFontInfo, text);  
  
  sprintf(text, "%d deg", temp);
  drawRectangleFilled(26, 21, 58, 35, COLOR_BLACK);
  drawString(26, 23, COLOR_GRAY_225, &verdana9ptFontInfo, text);
}

void DrawBar(uint16_t LocationX, uint16_t LocationY, uint16_t Width, uint16_t Range, int16_t Value, uint16_t Color)
{
  Value = MIN(Value,  Range);
  Value = MAX(Value, -Range);
  
  if (Value >= 0)
  {
    drawRectangleFilled(LocationX, LocationY - Range, LocationX + Width, LocationY - Value - 1, COLOR_BLACK);
    drawRectangleFilled(LocationX, LocationY + 1, LocationX + Width, LocationY + Range, COLOR_BLACK);
  }
  else
  {
    drawRectangleFilled(LocationX, LocationY - Range, LocationX + Width, LocationY - 1, COLOR_BLACK);
    drawRectangleFilled(LocationX, LocationY - Value + 1, LocationX + Width, LocationY + Range, COLOR_BLACK);
  }
  
  drawRectangleFilled(LocationX, LocationY, LocationX + Width, LocationY - Value, Color);
}

void DisplayAccelerometer(void)
{
  const uint16_t AccLocationX = 3;
  const uint16_t AccLocationY = 70;  
  
  Compass_Vector vector;
  Compass_ReadAcc(&vector);  
  vector.X /= 25;
  vector.Y /= 25;
  vector.Z /= 25;
  
  DrawBar(AccLocationX +  0, AccLocationY, 5, 30, vector.X, COLOR_BLUE);
  DrawBar(AccLocationX +  8, AccLocationY, 5, 30, vector.Y, COLOR_RED);
  DrawBar(AccLocationX + 16, AccLocationY, 5, 30, vector.Z, COLOR_YELLOW);
}

void DisplayMagnetomoter(void)
{
  const uint16_t MagLocationX = 40;
  const uint16_t MagLocationY = 70;    
  
  Compass_Vector vector;  
  Compass_ReadMag(&vector);
  vector.X /= 25;
  vector.Y /= 25;
  vector.Z /= 25;
  
  DrawBar(MagLocationX +  0, MagLocationY, 5, 30, vector.X, COLOR_BLUE);
  DrawBar(MagLocationX +  8, MagLocationY, 5, 30, vector.Y, COLOR_RED);
  DrawBar(MagLocationX + 16, MagLocationY, 5, 30, vector.Z, COLOR_YELLOW);
}

void DisplayGPS(void)
{
  /*
  GPS_Info * info;
  static uint32_t ppm;
  static uint8_t ps;
  char * nmea;
  char text[30];
  
  
  ps = GPS_GetPowerState();
  sprintf(text, "State: %d", ps);
  drawRectangleFilled(90, 28, 158, 39, COLOR_BLACK);
  drawString(90, 28, COLOR_WHITE, &verdana9ptFontInfo, text);  
  
  ppm = GPS_GetPPMCount();
  sprintf(text, "PPS: %d", ppm);
  drawRectangleFilled(90, 40, 158, 51, COLOR_BLACK);
  drawString(90, 40, COLOR_WHITE, &verdana9ptFontInfo, text);  
  
  nmea = GPS_GetLastNMEAString();
  if (nmea != NULL)
  {
    drawRectangleFilled(1, 115, 158, 126, COLOR_BLACK);
    drawString(2, 115, COLOR_WHITE, &verdana9ptFontInfo, nmea);
  }
  
  if (GPS_GetGPSInfo(&info))
  {
    sprintf(text, "%d.%d.%d", info->Year, info->Month, info->Day);
    drawRectangleFilled(70, 52, 158, 63, COLOR_BLACK);
    drawString(70, 52, COLOR_WHITE, &verdana9ptFontInfo, text);
    
    sprintf(text, "%d:%d:%d.%03d", info->Hour, info->Minute, info->Second, info->Millisecond);
    drawRectangleFilled(70, 64, 158, 75, COLOR_BLACK);
    drawString(70, 64, COLOR_WHITE, &verdana9ptFontInfo, text);
    
    sprintf(text, "Lat %f", info->Latitude);
    drawRectangleFilled(70, 76, 158, 87, COLOR_BLACK);
    drawString(70, 76, COLOR_WHITE, &verdana9ptFontInfo, text);
    
    sprintf(text, "Lon %f", info->Longitude);
    drawRectangleFilled(70, 88, 158, 99, COLOR_BLACK);
    drawString(70, 88, COLOR_WHITE, &verdana9ptFontInfo, text);
    
    sprintf(text, "Alt %f", info->Altitude);
    drawRectangleFilled(70, 100, 158, 111, COLOR_BLACK);
    drawString(70, 100, COLOR_WHITE, &verdana9ptFontInfo, text);
  }
  */
}

static char Files[64][12];
#endif
/**
  * @brief  Logic periodic task
  * @param  Period: Task execution period in milliseconds  
  */
void Logic_Task()
{
  static int selectedMenuIndex = 0;
  static uint8_t isInGeneralMenuMode = 1;
  
  uint32_t period = TaskHandler_tableOfTasks[TASK_LOGIC].period;
  Uptime += period;
  
  DisplayClock(period);
  DisplayBattery(period);

  if (Button_IsDown(BTN_LEFT) && Button_IsDown(BTN_RIGHT))
  {
    ShutDownDelay++;
    if (ShutDownDelay >= 2000/period)
    {
      Power_TurnOff(100);
    }
  }
  else
  {
    ShutDownDelay = 0;
  }
  
  if (isInGeneralMenuMode)
  {
	  if (Button_IsPressed(BTN_STOP)) // down in menu
	  {
		  ++selectedMenuIndex;
		  if (selectedMenuIndex >= numOfMenuItems)
			  selectedMenuIndex = numOfMenuItems - 1;
		  markMenuItemSelected(selectedMenuIndex);
	  }

	  if (Button_IsPressed(BTN_START)) // up in menu
	  {
		  --selectedMenuIndex;
		  if (selectedMenuIndex < numOfMenuItems)
			  selectedMenuIndex = 0;
		  markMenuItemSelected(selectedMenuIndex);
	  }

	  if (Button_IsPressed(BTN_RIGHT))  // activate mode
	  {
		  isInGeneralMenuMode = 0;
		  markMenuItemActivated(selectedMenuIndex);
	  }
  }
  else
  {
	  if (Button_IsPressed(BTN_LEFT))  // exit mode
	  {
		  isInGeneralMenuMode = 1;
		  markMenuItemsDeactivated();
	  }

	  if (selectedMenuIndex == MENU_EMERGENCY)
	  {
		  uint8_t activate = 1;
		  uint8_t deactivate = 0;
		  if (Button_IsPressed(BTN_START)) // set emergency situation
		  {
			  DL_setDataWithForcedAsyncSend(DLParamEmergencySetEvent, &activate);
			  DL_setDataWithForcedAsyncSend(DLParamEmergencyClearEvent, &deactivate);
		  }
		  if (Button_IsPressed(BTN_STOP)) // clear emergency situation
		  {
			  DL_setDataWithForcedAsyncSend(DLParamEmergencySetEvent, &deactivate);
			  DL_setDataWithForcedAsyncSend(DLParamEmergencyClearEvent, &activate);
		  }
	  }
  }
}

