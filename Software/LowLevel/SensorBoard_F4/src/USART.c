//#include <math.h>

#include "stm32f4xx.h"
#include "USART.h"
#include "GPIO.h"
#include "DataLayer.h"
#include "PacketHandler.h"
#include "TaskHandler.h"
#include "CANmessages.h"

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
GPIO_IdDef UART_GPIO_init(USART_TypeDef *uartX);
void UART_IRQ_init(GPIO_IdDef);
uint32_t intToASCIIchar(int32_t value, uint8_t *string);
void analyzeRecieveBuffer(void);
void storeDataToDataLayer(UART_CANmessage *message, PacketWithIndex *packet);
void sendUARTpackages(void);
void sendDataLayerDataToUART(PacketWithIndex *packet);
uint8_t getBitmaskForUARTmessage(uint8_t bitPosition, int16_t length);

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
	analyzeRecieveBuffer();
	sendUARTpackages();
}

void analyzeRecieveBuffer(void)
{
	uint16_t tempTail, tempHead;
	static MessageAnalyseState analyseState = noData;
	static UART_CANmessage message;
	static splitU16 messageCrc;
	static uint8_t length;
	static uint8_t dataIndex;
	static uint8_t rxMessageIndex;
	static PacketWithIndex *packet;
	uint8_t numOfPackets, j;

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
					message.canMessage.dlc = 0;
					message.canMessage.id = 0;
					message.canMessage.data[0] = 0;
					message.canMessage.data[1] = 0;
					message.canMessage.data[2] = 0;
					message.canMessage.data[3] = 0;
					message.canMessage.data[4] = 0;
					message.canMessage.data[5] = 0;
					message.canMessage.data[6] = 0;
					message.canMessage.data[7] = 0;
					message.crc = 0;
					// clear variables
					length = 0;
					dataIndex = 0;
					messageCrc.u16 = 0;
					rxMessageIndex = 0;
					// count also preambula into CRC calculation
					message.crc = USART_RxBuf[tempTail];
					analyseState = preambulaFound;
				}
				break;
			case preambulaFound:
				/* check first if it is a message to send further*/
				/* get list of messages to gateway, note that it is a Tx list */
				numOfPackets = Packet_getNumOfGatewayUARTtoCANPackets();
				packet = Packet_getGatewayUARTtoCANPackets();
				for (j = 0; j < numOfPackets; j++)
				{
					if (USART_RxBuf[tempTail] == packet[j].uiID)
					{
						// remember the index of the suitable message
						rxMessageIndex = j;
						message.canMessage.id = USART_RxBuf[tempTail];
						message.crc += USART_RxBuf[tempTail];
						analyseState = idOK;
						break;
					}
				}
				/* get list of UART rx messages to this module as it is not gateway message
				 * Now it is Rx list */
				if (analyseState != idOK)
				{
					numOfPackets = Packet_getNumOfUARTRxPackets();
					packet = Packet_getUARTRxPacketsData();
					for (j = 0; j < numOfPackets; j++)
					{
						if (USART_RxBuf[tempTail] == packet[j].uiID)
						{
							// remember the index of the suitable message
							rxMessageIndex = j;
							message.canMessage.id = USART_RxBuf[tempTail];
							message.crc += USART_RxBuf[tempTail];
							analyseState = idOK;
							break;
						}
					}
				}
				// if not ok message then start from beginning
				if (analyseState != idOK)
				{
					analyseState = noData;
				}
				break;
			case idOK:
				// check valid length (1-8)
				length = USART_RxBuf[tempTail];
				if (length == Packet_getMessageDLC(packet[rxMessageIndex].index))
				{
					message.canMessage.dlc = length;
					message.crc += length;
					analyseState = lengthOK;
				}
				else
				{
					// not valid message, continue with seaching next preambula
					analyseState = noData;
				}
				break;
			case lengthOK:
				// copy data

				if (length != 0)
				{
					message.canMessage.data[dataIndex] = USART_RxBuf[tempTail];
					message.crc += USART_RxBuf[tempTail];
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
				if (message.crc == messageCrc.u16)
				{
					// new valid data received, copy data into data layer
					storeDataToDataLayer(&message, &packet[rxMessageIndex]);
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
	USART_ITConfig(UART4,USART_IT_RXNE,DISABLE);  //disable UART receive interrupts
	USART_RxTail = tempHead;
	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);  // enable UART receive interrupts
}

