//#include <math.h>

#include "USART.h"
#include "GPIO.h"
#include "DataLayer.h"
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

typedef struct
{
	MessageAnalyseState analyseState;
	InterfaceMessage message;
	uint16_t calcCrc;
	splitU16 messageCrc;
	uint8_t length;
	uint8_t dataIndex;
	uint8_t head;
	uint8_t tail;
}
ReceivingDataState;

/*function declarations*/
static void USARTx_init(USART_TypeDef *uartX);
static GPIO_IdDef UARTx_GPIO_init(USART_TypeDef *uartX);
static void UARTx_IRQ_init(GPIO_IdDef);
static void UARTxBuffersInit(USART_TypeDef *uartX);
static void sendChar(USART_TypeDef *uartX, unsigned char data_char);
static void sendString(USART_TypeDef *uartX, unsigned char *data_string);
static void handleReceivedData(USART_TypeDef* activeUART, uint8_t* rxBuffer, ReceivingDataState* rxState);
static void sendMessage(USART_TypeDef *uartX, InterfaceMessage* msg);
static void sendMessageToUSART2(InterfaceMessage* msg);
static void handleTransmitData(void);
static Interface findInterface(USART_TypeDef *uartX);

//USART2 Rx
static unsigned char priv_USART2_RxBuf[USART_RX_BUFFER_SIZE];
static ReceivingDataState priv_USART2_RxState;
static volatile uint8_t priv_USART2_RxOverflow;

//UART4 Rx
static unsigned char priv_UART4_RxBuf[USART_RX_BUFFER_SIZE];
static ReceivingDataState priv_UART4_RxState;
static volatile uint8_t priv_UART4_RxOverflow;

//USART2 Tx
static unsigned char priv_USART2_TxBuf[USART_TX_BUFFER_SIZE];
static volatile uint8_t priv_USART2_TxHead;
static volatile uint8_t priv_USART2_TxTail;

// initialization of USART/UART
void USARTx_init(USART_TypeDef *uartX)
{
	GPIO_IdDef io;

	io = UARTx_GPIO_init(uartX);
	if (io == NUM_OF_GPIOS)
	{
		return;
	}
	UARTx_IRQ_init(io);
	UARTxBuffersInit(uartX);
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
	if (uartX == USART2)
		USART_ITConfig(uartX, USART_IT_TXE, ENABLE);
	//Enable UARTx
	USART_Cmd(uartX, ENABLE);
}


// initialize UARTx GPIO pins
GPIO_IdDef UARTx_GPIO_init(USART_TypeDef *uartX)
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

void UARTx_IRQ_init(GPIO_IdDef io)
{
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_InitStruct.NVIC_IRQChannel = GPIO_table[io].interrupt;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void UARTxBuffersInit(USART_TypeDef *uartX)
{
	if (uartX == USART2)
	{
		priv_USART2_RxState.tail = 0;
		priv_USART2_RxState.head = 0;
		priv_USART2_RxState.analyseState = noData;
		priv_USART2_TxTail = 0;
		priv_USART2_TxHead = 0;
		int i;
		for(i = 0; i < USART_TX_BUFFER_SIZE; i++)
			priv_USART2_TxBuf[i]='a';
	}
	else if (uartX == UART4)
	{
		priv_UART4_RxState.tail = 0;
		priv_UART4_RxState.head = 0;
		priv_UART4_RxState.analyseState = noData;
	}
}

void sendChar(USART_TypeDef *uartX, unsigned char data_char)
{
	uint8_t tempHead;

	if (uartX == USART2)
	{
		tempHead = (priv_USART2_TxHead + 1) & USART_TX_BUFFER_MASK;

		while(tempHead == priv_USART2_TxTail);

		priv_USART2_TxBuf[tempHead] = data_char;
		priv_USART2_TxHead = tempHead;
		USART_ITConfig(USART2, USART_IT_TXE,ENABLE);
	}
}

void sendString(USART_TypeDef *uartX, unsigned char *data_string)
{
	unsigned int i=0;
	while(data_string[i])
	{
		sendChar(uartX, (unsigned char)data_string[i]);
		i++;
	}
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
		tempHead = (priv_UART4_RxState.head + 1) & USART_RX_BUFFER_MASK;
		priv_UART4_RxState.head = tempHead;

		if(tempHead == priv_UART4_RxState.tail)
		{
			//ERROR: buffer overflow
			priv_UART4_RxOverflow = 1;
		}
		else
		{
			priv_UART4_RxBuf[tempHead] = rxdata;
			priv_UART4_RxOverflow = 0;
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
		tempHead = (priv_USART2_RxState.head + 1) & USART_RX_BUFFER_MASK;
		priv_USART2_RxState.head = tempHead;

		if(tempHead == priv_USART2_RxState.tail)
		{
			//ERROR: buffer overflow
			priv_USART2_RxOverflow = 1;
		}
		else
		{
			priv_USART2_RxBuf[tempHead] = rxdata;
			priv_USART2_RxOverflow = 0;
		}
	}

	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		uint8_t tempTail;
		if(priv_USART2_TxHead != priv_USART2_TxTail)
		{
			tempTail = ((priv_USART2_TxTail + 1) & (USART_TX_BUFFER_MASK));
			priv_USART2_TxTail = tempTail;
			USART_SendData(USART2, priv_USART2_TxBuf[tempTail]);
		}
		else
		{
			USART_ITConfig(USART2, USART_IT_TXE,DISABLE);
		}
		USART_ClearITPendingBit(USART2, USART_IT_TXE);
	}
}

