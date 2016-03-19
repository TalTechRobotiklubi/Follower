#include "CAN.h"
#include "DataLayer.h"
#include "MessageId.h"

#define ABS(A)	((A)<(0) ? (-A) : (A))			//!< Calculateas absolute value of a value
#define RX_MESSAGE_DLC  		4
#define TX_MESSAGE_DLC  		7
#define QEI_TX_MESSAGE_DLC		2
#define RX_MESSAGE_ID   		0xD0
#define MEM_ACCESS_ID			0xD4
#define MEM_MESSAGE_DLC  		8

static CanRxMsg priv_RxMessage;
static CanTxMsg priv_TxMessage;
static int16_t priv_requestedSpeed = 0;
static int16_t priv_timeoutCounter = 0;

static void initializeRxMessage(CanRxMsg *priv_RxMessage);
static void handleReceivedData();
static void handleTransmitData();

void initializeRxMessage(CanRxMsg *priv_RxMessage)
{
  uint8_t i = 0;

  priv_RxMessage->StdId = 0;
  priv_RxMessage->ExtId = 0;
  priv_RxMessage->IDE = CAN_ID_STD;
  priv_RxMessage->DLC = 0;
  priv_RxMessage->FMI = 0;
  for (i = 0; i < RX_MESSAGE_DLC; i++)
  {
    priv_RxMessage->Data[i] = 0;
  }
}

void CAN_init(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef       NVIC_InitStructure;
	uint8_t 			   i;

	/* Configure CAN1 and CAN2 IOs **********************************************/
	/* GPIOB, GPIOD and AFIO clocks enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure CAN RX pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure CAN TX pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Remap CAN GPIOs */
	//GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

	/* Configure CAN1 ***********************************************************/
	/* CAN1 Periph clocks enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	/* CAN1 register init */
	CAN_DeInit(CAN1);

	/* Struct init*/
	CAN_StructInit(&CAN_InitStructure);

	/* CAN1 cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;

	#if CAN_BAUDRATE == 1000 /* 1MBps */
	CAN_InitStructure.CAN_Prescaler =6;
	#elif CAN_BAUDRATE == 500 /* 500KBps */
	CAN_InitStructure.CAN_Prescaler =12;
	#elif CAN_BAUDRATE == 250 /* 250KBps */
	CAN_InitStructure.CAN_Prescaler =24;
	#elif CAN_BAUDRATE == 125 /* 125KBps */
	CAN_InitStructure.CAN_Prescaler =48;
	#elif  CAN_BAUDRATE == 100 /* 100KBps */
	CAN_InitStructure.CAN_Prescaler =60;
	#elif  CAN_BAUDRATE == 50 /* 50KBps */
	CAN_InitStructure.CAN_Prescaler =120;
	#elif  CAN_BAUDRATE == 20 /* 20KBps */
	CAN_InitStructure.CAN_Prescaler =300;
	#elif  CAN_BAUDRATE == 10 /* 10KBps */
	CAN_InitStructure.CAN_Prescaler =600;
	#else
	//#error "Please select first the CAN Baudrate in Private defines in main.c "
	#endif  /* CAN_BAUDRATE == 1000 */


	CAN_InitStructure.CAN_Prescaler = 32;

	/*Initializes the CAN1 */
	CAN_Init(CAN1, &CAN_InitStructure);

	/* CAN1 filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	/*Status transmit message*/
	priv_TxMessage.StdId = CurrentId;
	//priv_TxMessage.StdId = 0xD1;
	priv_TxMessage.RTR = CAN_RTR_DATA;
	priv_TxMessage.IDE = CAN_ID_STD;
	priv_TxMessage.DLC = TX_MESSAGE_DLC;
	for (i = 0; i < TX_MESSAGE_DLC; i++)
    {
	   priv_TxMessage.Data[i] = 0;
    }
	
	initializeRxMessage(&priv_RxMessage);

	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
  CAN_Receive(CAN1, CAN_FIFO0, &priv_RxMessage);

  if ((priv_RxMessage.IDE == CAN_ID_STD) &&
	  (priv_RxMessage.StdId == RX_MESSAGE_ID) &&
      (priv_RxMessage.DLC == RX_MESSAGE_DLC))
  {
	  if(CurrentId == Motor1Message)
	  {
		  priv_requestedSpeed = (int16_t)(((uint16_t)priv_RxMessage.Data[0] << 8) | (uint16_t)priv_RxMessage.Data[1]);
		  priv_timeoutCounter = 0;
	  }
	  else if(CurrentId == Motor2Message)
	  {
		  priv_requestedSpeed = (int16_t)(((uint16_t)priv_RxMessage.Data[2] << 8) | (uint16_t)priv_RxMessage.Data[3]);
		  priv_timeoutCounter = 0;
	  }
  }

  if ((priv_RxMessage.IDE == CAN_ID_STD) &&
	  (priv_RxMessage.StdId == MEM_ACCESS_ID) &&
      (priv_RxMessage.DLC == MEM_MESSAGE_DLC))
  {
	  int8_t count;
	  uint32_t u32Value = 0;
	  uint32_t u32Addr = 0;
	  for(count = 3; count >= 0; count--)
	  {
		  u32Addr |= priv_RxMessage.Data[3 - count] << (count * 8);
	  }
	  for(count = 3; count >= 0; count--)
	  {
		  u32Value |= priv_RxMessage.Data[7 - count] << (count * 8);
	  }
  }
}

