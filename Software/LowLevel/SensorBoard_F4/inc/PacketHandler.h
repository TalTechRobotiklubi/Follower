// ----------------------------------------------------------------------------
//
// Casecatcher generated packet configuration header file
//
// ----------------------------------------------------------------------------
#ifndef _PACKET_HANDLER_
#define _PACKET_HANDLER_

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "DataLayerConfig.h"
#include "PacketConfig.h"
//#include "stdint.h"

// ----------------------------------------------------------------------------
// Public types
// ----------------------------------------------------------------------------
typedef struct
{
	uint16_t uiID;
	int16_t  iperiod;
	//PacketParameter* psParameterList;
	//PacketDescriptor packets;
	//uint16_t uiParameterCount;
	//uint16_t uiDLC;
	Packet index; /*index in packet main table (psPacketDescriptorList)*/
}
PacketWithIndex;

/*statuses for packets, which are sent sporiadically*/
enum SPORIADIC_PACKET_STATUS
{
	PACKET_WAITING = -1,
	PACKET_READY_TO_SEND = -2,
	PACKET_RECEIVED = -3,
};

extern int16_t Packet_getMessagePeriod(uint8_t index);
extern uint16_t Packet_getMessageParameterCount(uint8_t index);
extern uint16_t Packet_getMessageDLC(uint8_t index);
extern const PacketParameter* Packet_getMessageParameterList(uint8_t index);

#if (ENABLE_CAN_INTERFACE == 1)
/*CAN*/
extern uint8_t Packet_getNumOfCANRxPackets(void);
extern PacketWithIndex* Packet_getCANRxPacketsData(void);
extern uint8_t Packet_getNumOfCANTxPackets(void);
extern PacketWithIndex* Packet_getCANTxPacketsData(void);
#endif

#if (ENABLE_UART_INTERFACE == 1)
/*UART*/
extern uint8_t Packet_getNumOfUARTRxPackets(void);
extern PacketWithIndex* Packet_getUARTRxPacketsData(void);
extern uint8_t Packet_getNumOfUARTTxPackets(void);
extern PacketWithIndex* Packet_getUARTTxPacketsData(void);
#endif

#if (ENABLE_GATEWAYING == 1)
/*Gateway*/
extern uint8_t Packet_getNumOfGatewayUARTtoCANPackets(void);
extern PacketWithIndex* Packet_getGatewayUARTtoCANPackets(void);
extern uint8_t Packet_getNumOfGatewayCANtoUARTPackets(void);
extern PacketWithIndex* Packet_getGatewayCANtoUARTPackets(void);
#endif

#endif
