
/* Includes ------------------------------------------------------------------*/
#include "OLED_LCD.h"
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"


__IO uint32_t  OLEDTimeout = OLED_LONG_TIMEOUT; 

uint32_t intToASCIIchar(int32_t value, uint8_t *string);

// CHAR-S     Refer to "Times New Roman" Font Database...
unsigned char Ascii[95][5]={	
					
    {0x00,0x00,0x00,0x00,0x00},	//   (  0)    - 0x0020 No-Break Space
    {0x00,0x00,0x4F,0x00,0x00},	//   (  1)  ! - 0x0021 Exclamation Mark
    {0x00,0x07,0x00,0x07,0x00},	//   (  2)  " - 0x0022 Quotation Mark
    {0x14,0x7F,0x14,0x7F,0x14},	//   (  3)  # - 0x0023 Number Sign
    {0x24,0x2A,0x7F,0x2A,0x12},	//   (  4)  $ - 0x0024 Dollar Sign
    {0x23,0x13,0x08,0x64,0x62},	//   (  5)  % - 0x0025 Percent Sign
    {0x36,0x49,0x55,0x22,0x50},	//   (  6)  & - 0x0026 Ampersand
    {0x00,0x05,0x03,0x00,0x00},	//   (  7)  ' - 0x0027 Apostrophe
    {0x00,0x1C,0x22,0x41,0x00},	//   (  8)  ( - 0x0028 Left Parenthesis
    {0x00,0x41,0x22,0x1C,0x00},	//   (  9)  ) - 0x0029 Right Parenthesis
    {0x14,0x08,0x3E,0x08,0x14},	//   ( 10)  * - 0x002A Asterisk
    {0x08,0x08,0x3E,0x08,0x08},	//   ( 11)  + - 0x002B Plus Sign
    {0x00,0x50,0x30,0x00,0x00},	//   ( 12)  , - 0x002C Comma
    {0x08,0x08,0x08,0x08,0x08},	//   ( 13)  - - 0x002D Hyphen-Minus
    {0x00,0x60,0x60,0x00,0x00},	//   ( 14)  . - 0x002E Full Stop
    {0x20,0x10,0x08,0x04,0x02},	//   ( 15)  / - 0x002F Solidus
    {0x3E,0x51,0x49,0x45,0x3E},	//   ( 16)  0 - 0x0030 Digit Zero
    {0x00,0x42,0x7F,0x40,0x00},	//   ( 17)  1 - 0x0031 Digit One
    {0x42,0x61,0x51,0x49,0x46},	//   ( 18)  2 - 0x0032 Digit Two
    {0x21,0x41,0x45,0x4B,0x31},	//   ( 19)  3 - 0x0033 Digit Three
    {0x18,0x14,0x12,0x7F,0x10},	//   ( 20)  4 - 0x0034 Digit Four
    {0x27,0x45,0x45,0x45,0x39},	//   ( 21)  5 - 0x0035 Digit Five
    {0x3C,0x4A,0x49,0x49,0x30},	//   ( 22)  6 - 0x0036 Digit Six
    {0x01,0x71,0x09,0x05,0x03},	//   ( 23)  7 - 0x0037 Digit Seven
    {0x36,0x49,0x49,0x49,0x36},	//   ( 24)  8 - 0x0038 Digit Eight
    {0x06,0x49,0x49,0x29,0x1E},	//   ( 25)  9 - 0x0039 Dight Nine
    {0x00,0x36,0x36,0x00,0x00},	//   ( 26)  : - 0x003A Colon
    {0x00,0x56,0x36,0x00,0x00},	//   ( 27)  ; - 0x003B Semicolon
    {0x08,0x14,0x22,0x41,0x00},	//   ( 28)  < - 0x003C Less-Than Sign
    {0x14,0x14,0x14,0x14,0x14},	//   ( 29)  = - 0x003D Equals Sign
    {0x00,0x41,0x22,0x14,0x08},	//   ( 30)  > - 0x003E Greater-Than Sign
    {0x02,0x01,0x51,0x09,0x06},	//   ( 31)  ? - 0x003F Question Mark
    {0x32,0x49,0x79,0x41,0x3E},	//   ( 32)  @ - 0x0040 Commercial At
    {0x7E,0x11,0x11,0x11,0x7E},	//   ( 33)  A - 0x0041 Latin Capital Letter A
    {0x7F,0x49,0x49,0x49,0x36},	//   ( 34)  B - 0x0042 Latin Capital Letter B
    {0x3E,0x41,0x41,0x41,0x22},	//   ( 35)  C - 0x0043 Latin Capital Letter C
    {0x7F,0x41,0x41,0x22,0x1C},	//   ( 36)  D - 0x0044 Latin Capital Letter D
    {0x7F,0x49,0x49,0x49,0x41},	//   ( 37)  E - 0x0045 Latin Capital Letter E
    {0x7F,0x09,0x09,0x09,0x01},	//   ( 38)  F - 0x0046 Latin Capital Letter F
    {0x3E,0x41,0x49,0x49,0x7A},	//   ( 39)  G - 0x0047 Latin Capital Letter G
    {0x7F,0x08,0x08,0x08,0x7F},	//   ( 40)  H - 0x0048 Latin Capital Letter H
    {0x00,0x41,0x7F,0x41,0x00},	//   ( 41)  I - 0x0049 Latin Capital Letter I
    {0x20,0x40,0x41,0x3F,0x01},	//   ( 42)  J - 0x004A Latin Capital Letter J
    {0x7F,0x08,0x14,0x22,0x41},	//   ( 43)  K - 0x004B Latin Capital Letter K
    {0x7F,0x40,0x40,0x40,0x40},	//   ( 44)  L - 0x004C Latin Capital Letter L
    {0x7F,0x02,0x0C,0x02,0x7F},	//   ( 45)  M - 0x004D Latin Capital Letter M
    {0x7F,0x04,0x08,0x10,0x7F},	//   ( 46)  N - 0x004E Latin Capital Letter N
    {0x3E,0x41,0x41,0x41,0x3E},	//   ( 47)  O - 0x004F Latin Capital Letter O
    {0x7F,0x09,0x09,0x09,0x06},	//   ( 48)  P - 0x0050 Latin Capital Letter P
    {0x3E,0x41,0x51,0x21,0x5E},	//   ( 49)  Q - 0x0051 Latin Capital Letter Q
    {0x7F,0x09,0x19,0x29,0x46},	//   ( 50)  R - 0x0052 Latin Capital Letter R
    {0x46,0x49,0x49,0x49,0x31},	//   ( 51)  S - 0x0053 Latin Capital Letter S
    {0x01,0x01,0x7F,0x01,0x01},	//   ( 52)  T - 0x0054 Latin Capital Letter T
    {0x3F,0x40,0x40,0x40,0x3F},	//   ( 53)  U - 0x0055 Latin Capital Letter U
    {0x1F,0x20,0x40,0x20,0x1F},	//   ( 54)  V - 0x0056 Latin Capital Letter V
    {0x3F,0x40,0x38,0x40,0x3F},	//   ( 55)  W - 0x0057 Latin Capital Letter W
    {0x63,0x14,0x08,0x14,0x63},	//   ( 56)  X - 0x0058 Latin Capital Letter X
    {0x07,0x08,0x70,0x08,0x07},	//   ( 57)  Y - 0x0059 Latin Capital Letter Y
    {0x61,0x51,0x49,0x45,0x43},	//   ( 58)  Z - 0x005A Latin Capital Letter Z
    {0x00,0x7F,0x41,0x41,0x00},	//   ( 59)  [ - 0x005B Left Square Bracket
    {0x02,0x04,0x08,0x10,0x20},	//   ( 60)  \ - 0x005C Reverse Solidus
    {0x00,0x41,0x41,0x7F,0x00},	//   ( 61)  ] - 0x005D Right Square Bracket
    {0x04,0x02,0x01,0x02,0x04},	//   ( 62)  ^ - 0x005E Circumflex Accent
    {0x40,0x40,0x40,0x40,0x40},	//   ( 63)  _ - 0x005F Low Line
    {0x01,0x02,0x04,0x00,0x00},	//   ( 64)  ` - 0x0060 Grave Accent
    {0x20,0x54,0x54,0x54,0x78},	//   ( 65)  a - 0x0061 Latin Small Letter A
    {0x7F,0x48,0x44,0x44,0x38},	//   ( 66)  b - 0x0062 Latin Small Letter B
    {0x38,0x44,0x44,0x44,0x20},	//   ( 67)  c - 0x0063 Latin Small Letter C
    {0x38,0x44,0x44,0x48,0x7F},	//   ( 68)  d - 0x0064 Latin Small Letter D
    {0x38,0x54,0x54,0x54,0x18},	//   ( 69)  e - 0x0065 Latin Small Letter E
    {0x08,0x7E,0x09,0x01,0x02},	//   ( 70)  f - 0x0066 Latin Small Letter F
    {0x06,0x49,0x49,0x49,0x3F},	//   ( 71)  g - 0x0067 Latin Small Letter G
    {0x7F,0x08,0x04,0x04,0x78},	//   ( 72)  h - 0x0068 Latin Small Letter H
    {0x00,0x44,0x7D,0x40,0x00},	//   ( 73)  i - 0x0069 Latin Small Letter I
    {0x20,0x40,0x44,0x3D,0x00},	//   ( 74)  j - 0x006A Latin Small Letter J
    {0x7F,0x10,0x28,0x44,0x00},	//   ( 75)  k - 0x006B Latin Small Letter K
    {0x00,0x41,0x7F,0x40,0x00},	//   ( 76)  l - 0x006C Latin Small Letter L
    {0x7C,0x04,0x18,0x04,0x7C},	//   ( 77)  m - 0x006D Latin Small Letter M
    {0x7C,0x08,0x04,0x04,0x78},	//   ( 78)  n - 0x006E Latin Small Letter N
    {0x38,0x44,0x44,0x44,0x38},	//   ( 79)  o - 0x006F Latin Small Letter O
    {0x7C,0x14,0x14,0x14,0x08},	//   ( 80)  p - 0x0070 Latin Small Letter P
    {0x08,0x14,0x14,0x18,0x7C},	//   ( 81)  q - 0x0071 Latin Small Letter Q
    {0x7C,0x08,0x04,0x04,0x08},	//   ( 82)  r - 0x0072 Latin Small Letter R
    {0x48,0x54,0x54,0x54,0x20},	//   ( 83)  s - 0x0073 Latin Small Letter S
    {0x04,0x3F,0x44,0x40,0x20},	//   ( 84)  t - 0x0074 Latin Small Letter T
    {0x3C,0x40,0x40,0x20,0x7C},	//   ( 85)  u - 0x0075 Latin Small Letter U
    {0x1C,0x20,0x40,0x20,0x1C},	//   ( 86)  v - 0x0076 Latin Small Letter V
    {0x3C,0x40,0x30,0x40,0x3C},	//   ( 87)  w - 0x0077 Latin Small Letter W
    {0x44,0x28,0x10,0x28,0x44},	//   ( 88)  x - 0x0078 Latin Small Letter X
    {0x0C,0x50,0x50,0x50,0x3C},	//   ( 89)  y - 0x0079 Latin Small Letter Y
    {0x44,0x64,0x54,0x4C,0x44},	//   ( 90)  z - 0x007A Latin Small Letter Z
    {0x00,0x08,0x36,0x41,0x00},	//   ( 91)  { - 0x007B Left Curly Bracket
    {0x00,0x00,0x7F,0x00,0x00},	//   ( 92)  | - 0x007C Vertical Line
    {0x00,0x41,0x36,0x08,0x00},	//   ( 93)  } - 0x007D Right Curly Bracket
    {0x02,0x01,0x02,0x04,0x02},	//   ( 94)  ~ - 0x007E Tilde
};




