//#include <math.h>

#include "stm32f4xx.h"
#include "USART.h"
#include "GPIO.h"
#include "DataLayer.h"

//Rx buffer
static unsigned char USART_RxBuf[USART_RX_BUFFER_SIZE];
static volatile uint8_t USART_RxHead;
static volatile uint8_t USART_RxTail;
static volatile uint8_t USART_RxOverflow;

//Tx buffer
static unsigned char USART_TxBuf[USART_TX_BUFFER_SIZE];
static volatile uint8_t USART_TxHead;
static volatile uint8_t USART_TxTail;

int16_t value = 0;

/*function declarations*/
void UART4_GPIO_init(void);
void UART4_IRQ_init(void);
uint32_t intToASCIIchar(int32_t value, uint8_t *string);

// initialization function over UART4
void USART_UART4_init(void)
{
	UART4_GPIO_init();
	UART4_IRQ_init();
	USART_RxTail = 0;
	USART_RxHead = 0;
	USART_TxTail = 0;
	USART_TxHead = 0;
	int i;
	for(i=0;i<USART_TX_BUFFER_SIZE;i++)
	{
		USART_TxBuf[i]='a';
	}
	USART_InitTypeDef  USART_InitStruct;
	//Configure 8N1 UART with 115200 baud rate
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//Configure UART4
	USART_Init(UART4, &USART_InitStruct);
	//Enable UART4 receive interrupts
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
	//Enable UART4
	USART_Cmd(UART4, ENABLE);
}

// send distances
void USART_TASK_sendDistances(void)
{
	uint8_t distance1, distance2, distance3, distance4;
	//uint8_t const* delimiter = " ";
	uint8_t string[26] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	/*send sensors distances to UART*/
	DL_peekData(DLParamDistanceSensor1, &distance1);
	DL_peekData(DLParamDistanceSensor2, &distance2);
	DL_peekData(DLParamDistanceSensor3, &distance3);
	DL_peekData(DLParamDistanceSensor4, &distance4);

	intToASCIIchar(distance1, string);
	intToASCIIchar(distance2, &string[6]);
	intToASCIIchar(distance3, &string[12]);
	intToASCIIchar(distance4, &string[18]);
	USART_SendString(string);
	//USART_SendChar(*delimiter);
	//USART_SendInt(distance1);
	//USART_SendChar(*delimiter);
}

// initialize UART4 GPIO pins
void UART4_GPIO_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	//Enable GPIO clock
	RCC_AHB1PeriphClockCmd(GPIO_table[UART4_Rx].clk, ENABLE);
	//Enable UART4 clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	// connect UART4 to pin source
	GPIO_PinAFConfig(GPIO_table[UART4_Rx].port, GPIO_table[UART4_Rx].pinSource, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIO_table[UART4_Tx].port, GPIO_table[UART4_Tx].pinSource, GPIO_AF_UART4);

	//Configure UART4 Rx as floating input
	GPIO_InitStruct.GPIO_Pin = GPIO_table[UART4_Rx].pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIO_table[UART4_Rx].port, &GPIO_InitStruct);

	//Configure UART4 Tx as alternate function push-pull
	GPIO_InitStruct.GPIO_Pin = GPIO_table[UART4_Tx].pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIO_table[UART4_Tx].port, &GPIO_InitStruct);
/*
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pins 6 (TX) and 7 (RX) are used
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 			// the pins are configured as alternate function so the USART peripheral has access to them
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		// this defines the IO speed and has nothing to do with the baudrate!
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;			// this defines the output type as push pull mode (as opposed to open drain)
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// this activates the pullup resistors on the IO pins
		GPIO_Init(GPIOB, &GPIO_InitStruct);					// now all the values are passed to the GPIO_Init() function which sets the GPIO registers
*/
}

void UART4_IRQ_init(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

}

uint8_t USART_DataInRxBuffer(void)
{
	//Return 0 if receive buffer is empty
	return (USART_RxHead != USART_RxTail);
}

void USART_SendChar(unsigned char data_char)
{
	uint8_t tempHead;
	tempHead = (USART_TxHead + 1) & USART_TX_BUFFER_MASK;

	while(tempHead == USART_TxTail);

	USART_TxBuf[tempHead] = data_char;
	USART_TxHead = tempHead;

	USART_ITConfig(UART4, USART_IT_TXE,ENABLE);
}

