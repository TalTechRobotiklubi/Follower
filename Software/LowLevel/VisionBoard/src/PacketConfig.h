// ----------------------------------------------------------------------------
//
// Casecatcher generated packet configuration header file
//
// ----------------------------------------------------------------------------
#ifndef _PACKET_CONFIG_
#define _PACKET_CONFIG_

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "Typedef.h"
#include "DataLayerConfig.h"

#define ENABLE_CAN_INTERFACE  1

// ----------------------------------------------------------------------------
// Packets enumeration
// ----------------------------------------------------------------------------
typedef enum
{
	PacketTrackedObject, // 0
	// Count of items is 1
	NumberOfPackets
}
Packet;

// ----------------------------------------------------------------------------
// Packet structures
// ----------------------------------------------------------------------------
typedef struct
{
	DLParam eParam;
	uint16_t uiStartBit;
	uint16_t uiLengthBits;
}
PacketParameter;

typedef struct
{
	uint16_t uiID;
	int16_t  iPeriod;
	PacketParameter const * psParameterList;
	uint16_t uiParameterCount;
	uint16_t uiDLC;
}
PacketDescriptor;

// ----------------------------------------------------------------------------
// Export packet descriptors
// ----------------------------------------------------------------------------
extern const PacketDescriptor psPacketDescriptorList[NumberOfPackets];

#endif

