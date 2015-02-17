//#include <math.h>

#include "stm32f4xx.h"
#include "USART.h"
#include "GPIO.h"
#include "DataLayer.h"
#include "TaskHandler.h"
#include "InterfaceHandler.h"

/* splitted u16 type, which can be accessed in separate byte manner*/
typedef union
{
	struct
	{
		uint8_t byteLow;
		uint8_t byteHigh;
	} u8;
	uint16_t u16;
} splitU16;

// UART-CAN message format
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+
// | Byte 1  | Byte 2 | Byte 3              | Byte 4  | Byte 5  | ... | Byte 3+N | Byte 4+N | Byte 5+N |
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+
// | 0xAA    | Msg ID | length (n), n = 1-8 | Data[0] | Data[1] | ... | Data[N]  | Checksum | Checksum |
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+

// For UART_CAN message parsing a local state machine is used
typedef enum
{
	noData,
	preambulaFound,
	idOK,
	lengthOK,
	crcFirstbyte,
	crcCheck,
	messageOk
}
MessageAnalyseState;

/*function declarations*/
static GPIO_IdDef UART_GPIO_init(USART_TypeDef *uartX);
static void UART_IRQ_init(GPIO_IdDef);
static uint32_t intToASCIIchar(int32_t value, uint8_t *string);
static void handleReceiveData(void);
static void sendUARTmessage(InterfaceMessage* msg);
static void handleTransmitData(void);

//Rx buffer
static unsigned char USART_RxBuf[USART_RX_BUFFER_SIZE];
static volatile uint8_t USART_RxHead;
static volatile uint8_t USART_RxTail;
static volatile uint8_t USART_RxOverflow;

//Tx buffer
static unsigned char USART_TxBuf[USART_TX_BUFFER_SIZE];
static volatile uint8_t USART_TxHead;
static volatile uint8_t USART_TxTail;

USART_TypeDef *activeUART = UART4; // from default set UART4 as active


// initialization of USART/UART
void USART_init(USART_TypeDef *uartX)
{
	GPIO_IdDef io;

	io = UART_GPIO_init(uartX);
	if (io == NUM_OF_GPIOS)
	{
		return;
	}
	UART_IRQ_init(io);
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
	//Configure UARTx
	USART_Init(uartX, &USART_InitStruct);
	//Enable UARTx receive interrupts
	USART_ITConfig(uartX, USART_IT_RXNE, ENABLE);
	USART_ITConfig(uartX, USART_IT_TXE, ENABLE);
	//Enable UARTx
	USART_Cmd(uartX, ENABLE);
	// currently only 1 UART supported at a time
	activeUART = uartX;
}


// initialize UARTx GPIO pins
GPIO_IdDef UART_GPIO_init(USART_TypeDef *uartX)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_IdDef ioRx;
	GPIO_IdDef ioTx;

	//Enable UARTx clock and connect UARTx to pin source
	if (uartX == UART4)
	{
		ioRx = UART4_Rx;
		ioTx = UART4_Tx;
		//Enable GPIO clock
		RCC_AHB1PeriphClockCmd(GPIO_table[ioRx].clk, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		GPIO_PinAFConfig(GPIO_table[ioRx].port, GPIO_table[ioRx].pinSource, GPIO_AF_UART4);
		GPIO_PinAFConfig(GPIO_table[ioTx].port, GPIO_table[ioTx].pinSource, GPIO_AF_UART4);
	}
	else if (uartX == USART2)
	{
		ioRx = USART2_Rx;
		ioTx = USART2_Tx;
		//Enable GPIO clock
		RCC_AHB1PeriphClockCmd(GPIO_table[ioRx].clk, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		GPIO_PinAFConfig(GPIO_table[ioRx].port, GPIO_table[ioRx].pinSource, GPIO_AF_USART2);
		GPIO_PinAFConfig(GPIO_table[ioTx].port, GPIO_table[ioTx].pinSource, GPIO_AF_USART2);
	}
	else
	{
		return NUM_OF_GPIOS; // return enum value which should indicate that initialization went wrong
	}


	//Configure UARTx Rx as floating input
	GPIO_InitStruct.GPIO_Pin = GPIO_table[ioRx].pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIO_table[ioRx].port, &GPIO_InitStruct);

	//Configure UARTx Tx as alternate function push-pull
	GPIO_InitStruct.GPIO_Pin = GPIO_table[ioTx].pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIO_table[ioTx].port, &GPIO_InitStruct);

	return ioRx;
}