/*saves data received from UART to data layer*/
void storeDataToDataLayer(UART_CANmessage *message, PacketWithIndex *packet)
{
	uint8_t byteIndex, bitPosition, j, type;
	int16_t length;
	uint16_t dataLayerOk;
	uint32_t data;

	dataLayerOk = 0;
	/*go through all the parameters in the message and store them into data layer*/
	for (j = 0; j < Packet_getMessageParameterCount(packet->index); j++)
	{
		byteIndex = ((Packet_getMessageParameterList(packet->index) + j)->uiStartBit / 8);
		bitPosition = ((Packet_getMessageParameterList(packet->index) + j)->uiStartBit % 8);
		length = (int16_t)((Packet_getMessageParameterList(packet->index) + j)->uiLengthBits);

		/*get parameter type from data layer*/
		type = DL_getDataType((Packet_getMessageParameterList(packet->index) + j)->eParam);
		switch(type)
		{
			case TypeBool:
			case TypeU4:
			case TypeS4:
			case TypeU8:
			case TypeS8:
				/*sanity check*/
				if (((bitPosition + length) <= 8) && ((bitPosition + length) >= 0))
				{
					/*involves only one byte */
					data = message->canMessage.data[byteIndex];
					/*data into normal value*/
					data = (data >> (8 - bitPosition - length)) & (0xFF >> (8 - length));
					//*((uint8_t*)(DL_GET_POINTER_TO_DATA(packet->psParameterList[j].eParam))) = (uint8_t)data;
					//DL_setDataValidity(packet->psParameterList[j].eParam, TRUE);
					DL_setDataByComm((Packet_getMessageParameterList(packet->index) + j)->eParam, &data);
					dataLayerOk++;
				}
				break;
			case TypeU16:
			case TypeS16:
				/*sanity check*/
				if ((length <= 16) && (length > 8) && (byteIndex < 7))
				{
					/*involves two bytes */
					/*first byte, bit position is assumed to be 0 and the byte is fully for this parameter*/
					data = (message->canMessage.data[byteIndex] << 8);
					length = length - 8;
					/*second byte, bit position is still 0, shift if length is not full byte*/
					data |= (message->canMessage.data[byteIndex + 1] & 0xFF);
					data = (uint16_t)(data >> (8 - length));
					//*((uint16_t*)(DL_GET_POINTER_TO_DATA(packet->psParameterList[j].eParam))) = (uint16_t)data;
					//DL_setDataValidity(packet->psParameterList[j].eParam, TRUE);
					DL_setDataByComm((Packet_getMessageParameterList(packet->index) + j)->eParam, &data);
					dataLayerOk++;
				}
				break;
			default:
				break;
		}
	}
	/*check if data storing to data layer was ok, if yes then update period depending if it is sporiadic or
	 * periodic message*/
	if (dataLayerOk == Packet_getMessageParameterCount(packet->index))
	{
		if (packet->iperiod >= 0)
		{
			packet->iperiod = 0;
		}
		else
		{
			/*mark it received*/
			packet->iperiod = PACKET_RECEIVED;
		}
	}

}

/*Checks if any UART message is ready for sending*/
void sendUARTpackages(void)
{
	uint8_t numOfPackets, i;
	PacketWithIndex *packet;

	/*check gateway messages*/
	numOfPackets = Packet_getNumOfGatewayCANtoUARTPackets();
	packet = Packet_getGatewayCANtoUARTPackets();
	for (i = 0; i < numOfPackets; i++)
	{
		/*check that period is indicating to sporiadic message*/
		if ((packet[i].iperiod < 0) && (Packet_getMessagePeriod(packet[i].index) < 0))
		{
			 /*It is signal gatewaying, the status "received" should be checked*/
			if (/*(packet[i].iperiod == PACKET_READY_TO_SEND) ||*/ (packet[i].iperiod == PACKET_RECEIVED))
			{
				/*send package*/
				sendDataLayerDataToUART(&packet[i]);
				/*come back here only if new need for sending sporiadic message*/
				packet[i].iperiod = PACKET_WAITING;
			}

		}
	}
	/*check UART messages*/
	numOfPackets = Packet_getNumOfUARTTxPackets();
	packet = Packet_getUARTTxPacketsData();
	for (i = 0; i < numOfPackets; i++)
	{
		/*is it a periodic message?*/
		if ((packet[i].iperiod >= 0) && (Packet_getMessagePeriod(packet[i].index) >= 0))
		{
			/*yes, increase the period counter by UART_TASK period*/
			packet[i].iperiod += TaskHandler_tableOfTasks[TASK_USART].period;
			/*check periodic messages, if the period is full*/
			if (packet[i].iperiod >= Packet_getMessagePeriod(packet[i].index))
			{
				/*send package*/
				sendDataLayerDataToUART(&packet[i]);
				/*set period counter back to 0*/
				packet[i].iperiod = 0;
			}
		}
		/*check that period is indicating to sporiadic message*/
		if ((packet[i].iperiod < 0) && (Packet_getMessagePeriod(packet[i].index) < 0))
		{
			/*check if sporadic message is ready to send*/
			if ( (packet[i].iperiod == PACKET_READY_TO_SEND) )
			{
				/*send package*/
				sendDataLayerDataToUART(&packet[i]);
				/*come back here only if new need for sending sporiadic message*/
				packet[i].iperiod = PACKET_WAITING;
			}
		}
	}
}

