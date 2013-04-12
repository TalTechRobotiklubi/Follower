
#pragma once

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "DataLayerConfig.h"
#include "PacketConfig.h"
#include "stdint.h"

// ----------------------------------------------------------------------------
// Public types
// ----------------------------------------------------------------------------
typedef struct
{
	uint16_t uiID;
	int16_t  iperiod;
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

class PacketHandler
{
public:
    int16_t                 Packet_getMessagePeriod(uint16_t index);
    uint16_t                Packet_getMessageParameterCount(uint16_t index);
    uint16_t                Packet_getMessageDLC(uint16_t index);
    const PacketParameter*  Packet_getMessageParameterList(uint16_t index);

/*UART*/
    uint8_t                 Packet_getNumOfUARTRxPackets(void);
    PacketWithIndex*        Packet_getUARTRxPacketsData(void);
    uint8_t                 Packet_getNumOfUARTTxPackets(void);
    PacketWithIndex*        Packet_getUARTTxPacketsData(void);

private: 
//    PacketConfig*           packetConfig_;
};

