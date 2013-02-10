
#ifndef OLED_LCD_H_
#define OLED_LCD_H_

#include "DataLayer.h"

/* Defines -------------------------------------------------------------------*/
#define OLED_I2C                         I2C1
#define OLED_I2C_CLK                     RCC_APB1Periph_I2C1
#define OLED_I2C_SCL_PIN                 GPIO_Pin_6                  /* PB.06 */
#define OLED_I2C_SCL_GPIO_PORT           GPIOB                       /* GPIOB */
#define OLED_I2C_SCL_GPIO_CLK            RCC_APB2Periph_GPIOB
#define OLED_I2C_SDA_PIN                 GPIO_Pin_7                  /* PB.07 */
#define OLED_I2C_SDA_GPIO_PORT           GPIOB                       /* GPIOB */
#define OLED_I2C_SDA_GPIO_CLK            RCC_APB2Periph_GPIOB
#define OLED_RST_PIN                     GPIO_Pin_15                 /* PC.15 */
#define OLED_RST_GPIO_PORT               GPIOC                       /* GPIOC */
#define OLED_RST_GPIO_CLK                RCC_APB2Periph_GPIOC

#define OLED_ADDR           0x78   /*!< LM75 address */
#define OLED_I2C_SPEED      400000 /*!< I2C Speed */

#define OLED_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define OLED_LONG_TIMEOUT         ((uint32_t)(10 * OLED_FLAG_TIMEOUT))

#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel		((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xCF


#define INCREMENT       1
#define KEEP            0

/*define rows for data*/
#define START_STOP_ROW			1
#define ATTACK_GOAL_ROW			2
#define MOTOR_1_ACT_SPEED_ROW   4
#define MOTOR_2_ACT_SPEED_ROW   5
#define MOTOR_3_ACT_SPEED_ROW   6

/* Function prototypes -------------------------------------------------------*/
void OLED_LowLevel_Init(void);
void OLED_Init(void);

void ERR_fun(void);

void Write_Command(unsigned char Data);
void Write_Data(unsigned char Data);
unsigned int OLED_I2C_Write(unsigned char CD, unsigned char *Data_Pointer, 
                             unsigned char DatPtrInc, unsigned char DataSize);

void Fill_RAM(unsigned char Data);
void Fill_Block(unsigned char Data, unsigned char a, unsigned char b, 
                unsigned char c, unsigned char d);
void Show_Font57(unsigned char a, unsigned char b, unsigned char c);
void Show_String(unsigned char *Data_Pointer, unsigned char a, unsigned char b);

void Set_Start_Column(unsigned char d);
void Set_Addressing_Mode(unsigned char d);
void Set_Column_Address(unsigned char a, unsigned char b);
void Set_Page_Address(unsigned char a, unsigned char b);
void Set_Start_Line(unsigned char d);
void Set_Contrast_Control(unsigned char d);
void Set_Charge_Pump(unsigned char d);
void Set_Segment_Remap(unsigned char d);
void Set_Entire_Display(unsigned char d);
void Set_Inverse_Display(unsigned char d);
void Set_Multiplex_Ratio(unsigned char d);
void Set_Display_On_Off(unsigned char d);
void Set_Start_Page(unsigned char d);
void Set_Common_Remap(unsigned char d);
void Set_Display_Offset(unsigned char d);
void Set_Display_Clock(unsigned char d);
void Set_Precharge_Period(unsigned char d);
void Set_Common_Config(unsigned char d);
void Set_VCOMH(unsigned char d);
void Set_NOP(void);

extern void OLED_LCD_TASK(void);

#endif