/*sets data layer data to UART Tx buffer*/
void sendDataLayerDataToUART(PacketWithIndex *packet)
{
	uint8_t byteIndex, bitPosition;
	uint16_t j;
	int16_t length;
	uint8_t bitmask;
	Type type;
	uint32_t data;
	UART_CANmessage message;
	splitU16 crc;

	/*set id and DLC, empty data*/
	message.canMessage.id = packet->uiID;
	message.canMessage.dlc = Packet_getMessageDLC(packet->index);
	for (j = 0; j < 8; j++)
	{
		message.canMessage.data[j] = 0;
	}

	/*set data - take all parameters from packet*/
	for (j = 0; j < Packet_getMessageParameterCount(packet->index); j++)
	{
		byteIndex = ((Packet_getMessageParameterList(packet->index) + j)->uiStartBit / 8);
		bitPosition = ((Packet_getMessageParameterList(packet->index) + j)->uiStartBit % 8);
		length = (int16_t)((Packet_getMessageParameterList(packet->index) + j)->uiLengthBits);
		/*get parameter type from data layer*/
		type = DL_getDataType((Packet_getMessageParameterList(packet->index) + j)->eParam);

		switch(type)
		{
			case TypeBool:
			case TypeU4:
			case TypeS4:
			case TypeU8:
			case TypeS8:
				/*involves only one byte */
				bitmask = getBitmaskForUARTmessage(bitPosition, length);
				//data = *((uint8_t*)(DL_GET_POINTER_TO_DATA(packet->psParameterList[j].eParam)));
				DL_getDataByComm((Packet_getMessageParameterList(packet->index) + j)->eParam, &data);
				message.canMessage.data[byteIndex] |= (((((uint8_t)data) & 0xFF) << (8 - length - bitPosition)) & bitmask);
				break;
			case TypeU16:
			case TypeS16:
				/*involves two bytes */
				//data = *((uint16_t*)(DL_GET_POINTER_TO_DATA(packet->psParameterList[j].eParam)));
				DL_getDataByComm((Packet_getMessageParameterList(packet->index) + j)->eParam, &data);
				/*first byte, bit position is assumed to be 0 and the byte is fully for this parameter*/
				message.canMessage.data[byteIndex] |= (uint8_t)((data >> (length - 8)) & 0xFF);
				/*second byte, bit position is still 0*/
				bitmask = getBitmaskForUARTmessage(0, length - 8);
				message.canMessage.data[byteIndex + 1] |= (uint8_t)(((data & 0xFF) << (16 - length)) & bitmask);
				break;
			case TypeU32:
			case TypeS32:
				break;
			default:
				break;
		}
	}

	/*send data*/
	USART_SendChar(0xAA);
	crc.u16 = 0xAA;
	USART_SendChar(message.canMessage.id);
	crc.u16 += message.canMessage.id;
	USART_SendChar(message.canMessage.dlc);
	crc.u16 += message.canMessage.dlc;
	for (j = 0; j < message.canMessage.dlc; j++)
	{
		USART_SendChar(message.canMessage.data[j]);
		crc.u16 += message.canMessage.data[j];
	}
	USART_SendChar(crc.u8.byteHigh);
	USART_SendChar(crc.u8.byteLow);

}

/*Finds which mask to use for data setting in message when bit position and data length is known
 Params: bitPosition - bit position in byte, NB! not absolute position in message
 	 	 length - remaining data length
 */
uint8_t getBitmaskForUARTmessage(uint8_t bitPosition, int16_t length)
{
  uint8_t mask = 0xFF;
  uint8_t zerosFromRight, i;

  // shift to left so upper bits are set to zero
  mask = mask >> bitPosition;

  if (length > 0)
  {
	  // check how many bits should be set to 0 from right side
	  if (length > 8) length = 8;
	  zerosFromRight = (8 - length - bitPosition);
	  for (i = 0; i < zerosFromRight; i++)
	  {
		  mask = mask & ~(1 << i);
	  }
	  // invert the mask as bits not involved with data should be set to 1
  }
  else
  {
	  mask = 0;
  }
  return mask;
}

