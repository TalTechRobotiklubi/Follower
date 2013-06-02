#include "CAN.h"
#include "stm32f4xx.h"
#include "PacketHandler.h"
#include "DataLayer.h"
#include "TaskHandler.h"
#include "GPIO.h"

typedef enum
{
	CAN_Box_Not_Used,
	CAN_Box_Empty,
	CAN_Box_Locked,
	CAN_Box_Rx_New_Data
} CAN_Box_Flag;

typedef struct
{
	uint8_t data[8];
	uint8_t dlc;
	uint8_t id;
	uint8_t flag;
} CAN_MessageBox;

//#define CAN_BAUDRATE 250

static CanRxMsg priv_RxMessage;
static CanTxMsg priv_TxMessage;

CAN_InitTypeDef        CAN_InitStructure;
CAN_FilterInitTypeDef  CAN_FilterInitStructure;
CanTxMsg TxMessage;

// every message has it's own message box
static CAN_MessageBox priv_CANmessageBoxes[NumberOfPackets];

void CAN1_NVIC_Config(void);
void initializeRxMessage(CanRxMsg *priv_RxMessage, uint32_t id, uint8_t dlc);
void initializeTxMessage(CanTxMsg *priv_TxMessage, uint32_t id, uint8_t dlc);
uint8_t getBitmaskForCANmessage(uint8_t bitPosition, int16_t length);
void sendDataLayerDataToCAN(PacketWithIndex *packet);
void storeCANDataToDataLayer(PacketWithIndex *packet);
void storeReceivedDataToMessageBox(Packet index);
void handleReceivedData(void);
void handleTransmitData(void);
void initializeMessageBoxes(void);

/**
  * @brief  Initializes a Rx Message.
  * @param  priv_RxMessage
  * @param  id
  * @param  dlc
  */
void initializeRxMessage(CanRxMsg *priv_RxMessage, uint32_t id, uint8_t dlc)
{
  uint8_t i = 0;

  priv_RxMessage->StdId = id;
  priv_RxMessage->ExtId = 0;
  priv_RxMessage->IDE = CAN_ID_STD;
  priv_RxMessage->DLC = dlc;
  priv_RxMessage->FMI = 0;
  for (i = 0; i < dlc; i++)
  {
    priv_RxMessage->Data[i] = 0;
  }
}

void initializeTxMessage(CanTxMsg *priv_TxMessage, uint32_t id, uint8_t dlc)
{
  uint8_t i = 0;

  priv_TxMessage->StdId = id;
  priv_TxMessage->RTR = CAN_RTR_DATA;
  priv_TxMessage->IDE = CAN_ID_STD;
  priv_TxMessage->DLC = dlc;
  for (i = 0; i < 8; i++)
  {
    priv_TxMessage->Data[i] = 0;
  }
}

void initializeMessageBoxes(void)
{
	uint8_t i;

	for (i = 0; i < NumberOfPackets; i++)
	{
		priv_CANmessageBoxes[i].flag = CAN_Box_Not_Used;
	}
}

/*Finds which mask to use for data setting in message when bit position and data length is known
 Params: bitPosition - bit position in byte, NB! not absolute position in message
 	 	 length - remaining data length
 */
uint8_t getBitmaskForCANmessage(uint8_t bitPosition, int16_t length)
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

/*#define CANx                       CAN1
  #define CAN_CLK                    RCC_APB1Periph_CAN1
  #define CAN_RX_PIN                 GPIO_Pin_0
  #define CAN_TX_PIN                 GPIO_Pin_1
  #define CAN_GPIO_PORT              GPIOD
  #define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOD
  #define CAN_AF_PORT                GPIO_AF_CAN1
  #define CAN_RX_SOURCE              GPIO_PinSource0
  #define CAN_TX_SOURCE              GPIO_PinSource1
*/

