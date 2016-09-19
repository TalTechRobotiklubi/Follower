#include "CAN.h"

#include "stm32f4xx.h"
#include "GPIO.h"
#include "InterfaceHandler.h"

typedef enum
{
	CAN_Box_Not_Used,
	CAN_Box_Empty,
	CAN_Box_Locked,
	CAN_Box_Rx_New_Data
} CAN_Box_Flag;

typedef struct
{
	InterfaceMessage msg;
	uint8_t flag;
} CAN_MessageBox;

// every message has it's own message box
static CAN_MessageBox priv_CANmessageBoxes[NumberOfPackets];
static uint8_t  priv_bs1 = 7;
static uint8_t  priv_bs2 = 4;
static uint16_t priv_prescaler = 24;

static void initCAN1parameters(uint8_t bs1, uint8_t bs2, uint16_t prescaler);
static void initializeMessageBoxes(void);
static void CAN1_NVIC_Config(void);
static void sendCANmessage(InterfaceMessage* msg);
static void storeReceivedDataToMessageBox(InterfaceMessage* msg, CanRxMsg* canMsg);
static void handleReceivedData(void);
static void handleTransmitData();

void initCAN1parameters(uint8_t bs1, uint8_t bs2, uint16_t prescaler)
{
	CAN_InitTypeDef CAN_InitStructure;

	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

	/* CAN Baudrate = 125 kbit/s
	bit time = tq + tbs1 + tbs2 , where tq = (Prescale + 1) x APB1 clock*/
	CAN_InitStructure.CAN_BS1 = bs1;
	CAN_InitStructure.CAN_BS2 = bs2;
	CAN_InitStructure.CAN_Prescaler = prescaler;
	CAN_Init(CAN1, &CAN_InitStructure);
}


void initializeMessageBoxes(void)
{
	uint8_t i;

	for (i = 0; i < NumberOfPackets; i++)
	{
		priv_CANmessageBoxes[i].flag = CAN_Box_Not_Used;
	}
}


void CAN1_NVIC_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = GPIO_table[CAN1_Rx].interrupt;

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/*Initialized CAN1 module.*/
void CAN_CAN1Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/* CAN GPIOs configuration **************************************************/

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(GPIO_table[CAN1_Rx].clk, ENABLE);

	/* Connect CAN pins to AF9 */
	GPIO_PinAFConfig(GPIO_table[CAN1_Rx].port, GPIO_table[CAN1_Rx].pinSource, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIO_table[CAN1_Tx].port, GPIO_table[CAN1_Tx].pinSource, GPIO_AF_CAN1);

	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin = GPIO_table[CAN1_Rx].pin | GPIO_table[CAN1_Tx].pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIO_table[CAN1_Rx].port, &GPIO_InitStructure);

	/* CAN configuration ********************************************************/
	/* Enable CAN clock,
	 * APB1 clock is system clock / prescale -> 168 Mhz / 4 -> 42 Mhz */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	/* CAN register init */
	CAN_DeInit(CAN1);

	/* CAN cell init */
	initCAN1parameters(priv_bs1, priv_bs2, priv_prescaler);

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	/*message box init*/
	initializeMessageBoxes();

	/* Configure interrupt*/
	CAN1_NVIC_Config();

	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}


void sendCANmessage(InterfaceMessage* msg)
{
	uint8_t i = 0;
	CanTxMsg canMessage;

	canMessage.StdId = msg->id;
	canMessage.RTR = CAN_RTR_DATA;
	canMessage.IDE = CAN_ID_STD;
	canMessage.DLC = msg->length;

	if (msg->length >= 0 && msg->length <= 8)
	{
		for (i = 0; i < msg->length; i++)
		{
			canMessage.Data[i] = msg->data[i];
		}
		CAN_Transmit(CAN1, &canMessage);
	}
}


void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg receiveMsg;

	CAN_Receive(CAN1, CAN_FIFO0, &receiveMsg);

	InterfaceMessage msg;
	msg.id = receiveMsg.StdId;
	msg.length = receiveMsg.DLC;

	if (InterfaceHandler_checkIfReceivedMessageExists(InterfaceCAN, &msg))
	{
		storeReceivedDataToMessageBox(&msg, &receiveMsg);
	}
}


/* Stores CAN message data to message box. Every message has its own box.
 * Writes over previous data.
 * NB! Called from interrupt!!!*/
void storeReceivedDataToMessageBox(InterfaceMessage* msg, CanRxMsg* canMsg)
{
	if (priv_CANmessageBoxes[msg->packet].flag != CAN_Box_Locked)
	{
		int i;
		priv_CANmessageBoxes[msg->packet].msg.id = canMsg->StdId;
		priv_CANmessageBoxes[msg->packet].msg.length = canMsg->DLC;
		priv_CANmessageBoxes[msg->packet].msg.period = msg->period;
		priv_CANmessageBoxes[msg->packet].msg.packet = msg->packet;
		for ( i = 0; i < canMsg->DLC; i++)
			priv_CANmessageBoxes[msg->packet].msg.data[i] = canMsg->Data[i];
		priv_CANmessageBoxes[msg->packet].flag = CAN_Box_Rx_New_Data;
	}
}


/*check CAN message boxes and stores data to data layer if something new*/
void handleReceivedData(void)
{
	uint8_t i;

	for (i = 0; i < NumberOfPackets; i++)
	{
		/*check if new data has received to any of the message boxes*/
		if (priv_CANmessageBoxes[i].flag == CAN_Box_Rx_New_Data)
		{
			priv_CANmessageBoxes[i].flag = CAN_Box_Locked;
			InterfaceHandler_storeReceivedData(&(priv_CANmessageBoxes[i].msg));
			priv_CANmessageBoxes[i].flag = CAN_Box_Empty;
		}
	}
}

void handleTransmitData(void)
{
	InterfaceHandler_transmitData(InterfaceCAN, sendCANmessage);
}


void CAN_init(void)
{
	CAN_CAN1Init();
}


void CAN_task(void)
{
	handleReceivedData();
	handleTransmitData();
}