void USART_SendString(unsigned char *data_string)
{
	unsigned int i=0;
	while(data_string[i])
	{
		USART_SendChar((unsigned char)data_string[i]);
		i++;
	}
}

void USART_SendInt(int data_int)
{
#if 0
	int digits,i,n;
	int begin=0;
	if(data_int == 0)
	{
		USART_SendChar('0');
		USART_SendChar('\r');
		USART_SendChar('\n');
		return;
	}
	digits = (int)log10(fabs((double)data_int));
	digits++;
	n=digits-1;
	char digit[digits];
	if(data_int < 0)
	{
		data_int *= -1;
		begin = 1;
		digit[0] = '-';
	}
	for(i=0;i<digits;i++)
	{
		digit[n+begin] = (data_int % 10)+48;
		data_int /= 10;
		n--;
	}
	for(i=0;i<digits+begin;i++)
	{
		USART_SendChar(digit[i]);
	}
	USART_SendChar('\r');
	USART_SendChar('\n');

#endif
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


void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		unsigned char rxdata;
		uint8_t tempHead;

		//Read one byte from receive data register
		rxdata = USART_ReceiveData(UART4);
		//Calculate buffer index
		tempHead = (USART_RxHead + 1) & USART_RX_BUFFER_MASK;
		USART_RxHead = tempHead;

		if(tempHead == USART_RxTail)
		{
			//ERROR: buffer overflow
			USART_RxOverflow = 1;
		}
		else
		{
			USART_RxBuf[tempHead] = rxdata;
			USART_RxOverflow = 0;
		}

		//
		if(rxdata == '\n') USART_ReceiveInt();
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
	}

	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
	{
		uint8_t tempTail;
		if(USART_TxHead != USART_TxTail)
		{
			tempTail = ((USART_TxTail + 1) & (USART_TX_BUFFER_MASK));
			USART_TxTail = tempTail;
			USART_SendData(UART4, USART_TxBuf[tempTail]);
		}
		else
		{
			USART_ITConfig(UART4, USART_IT_TXE,DISABLE);
		}
	}
}

void USART_ReceiveInt(void)
{
#if 0
	uint16_t p = 0;
	uint16_t i = 0;
	uint16_t d = 0;
	uint16_t f = 0;
	if(USART_RxBuf[1] == 'm')
	{
		value = 0;
		value += (USART_RxBuf[4] - 48) * pow(10,2);
		value += (USART_RxBuf[5] - 48) * pow(10,1);
		value += (USART_RxBuf[6] - 48);
	}
	else if(USART_RxBuf[1] == 'p')
	{
		p += (USART_RxBuf[3] - 48) * pow(10,3);
		p += (USART_RxBuf[4] - 48) * pow(10,2);
		p += (USART_RxBuf[5] - 48) * pow(10,1);
		p += (USART_RxBuf[6] - 48) * pow(10,0);
	}
	else if(USART_RxBuf[1] == 'i')
	{
		i += (USART_RxBuf[3] - 48) * pow(10,3);
		i += (USART_RxBuf[4] - 48) * pow(10,2);
		i += (USART_RxBuf[5] - 48) * pow(10,1);
		i += (USART_RxBuf[6] - 48) * pow(10,0);
	}
	else if(USART_RxBuf[1] == 'd')
	{
		d += (USART_RxBuf[3] - 48) * pow(10,3);
		d += (USART_RxBuf[4] - 48) * pow(10,2);
		d += (USART_RxBuf[5] - 48) * pow(10,1);
		d += (USART_RxBuf[6] - 48) * pow(10,0);
	}
	else if(USART_RxBuf[1] == 'f')
	{
		f += (USART_RxBuf[3] - 48) * pow(10,3);
		f += (USART_RxBuf[4] - 48) * pow(10,2);
		f += (USART_RxBuf[5] - 48) * pow(10,1);
		f += (USART_RxBuf[6] - 48) * pow(10,0);
	}
	if(USART_RxBuf[3] == '-') value *= -1;
	USART_RxHead = 0;
#endif
}