/**
  * @brief  Initializes the OLED_I2C..
  */
void OLED_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /*!< OLED_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(OLED_I2C_CLK, ENABLE);
    
  /*!< OLED_I2C_SCL_GPIO_CLK, OLED_I2C_SDA_GPIO_CLK 
       and OLED_RST_GPIO_CLK Periph clock enable */
  RCC_APB2PeriphClockCmd(OLED_I2C_SCL_GPIO_CLK | OLED_I2C_SDA_GPIO_CLK |
                         OLED_RST_GPIO_CLK, ENABLE);
  
  /*!< Configure OLED_I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pin = OLED_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(OLED_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure OLED_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = OLED_I2C_SDA_PIN;
  GPIO_Init(OLED_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure OLED_I2C pin: RESET */
  GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(OLED_RST_GPIO_PORT, &GPIO_InitStructure); 
}

/* OLED INIT: VCC Generated by Internal DC/DC Circuit */
void OLED_Init(void)
{
  unsigned int count = 0;
  
  I2C_InitTypeDef   I2C_InitStructure;
  
  OLED_LowLevel_Init();
  // Reset LCD
  GPIO_WriteBit(OLED_RST_GPIO_PORT,OLED_RST_PIN ,Bit_RESET);
  I2C_DeInit(OLED_I2C);

  /*!< OLED_I2C Init */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = OLED_ADDR;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = OLED_I2C_SPEED;
  I2C_Init(OLED_I2C, &I2C_InitStructure);
  

  /*!< OLED_I2C Init */
  I2C_Cmd(OLED_I2C, ENABLE);
  while(count < 100000){
    count++;
  }
  // Release LCD reset
  GPIO_WriteBit(OLED_RST_GPIO_PORT,OLED_RST_PIN ,Bit_SET);
  
  Set_Display_On_Off(0x00);	      // Display Off (0x00/0x01)
  Set_Display_Clock(0x80);	      // Set Clock as 100 Frames/Sec
  Set_Multiplex_Ratio(0x3F);	      // 1/64 Duty (0x0F~0x3F)
  Set_Display_Offset(0x00);	      // Shift Mapping RAM Counter (0x00~0x3F)
  Set_Start_Line(0x00);		      // Set Mapping RAM Display Start Line 
                                      // (0x00~0x3F)
  Set_Charge_Pump(0x04);	      // Enable Embedded DC/DC Converter 
                                      // (0x00/0x04)
  Set_Addressing_Mode(0x02);	      // Set Page Addressing Mode 
                                      // (0x00/0x01/0x02)
  Set_Segment_Remap(0x01);	      // Set SEG/Column Mapping (0x00/0x01)
  Set_Common_Remap(0x08); 	      // Set COM/Row Scan Direction (0x00/0x08)
  Set_Common_Config(0x10);	      // Set Sequential Configuration 
                                      // (0x00/0x10)
  Set_Contrast_Control(Brightness);   // Set SEG Output Current
  Set_Precharge_Period(0xF1);	      // Set Pre-Charge as 15 Clocks & 
                                      // Discharge as 1 Clock
  Set_VCOMH(0x40);		      // Set VCOM Deselect Level
  Set_Entire_Display(0x00);	      // Disable Entire Display On (0x00/0x01)
  Set_Inverse_Display(0x00);	      // Disable Inverse Display On (0x00/0x01)

  Fill_RAM(0x00);		      // Clear Screen

  Set_Display_On_Off(0x01);	      // Display On (0x00/0x01)
  
}