void CAN1_NVIC_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = GPIO_table[CAN1_Rx].interrupt; //CAN1_RX0_IRQn;

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*Initialized CAN1 module.*/
void CAN_CAN1Init()
{
	void CAN1_NVIC_Config(void);

	GPIO_InitTypeDef  GPIO_InitStructure;

	/* CAN GPIOs configuration **************************************************/

	/* Enable GPIO clock */
	//RCC_AHB1PeriphClockCmd( CAN_GPIO_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(GPIO_table[CAN1_Rx].clk, ENABLE);

	/* Connect CAN pins to AF9 */
	//GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
	//GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT);
	GPIO_PinAFConfig(GPIO_table[CAN1_Rx].port, GPIO_table[CAN1_Rx].pinSource, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIO_table[CAN1_Tx].port, GPIO_table[CAN1_Tx].pinSource, GPIO_AF_CAN1);

	/* Configure CAN RX and TX pins */
	//GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_table[CAN1_Rx].pin | GPIO_table[CAN1_Tx].pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	//GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);
	GPIO_Init(GPIO_table[CAN1_Rx].port, &GPIO_InitStructure);

	/* CAN configuration ********************************************************/
	/* Enable CAN clock,
	 * APB1 clock is system clock / prescale -> 168 Mhz / 4 -> 42 Mhz */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	/* CAN register init */
	CAN_DeInit(CAN1);

	/* CAN cell init */
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
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 41;
	CAN_Init(CAN1, &CAN_InitStructure);

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

	/* Transmit Structure preparation */
	TxMessage.StdId = 0x321;
	TxMessage.ExtId = 0x01;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.DLC = 1;

	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

	/*message box init*/
	//initializeMessageBoxes();



}

/**
  * @brief  This function handles CAN1 RX0 Handler.
  */
void CAN1_RX0_IRQHandler(void)
{
	uint8_t numOfPackets, i, messageHandled;
	PacketWithIndex *packet;

	CAN_Receive(CAN1, CAN_FIFO0, &priv_RxMessage);

	if (priv_RxMessage.IDE == CAN_ID_STD)
	{
		messageHandled = 0;

#if (ENABLE_GATEWAYING == 1)
		/*Check first gateway messages*/
		numOfPackets = Packet_getNumOfGatewayCANtoUARTPackets();
		packet = Packet_getGatewayCANtoUARTPackets();

		for (i = 0; i < numOfPackets; i++)
		{
			/*check if it is a message we expect*/
			if ((priv_RxMessage.StdId == packet[i].uiID) && (priv_RxMessage.DLC == Packet_getMessageDLC(packet[i].index)))
			{
				//storeCANDataToDataLayer(&packet[i]);
				storeReceivedDataToMessageBox(packet[i].index);
				messageHandled = 1;
				break;
			}
		}
		if (messageHandled)
		{
			return;
		}
#endif

#if (ENABLE_CAN_RECEIVE == 1)
		/*Check CAN messages to module*/

		numOfPackets = Packet_getNumOfCANRxPackets();
		packet = Packet_getCANRxPacketsData();

		for (i = 0; i < numOfPackets; i++)
		{
			/*check if it is a message we expect*/
			if ((priv_RxMessage.StdId == packet[i].uiID) && (priv_RxMessage.DLC == Packet_getMessageDLC(packet[i].index)))
			{
				//storeCANDataToDataLayer(&packet[i]);
				storeReceivedDataToMessageBox(packet[i].index);
				break;
			}
		}
#endif

	}
}

/* Stores CAN message data to message box. Every message has its own box.
 * Writes over previous data.
 * NB! Called from interrupt!!!*/
void storeReceivedDataToMessageBox(Packet index)
{
	uint8_t i;

	if (priv_CANmessageBoxes[index].flag != CAN_Box_Locked)
	{
		priv_CANmessageBoxes[index].id = priv_RxMessage.StdId;
		priv_CANmessageBoxes[index].dlc = priv_RxMessage.DLC;
		for(i = 0; i < priv_CANmessageBoxes[index].dlc; i++)
		{
			priv_CANmessageBoxes[index].data[i] = priv_RxMessage.Data[i];
		}
		priv_CANmessageBoxes[index].flag = CAN_Box_Rx_New_Data;
	}
}