void handleReceivedData(USART_TypeDef* activeUART, uint8_t* rxBuffer, ReceivingDataState* rxState)
{
	uint8_t tempHead;
	uint8_t tempTail;

	/*save the buffer tail and head into local variable*/
	USART_ITConfig(activeUART, USART_IT_RXNE, DISABLE);  //disable UART receive interrupts
	tempHead = rxState->head;
	tempTail = rxState->tail;
	USART_ITConfig(activeUART, USART_IT_RXNE, ENABLE);  // enable UART receive interrupts

	/*Analyze received data*/
	while (tempTail != tempHead)
	{
		switch (rxState->analyseState)
		{
			case noData:
				// search preambula '0xAA'
				if (rxBuffer[tempTail] == 0xaa || rxBuffer[tempTail] == 0xAA)
				{
					// clear message for next data
					rxState->message.length = 0;
					rxState->message.id = 0;
					rxState->message.data[0] = 0;
					rxState->message.data[1] = 0;
					rxState->message.data[2] = 0;
					rxState->message.data[3] = 0;
					rxState->message.data[4] = 0;
					rxState->message.data[5] = 0;
					rxState->message.data[6] = 0;
					rxState->message.data[7] = 0;
					// clear variables
					rxState->length = 0;
					rxState->dataIndex = 0;
					rxState->messageCrc.u16 = 0;
					//rxMessageIndex = 0;
					// preambula is part of CRC
					rxState->calcCrc = rxBuffer[tempTail];
					rxState->analyseState = preambulaFound;
				}
				break;
			case preambulaFound:
				rxState->message.id = rxBuffer[tempTail];
				rxState->calcCrc += rxState->message.id;
				rxState->analyseState = idOK;
				break;
			case idOK:
				rxState->message.length = rxBuffer[tempTail];
				__disable_irq();
				if (InterfaceHandler_checkIfReceivedMessageExists(findInterface(activeUART), &rxState->message))
				{
					rxState->calcCrc += rxState->message.length;
					rxState->length = rxState->message.length;
					rxState->analyseState = lengthOK;
				}
				else
				{
					// not valid message, continue with seaching next preambula
					rxState->analyseState = noData;
				}
				__enable_irq();
				break;
			case lengthOK:
				// copy data
				if (rxState->length != 0)
				{
					rxState->message.data[rxState->dataIndex] = rxBuffer[tempTail];
					rxState->calcCrc += rxBuffer[tempTail];
					rxState->dataIndex++;
					rxState->length--;
					if (rxState->length == 0)
					{
						// continue with CRC
						rxState->analyseState = crcFirstbyte;
					}
				}
				else
				{
					// length should not be 0 from the beginning -> logic mistake, start from the beginning
					rxState->analyseState = noData;
				}
				break;
			case crcFirstbyte:
				// store crc first byte
				rxState->messageCrc.u8.byteLow = rxBuffer[tempTail];
				rxState->analyseState = crcCheck;
				break;
			case crcCheck:
				// store 2nd byte
				rxState->messageCrc.u8.byteHigh = rxBuffer[tempTail];
				// check CRC
				if (rxState->calcCrc == rxState->messageCrc.u16)
				{
					// new valid message received, copy data into data layer
					InterfaceHandler_storeReceivedData(&rxState->message);
					// ready for next packet
					rxState->analyseState = noData;
				}
				else
				{
					rxState->analyseState = noData;
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
	rxState->tail = tempHead;
	USART_ITConfig(activeUART, USART_IT_RXNE, ENABLE);  // enable UART receive interrupts
}

void sendMessage(USART_TypeDef *uartX, InterfaceMessage* msg)
{
	splitU16 crc;
	uint16_t j;

	sendChar(uartX, 0xAA);
	crc.u16 = 0xAA;
	sendChar(uartX, msg->id);
	crc.u16 += msg->id;
	sendChar(uartX, msg->length);
	crc.u16 += msg->length;
	for (j = 0; j < msg->length; j++)
	{
		sendChar(uartX, msg->data[j]);
		crc.u16 += msg->data[j];
	}
	sendChar(uartX, crc.u8.byteLow);
	sendChar(uartX, crc.u8.byteHigh);
}

void sendMessageToUSART2(InterfaceMessage* msg)
{
	sendMessage(USART2, msg);
}

Interface findInterface(USART_TypeDef *uartX)
{
	Interface interface = NumberOfInterfaces;
	if (uartX == UART4)
		interface = InterfaceUART_IMU;
	else if (uartX == USART2)
		interface = InterfaceUART_Remote;
	return interface;
}

void handleTransmitData(void)
{
	InterfaceHandler_transmitData(findInterface(USART2), sendMessageToUSART2);
}

void USART_init()
{
	USARTx_init(USART2);
	USARTx_init(UART4);
}

/*Periodic UART task*/
void USART_task(void)
{
	handleReceivedData(USART2, priv_USART2_RxBuf, &priv_USART2_RxState);
	handleReceivedData(UART4, priv_UART4_RxBuf, &priv_UART4_RxState);
	handleTransmitData();
}