/* Function for showing I2C ERROR on LED */
void ERR_fun(void) {
  GPIO_WriteBit(GPIOB,GPIO_Pin_15 ,Bit_SET);
  while(1) {;}
}


/* Function for write OLED LCD data */
unsigned int OLED_I2C_Write(unsigned char CD, unsigned char *Data_Pointer, 
                            unsigned char DatPtrInc, unsigned char DataSize){
  
  unsigned char *Src_Pointer;
  unsigned char i;

	Src_Pointer = Data_Pointer;
  
  /* Test on BUSY Flag */
  OLEDTimeout = OLED_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(OLED_I2C,I2C_FLAG_BUSY)){
    if((OLEDTimeout--) == 0){
      return 1;
    }
  }
  
  /* Enable the I2C peripheral & send START condition */
  I2C_GenerateSTART(OLED_I2C, ENABLE); 
  
  /* Test on SB Flag */
  OLEDTimeout = OLED_FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(OLED_I2C,I2C_FLAG_SB) == RESET){
    if((OLEDTimeout--) == 0){
      return 2;
    }
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(OLED_I2C, OLED_ADDR, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  OLEDTimeout = OLED_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
    if((OLEDTimeout--) == 0){
      return 3;
    }
  }
  
  /* Transmit first byte*/
  I2C_SendData(OLED_I2C, CD);
  OLEDTimeout = OLED_FLAG_TIMEOUT;
  while ((!I2C_GetFlagStatus(OLED_I2C,I2C_FLAG_TXE)) && 
         (!I2C_GetFlagStatus(OLED_I2C,I2C_FLAG_BTF))) {
    if((OLEDTimeout--) == 0){
      return 4;
    }
  }
  
  for(i = 0; i < DataSize; i++) {
    /* Transmit data bytes */
    I2C_SendData(OLED_I2C, *Src_Pointer);
    if(DatPtrInc) {
      Src_Pointer++;
    }
    OLEDTimeout = OLED_FLAG_TIMEOUT;
    while ((!I2C_GetFlagStatus(OLED_I2C,I2C_FLAG_TXE)) && 
           (!I2C_GetFlagStatus(OLED_I2C,I2C_FLAG_BTF))) {
      if((OLEDTimeout--) == 0){
        return 5;
      }
    }
  }
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(OLED_I2C, ENABLE);
  return 0;
}