/*Execution period -> check from TASK table*/
void CAN_TASK(void)
{
	handleTransmitData();
	handleReceivedData();
}

/*check CAN message boxes and stores data to data layer if something new*/
void handleReceivedData(void)
{
	uint8_t i, j, k, numOfPackets;
	PacketWithIndex* packet;
	uint8_t messageHandled = 0;

	for (i = 0; i < NumberOfPackets; i++)
	{
		/*check if new data has received to any of the message boxes*/
		if (priv_CANmessageBoxes[i].flag == CAN_Box_Rx_New_Data)
		{
			/*Check first gateway messages*/
			numOfPackets = Packet_getNumOfGatewayCANtoUARTPackets();
			packet = Packet_getGatewayCANtoUARTPackets();

			for (j = 0; j < numOfPackets; j++)
			{
				/*check if it is a message we expect*/
				if ((priv_CANmessageBoxes[i].id == packet[j].uiID) && (priv_CANmessageBoxes[i].dlc == Packet_getMessageDLC(packet[j].index)))
				{
					storeCANDataToDataLayer(&packet[j]);
					messageHandled = 1;
					break;
				}
			}
			if (messageHandled)
			{
				return;
			}

			/*Check CAN messages to module*/
			numOfPackets = Packet_getNumOfCANRxPackets();
			packet = Packet_getCANRxPacketsData();

			for (k = 0; k < numOfPackets; k++)
			{
				/*check if it is a message we expect*/
				if ((priv_CANmessageBoxes[i].id == packet[k].uiID) && (priv_CANmessageBoxes[i].dlc == Packet_getMessageDLC(packet[k].index)))
				{
					storeCANDataToDataLayer(&packet[k]);
					break;
				}
			}
		}
	}
}

/*saves data received from CAN to data layer.*/
void storeCANDataToDataLayer(PacketWithIndex *packet)
{
	uint8_t byteIndex, bitPosition, j, type;
	int16_t length;
	uint8_t dataLayerOk;
	uint32_t data;

	dataLayerOk = 0;

	priv_CANmessageBoxes[packet->index].flag = CAN_Box_Locked;
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
				if (((bitPosition + length) <= 8) && ((bitPosition + length) >= 0))
				{
					/*involves only one byte */
					data = priv_CANmessageBoxes[packet->index].data[byteIndex];
					/*sanity check*/
					data = (data >> (8 - bitPosition - length)) & (0xFF >> (8 - length));
					DL_setDataByComm((Packet_getMessageParameterList(packet->index) + j)->eParam, &data);
					//*((uint8_t*)(DL_GET_POINTER_TO_DATA(packet->psParameterList[j].eParam))) = (uint8_t)data;
					//DL_setDataValidity(packet->psParameterList[j].eParam, TRUE);
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
					data = (priv_CANmessageBoxes[packet->index].data[byteIndex] << 8);
					length = length - 8;
					/*second byte, bit position is still 0, shift if length is not full byte*/
					data |= (priv_CANmessageBoxes[packet->index].data[byteIndex + 1] & 0xFF);
					data = (uint16_t)(data >> (8 - length));
					DL_setDataByComm((Packet_getMessageParameterList(packet->index) + j)->eParam, &data);
					//*((uint16_t*)(DL_GET_POINTER_TO_DATA(packet->psParameterList[j].eParam))) = (uint16_t)data;
					//DL_setDataValidity(packet->psParameterList[j].eParam, TRUE);
					dataLayerOk++;
				}
				break;
			default:
				break;
		}
	}
	priv_CANmessageBoxes[packet->index].flag = CAN_Box_Empty;
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

