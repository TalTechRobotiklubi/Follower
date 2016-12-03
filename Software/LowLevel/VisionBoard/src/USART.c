//#include <math.h>

#include "USART.h"
#include "GPIO.h"
#include "DataLayer.h"


/*function declarations*/
static void USARTx_init(USART_TypeDef *uartX);
static GPIO_IdDef UARTx_GPIO_init(USART_TypeDef *uartX);
static void UARTx_IRQ_init(GPIO_IdDef);
static void UARTxBuffersInit(USART_TypeDef *uartX);
static void sendChar(USART_TypeDef *uartX, unsigned char data_char);
static void handleTransmitData(void);

//USART2 Tx
static unsigned char priv_USART2_TxBuf[USART_TX_BUFFER_SIZE];
static volatile uint8_t priv_USART2_TxHead;
static volatile uint8_t priv_USART2_TxTail;

// initialization of USART/UART
void USARTx_init(USART_TypeDef *uartX)
{
	GPIO_IdDef io = UARTx_GPIO_init(uartX);
	if (io == NUM_OF_GPIOS)
		return;

	UARTx_IRQ_init(io);
	UARTxBuffersInit(uartX);
	USART_InitTypeDef  USART_InitStruct;
	//Configure 8N1 UART with 9600 baud rate
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//Configure UARTx
	USART_Init(uartX, &USART_InitStruct);

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
    if (uartX == USART2)
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
		priv_USART2_TxTail = 0;
		priv_USART2_TxHead = 0;
		int i;
		for(i = 0; i < USART_TX_BUFFER_SIZE; i++)
			priv_USART2_TxBuf[i]='a';
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

void USART2_IRQHandler(void)
{
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

void handleTransmitData(void)
{
	static uint8_t prev_activity = 0;
	uint8_t activity;
	if (DL_getData(DLParamVisualizationActivity, &activity))
	{
		if (activity != prev_activity)
		{
			sendChar(USART2, 'p');  // p
			sendChar(USART2, 'a');
			sendChar(USART2, 'g');
			sendChar(USART2, 'e');
			sendChar(USART2, ' ');
			sendChar(USART2, activity + 0x30);
			sendChar(USART2, 0xFF);
			sendChar(USART2, 0xFF);
			sendChar(USART2, 0xFF);
		}
		prev_activity = activity;
	}
}

void USART_init()
{
	USARTx_init(USART2);
}

/*Periodic UART task*/
void USART_task(void)
{
	handleTransmitData();
}