/* Function to write Command Byte to LCD */
void Write_Command(unsigned char Data)
{
  if((OLED_I2C_Write(0x00, &Data, KEEP, 1)) != 0) {
    ERR_fun();
  } 
}

/* Function to write Data Byte to LCD */
void Write_Data(unsigned char Data)
{
  if((OLED_I2C_Write(0x40, &Data, KEEP, 1)) != 0) {
    ERR_fun();
  }
}




//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM(unsigned char Data)
{
unsigned char i;

  for(i=0;i<8;i++) {
    Set_Start_Page(i);
    Set_Start_Column(0x00);

    if((OLED_I2C_Write(0x40, &Data, KEEP, 128)) != 0) {
      ERR_fun();
    }
  }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)
//
//    a: Start Page
//    b: End Page
//    c: Start Column
//    d: Total Columns
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Block(unsigned char Data, unsigned char a, unsigned char b, 
                unsigned char c, unsigned char d)
{
unsigned char i;
	
    for(i=a;i<(b+1);i++) {
      Set_Start_Page(i);
      Set_Start_Column(c);

      if((OLED_I2C_Write(0x40, &Data, KEEP, d)) != 0) {
        ERR_fun();
      }
    }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Character (5x7)
//
//    a: Ascii char
//    b: Start Page
//    c: Start Column
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Font57(unsigned char a, unsigned char b, unsigned char c)
{
unsigned char *Src_Pointer;


    // ASCII subset
    if(a < 0x20 || a > 0x7E) {      // NOT in subset
      Src_Pointer=&Ascii[3][0];   // "#" char is selected
    } else {                        // In subset
      Src_Pointer=&Ascii[(a-0x20)][0];   
    }           
	
    Set_Start_Page(b);
    Set_Start_Column(c);
    
    if((OLED_I2C_Write(0x40, Src_Pointer, INCREMENT, 5)) != 0) {
      ERR_fun();
    }    
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show String
//
//    a: Start Page
//    b: Start Column
//    * Must write "0" in the end...
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_String(unsigned char *Data_Pointer, unsigned char a, unsigned char b)
{
unsigned char *Src_Pointer;

	Src_Pointer=Data_Pointer;
	Show_Font57(0x20,a,b);	// No-Break Space
			//   Must be written first before the string start...

	while(1)
	{
		Show_Font57(*Src_Pointer,a,b);
		Src_Pointer++;
		b+=6;
		if(*Src_Pointer == 0) break;
	}
}


//------------------------------------------------------------------------------




//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Start_Column(unsigned char d)
{
	Write_Command(0x00+d%16);     /* Set Lower Column Start Address for 
                                         Page Addressing Mode */
				      // Default => 0x00
	Write_Command(0x10+d/16);     /* Set Higher Column Start Address for 
                                         Page Addressing Mode */
				      // Default => 0x10
}


void Set_Addressing_Mode(unsigned char d)
{
	Write_Command(0x20);	      /* Set Memory Addressing Mode */
	Write_Command(d);	      // Default => 0x02
				      // 0x00 => Horizontal Addressing Mode
				      // 0x01 => Vertical Addressing Mode
				      // 0x02 => Page Addressing Mode
}


void Set_Column_Address(unsigned char a, unsigned char b)
{
	Write_Command(0x21);	      /* Set Column Address */
	Write_Command(a);	      // Default => 0x00 (Column Start Address)
	Write_Command(b);	      // Default => 0x7F (Column End Address)
}


void Set_Page_Address(unsigned char a, unsigned char b)
{
	Write_Command(0x22);	      /* Set Page Address */
	Write_Command(a);	      // Default => 0x00 (Page Start Address)
	Write_Command(b);	      // Default => 0x07 (Page End Address)
}


void Set_Start_Line(unsigned char d)
{
	Write_Command(0x40|d);	      /* Set Display Start Line */
				      // Default => 0x40 (0x00)
}


void Set_Contrast_Control(unsigned char d)
{
	Write_Command(0x81);	      /* Set Contrast Control */
	Write_Command(d);	      // Default => 0x7F
}


void Set_Charge_Pump(unsigned char d)
{
	Write_Command(0x8D);	      /* Set Charge Pump */
	Write_Command(0x10|d);	      // Default => 0x10
				      // 0x10 (0x00) => Disable Charge Pump
				      // 0x14 (0x04) => Enable Charge Pump
}


void Set_Segment_Remap(unsigned char d)
{
	Write_Command(0xA0|d);	      /* Set Segment Re-Map */
				      // Default => 0xA0
				      // 0xA0 (0x00) => Column Address 0 
                                      // Mapped to SEG0
				      // 0xA1 (0x01) => Column Address 0 
                                      // Mapped to SEG127
}


void Set_Entire_Display(unsigned char d)
{
	Write_Command(0xA4|d);	      /* Set Entire Display On / Off */
				      // Default => 0xA4
				      // 0xA4 (0x00) => Normal Display
				      // 0xA5 (0x01) => Entire Display On
}


void Set_Inverse_Display(unsigned char d)
{
	Write_Command(0xA6|d);	      /* Set Inverse Display On/Off */
				      // Default => 0xA6
				      // 0xA6 (0x00) => Normal Display
				      // 0xA7 (0x01) => Inverse Display On
}


void Set_Multiplex_Ratio(unsigned char d)
{
	Write_Command(0xA8);	      /* Set Multiplex Ratio */
	Write_Command(d);	      // Default => 0x3F (1/64 Duty)
}


void Set_Display_On_Off(unsigned char d)	
{
	Write_Command(0xAE|d);	      /* Set Display On/Off */
				      // Default => 0xAE
				      // 0xAE (0x00) => Display Off
				      // 0xAF (0x01) => Display On
}


void Set_Start_Page(unsigned char d)
{
	Write_Command(0xB0|d);	      /* Set Page Start Address for 
                                         Page Addressing Mode */
				      // Default => 0xB0 (0x00)
}


void Set_Common_Remap(unsigned char d)
{
	Write_Command(0xC0|d);	      /* Set COM Output Scan Direction */
				      // Default => 0xC0
				      // 0xC0 (0x00) => Scan from COM0 to 63
				      // 0xC8 (0x08) => Scan from COM63 to 0
}


void Set_Display_Offset(unsigned char d)
{
	Write_Command(0xD3);	      /* Set Display Offset */
	Write_Command(d);	      // Default => 0x00
}


void Set_Display_Clock(unsigned char d)
{
	Write_Command(0xD5);	      /* Set Display Clock Divide Ratio / 
                                         Oscillator Frequency */
	Write_Command(d);	      // Default => 0x80
				      // D[3:0] => Display Clock Divider
				      // D[7:4] => Oscillator Frequency
}


void Set_Precharge_Period(unsigned char d)
{
	Write_Command(0xD9);          /* Set Pre-Charge Period */
	Write_Command(d);	      // Default => 0x22 (2 Display Clocks  
                                      // [Phase 2] / 2 Display Clocks [Phase 1])
				      // D[3:0] => Phase 1 Period in 1~15 
                                      // Display Clocks
			              // D[7:4] => Phase 2 Period in 1~15 
                                      // Display Clocks
}


void Set_Common_Config(unsigned char d)
{
	Write_Command(0xDA);	      /* Set COM Pins Hardware Configuration */
	Write_Command(0x02|d);	      // Default => 0x12 (0x10)
			              // Alternative COM Pin Configuration
				      // Disable COM Left/Right Re-Map
}


void Set_VCOMH(unsigned char d)
{
	Write_Command(0xDB);	      /* Set VCOMH Deselect Level */
	Write_Command(d);	      // Default => 0x20 (0.77*VCC)
}

void Set_NOP()
{
	Write_Command(0xE3);	      /* Command for No Operation */
}

void OLED_LCD_TASK(void)
{
	uint8_t distance1;

	uint8_t row, column;
	uint8_t string[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Boolean newText = FALSE;

	//Fill_RAM(0x00);

	/*write sensor distance to LCD*/
	DL_peekData(DLParamDistanceSensor2, &distance1);

	intToASCIIchar(distance1, string);
	Show_String(&string[0], 4, 5);

	/*write out received characters*/
	if (DL_getData(DLParamScreenTextChar0, &string[0])) newText = TRUE;
	if (DL_getData(DLParamScreenTextChar1, &string[1])) newText = TRUE;
	if (DL_getData(DLParamScreenTextChar2, &string[2])) newText = TRUE;
	if (DL_getData(DLParamScreenTextChar3, &string[3])) newText = TRUE;
	if (DL_getData(DLParamScreenTextChar4, &string[4])) newText = TRUE;
	if (DL_getData(DLParamScreenTextChar5, &string[5])) newText = TRUE;

	if (newText)
	{
		DL_getData(DLParamScreenRow, &row);
		DL_getData(DLParamScreenColumn, &column);
		string[6] = 0;
		Show_String(&string[0], row, column);
	}


}

/*converts integer value to string (integer max 999.)
 * return value shows the string length (contains the terminator 0).
 * String value must be at least 6 bytes long*/
uint32_t intToASCIIchar(int32_t value, uint8_t *string)
{
	uint32_t length = 6;

	if (value < 0)
	{
		value = value * -1;
		string[0] = '-';
	}
	else
	{
		string[0] = ' ';
	}
	if (value < 10)
	{
		string[1] = value + 48;
		string[2] = ' ';
		string[3] = ' ';
		string[4] = ' ';
		string[5] = ' ';
		string[6] = 0;
	}
	else if (value < 100)
	{
		string[1] = (value / 10) + 48;
		string[2] = (value % 10) + 48;
		string[3] = ' ';
		string[4] = ' ';
		string[5] = ' ';
		string[6] = 0;
	}
	else if (value < 1000)
	{
		string[1] = (value / 100) + 48;
		string[2] = ((value / 10) % 10) + 48;
		string[3] = (value % 10) + 48;
		string[4] = ' ';
		string[5] = ' ';
		string[6] = 0;
	}
	else if(value < 10000)
	{
		string[1] = (value / 1000) + 48;
		string[2] = ((value / 100) % 10) + 48;
		string[3] = ((value / 10) % 10) + 48;
		string[4] = (value % 10) + 48;
		string[5] = ' ';
		string[6] = 0;
	}
	else if(value < 100000)
	{
		string[1] = (value / 10000) + 48;
		string[2] = ((value / 1000) % 10) + 48;
		string[3] = ((value / 100) % 10) + 48;
		string[4] = ((value / 10) % 10) + 48;
		string[5] = (value % 10) + 48;
		string[6] = 0;
	}
	return length;
}