void handleTransmitData(void)
{
	uint8_t numOfPackets, i;
	PacketWithIndex *packet;

#if	(ENABLE_GATEWAYING == 1)
	/*check gateway messages*/
	numOfPackets = Packet_getNumOfGatewayUARTtoCANPackets();
	packet = Packet_getGatewayUARTtoCANPackets();
	for (i = 0; i < numOfPackets; i++)
	{
		/*check that period is indicating to sporiadic message*/
		if ((packet[i].iperiod < 0) && (Packet_getMessagePeriod(packet[i].index) < 0))
		{
			/*check if sporadic message is ready to send,
			 *in case of signal gatewaying the status "received" should also be checked*/
			if ((packet[i].iperiod == PACKET_READY_TO_SEND) || (packet[i].iperiod == PACKET_RECEIVED))
			{
				/*send package*/
				sendDataLayerDataToCAN(&packet[i]);
				/*come back here only if new need for sending sporiadic message*/
				packet[i].iperiod = PACKET_WAITING;
			}

		}
	}
#endif

#if (ENABLE_CAN_TRANSMIT == 1)
	/*check periodic CAN messages to send*/
	numOfPackets = Packet_getNumOfCANTxPackets();
	packet = Packet_getCANTxPacketsData();
	for (i = 0; i < numOfPackets; i++)
	{
		/*is it a periodic message?*/
		if ((packet[i].iperiod >= 0) && (Packet_getMessagePeriod(packet[i].index) >= 0))
		{
			/*yes, increase the period counter by CAN_TASK period*/
			packet[i].iperiod += TaskHandler_tableOfTasks[TASK_CAN].period;
			/*check periodic messages, if the period is full*/
			if (packet[i].iperiod >= Packet_getMessagePeriod(packet[i].index))
			{
				/*send package*/
				sendDataLayerDataToCAN(&packet[i]);
				/*set period counter back to 0*/
				packet[i].iperiod = 0;
			}
		}
	}
#endif
}


/*gets data from DL and stores into CAN message*/
void sendDataLayerDataToCAN(PacketWithIndex *packet)
{
	uint8_t byteIndex, bitPosition;
	uint16_t j;
	int16_t length;
	uint8_t bitmask;
	Type type;
	uint32_t data;

	/*set id and DLC, empty data*/
	initializeTxMessage(&priv_TxMessage, packet->uiID, Packet_getMessageDLC(packet->index));

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
				bitmask = getBitmaskForCANmessage(bitPosition, length);
				//data = *((uint8_t*)(DL_GET_POINTER_TO_DATA(packet->psParameterList[j].eParam)));
				DL_getDataByComm((Packet_getMessageParameterList(packet->index) + j)->eParam, &data);
				priv_TxMessage.Data[byteIndex] |= (((((uint8_t)data) & 0xFF) << (8 - length - bitPosition)) & bitmask);
				break;
			case TypeU16:
			case TypeS16:
				/*involves two bytes */
				//data = *((uint16_t*)(DL_GET_POINTER_TO_DATA(packet->psParameterList[j].eParam)));
				DL_getDataByComm((Packet_getMessageParameterList(packet->index) + j)->eParam, &data);
				/*first byte, bit position is assumed to be 0 and the byte is fully for this parameter*/
				priv_TxMessage.Data[byteIndex] |= (uint8_t)((data >> (length - 8)) & 0xFF);
				/*second byte, bit position is still 0*/
				bitmask = getBitmaskForCANmessage(0, length - 8);
				priv_TxMessage.Data[byteIndex + 1] |= (uint8_t)(((data & 0xFF) << (16 - length)) & bitmask);
				break;
			case TypeU32:
			case TypeS32:
				break;
			default:
				break;
		}
	}
	/*send data*/
	CAN_Transmit(CAN1, &priv_TxMessage);
	// TODO good to check if CAN_Transmit() is returning CAN_TxStatus_NoMailBox
}