void UART_IRQ_init(GPIO_IdDef io)
{
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_InitStruct.NVIC_IRQChannel = GPIO_table[io].interrupt;
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

	USART_ITConfig(activeUART, USART_IT_TXE,ENABLE);
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

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		unsigned char rxdata;
		uint8_t tempHead;

		//Read one byte from receive data register
		rxdata = USART_ReceiveData(USART2);
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
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}

	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		uint8_t tempTail;
		if(USART_TxHead != USART_TxTail)
		{
			tempTail = ((USART_TxTail + 1) & (USART_TX_BUFFER_MASK));
			USART_TxTail = tempTail;
			USART_SendData(USART2, USART_TxBuf[tempTail]);
		}
		else
		{
			USART_ITConfig(USART2, USART_IT_TXE,DISABLE);
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

/*Periodic UART task*/
void USART_TASK(void)
{
	handleReceiveData();
	handleTransmitData();
}


void handleReceiveData(void)
{
	uint16_t tempTail, tempHead;
	static MessageAnalyseState analyseState = noData;
	static InterfaceMessage message;
	static uint16_t calcCrc;
	static splitU16 messageCrc;
	static uint8_t length;
	static uint8_t dataIndex;

	/*save the buffer tail and head into local variable*/
	USART_ITConfig(activeUART, USART_IT_RXNE, DISABLE);  //disable UART receive interrupts
	tempTail = USART_RxTail;
	tempHead = USART_RxHead;
	USART_ITConfig(activeUART, USART_IT_RXNE, ENABLE);  // enable UART receive interrupts

	/*Analyze received data*/
	while (tempTail != tempHead)
	{
		switch (analyseState)
		{
			case noData:
				// search preambula '0xAA'
				if ((USART_RxBuf[tempTail] == 0xaa) || (USART_RxBuf[tempTail] == 0xAA))
				{
					// clear message for next data
					message.length = 0;
					message.id = 0;
					message.data[0] = 0;
					message.data[1] = 0;
					message.data[2] = 0;
					message.data[3] = 0;
					message.data[4] = 0;
					message.data[5] = 0;
					message.data[6] = 0;
					message.data[7] = 0;
					// clear variables
					length = 0;
					dataIndex = 0;
					messageCrc.u16 = 0;
					//rxMessageIndex = 0;
					// preambula is part of CRC
					calcCrc = USART_RxBuf[tempTail];
					analyseState = preambulaFound;
				}
				break;
			case preambulaFound:
				message.id = USART_RxBuf[tempTail];
				calcCrc += message.id;
				analyseState = idOK;
				break;
			case idOK:
				message.length = USART_RxBuf[tempTail];
				__disable_irq();
				if (InterfaceHandler_checkIfReceivedMessageExists(InterfaceUART, &message))
				{
					calcCrc += message.length;
					length = message.length;
					analyseState = lengthOK;
				}
				else
				{
					// not valid message, continue with seaching next preambula
					analyseState = noData;
				}
				__enable_irq();
				break;
			case lengthOK:
				// copy data
				if (length != 0)
				{
					message.data[dataIndex] = USART_RxBuf[tempTail];
					calcCrc += USART_RxBuf[tempTail];
					dataIndex++;
					length--;
					if (length == 0)
					{
						// continue with CRC
						analyseState = crcFirstbyte;
					}
				}
				else
				{
					// length should not be 0 from the beginning -> logic mistake, start from the beginning
					analyseState = noData;
				}
				break;
			case crcFirstbyte:
				// store crc first byte
				messageCrc.u8.byteHigh = USART_RxBuf[tempTail];
				analyseState = crcCheck;
				break;
			case crcCheck:
				// store 2nd byte
				messageCrc.u8.byteLow = USART_RxBuf[tempTail];
				// check CRC
				if (calcCrc == messageCrc.u16)
				{
					// new valid message received, copy data into data layer
					InterfaceHandler_storeReceivedData(&message);
					// ready for next packet
					analyseState = noData;
				}
				else
				{
					analyseState = noData;
				}
				break;
			default:

				break;
		}
		/*take next byte*/
		tempTail = tempTail + 1;
		if (tempTail >= USART_RX_BUFFER_SIZE)
		{
			tempTail = 0;
		}
	}

	/*update the buffer tail*/
	USART_ITConfig(activeUART, USART_IT_RXNE, DISABLE);  //disable UART receive interrupts
	USART_RxTail = tempHead;
	USART_ITConfig(activeUART, USART_IT_RXNE, ENABLE);  // enable UART receive interrupts
}


void sendUARTmessage(InterfaceMessage* msg)
{
	splitU16 crc;
	uint16_t j;

	USART_SendChar(0xAA);
	crc.u16 = 0xAA;
	USART_SendChar(msg->id);
	crc.u16 += msg->id;
	USART_SendChar(msg->length);
	crc.u16 += msg->length;
	for (j = 0; j < msg->length; j++)
	{
		USART_SendChar(msg->data[j]);
		crc.u16 += msg->data[j];
	}
	USART_SendChar(crc.u8.byteHigh);
	USART_SendChar(crc.u8.byteLow);
}


void handleTransmitData(void)
{
	InterfaceHandler_transmitData(InterfaceUART, sendUARTmessage, TaskHandler_tableOfTasks[TASK_USART].period);
}