void CAN_task()
{
	handleReceivedData();
	handleTransmitData();
}

void handleReceivedData()
{
	int16_t requestSpeed = priv_requestedSpeed;

	// increase timeout counter for requested speed. If no new message received during 10 cycles
	// for requested speed then stop motors
	priv_timeoutCounter++;

	if (priv_timeoutCounter >= 10)
	{
		priv_timeoutCounter = 10;  // to prevent overflow
		requestSpeed = 0;
	}

	if (CurrentId == Motor1Message)
		DL_setDataWithoutAffectingStatus(DLParamMotor1RequestSpeed, &requestSpeed);
	else if (CurrentId == Motor2Message)
		DL_setDataWithoutAffectingStatus(DLParamMotor2RequestSpeed, &requestSpeed);
}

void handleTransmitData()
{
	int16_t speed = 0;
	int16_t encoder = 0;
	uint8_t current = 0;
	uint8_t temp = 0;

	if (CurrentId == Motor1Message)
	{
		DL_getDataWithoutAffectingStatus(DLParamMotor1ActualSpeed, &speed);
		DL_getDataWithoutAffectingStatus(DLParamMotor1EncoderClicks, &encoder);
		DL_getDataWithoutAffectingStatus(DLParamMotor1CurrentDraw, &current);
		DL_getDataWithoutAffectingStatus(DLParamMotor1DriverTemp, &temp);
	}
	else if (CurrentId == Motor2Message)
	{
		DL_getDataWithoutAffectingStatus(DLParamMotor2ActualSpeed, &speed);
		DL_getDataWithoutAffectingStatus(DLParamMotor2EncoderClicks, &encoder);
		DL_getDataWithoutAffectingStatus(DLParamMotor2CurrentDraw, &current);
		DL_getDataWithoutAffectingStatus(DLParamMotor2DriverTemp, &temp);
	}

	priv_TxMessage.Data[0] = (int8_t)(speed >> 8);
    priv_TxMessage.Data[1] = (int8_t)(speed & 0xFF);
    priv_TxMessage.Data[2] = (uint8_t)(encoder >> 8);
    priv_TxMessage.Data[3] = (uint8_t)(encoder & 0xFF);
    priv_TxMessage.Data[4] = current;
    priv_TxMessage.Data[5] = temp;

	CAN_Transmit(CAN1, &priv_TxMessage);

	encoder = 0;
	DL_setDataWithoutAffectingStatus(DLParamMotor1EncoderClicks, &encoder);
	DL_setDataWithoutAffectingStatus(DLParamMotor2EncoderClicks, &encoder);
}
