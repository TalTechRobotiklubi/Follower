// ----------------------------------------------------------------------------
//
// Casecatcher generated node interfaces header file
//
// ----------------------------------------------------------------------------
#ifndef _INTERFACE_CONFIG_
#define _INTERFACE_CONFIG_

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "Typedef.h"
#include "PacketConfig.h"

// ----------------------------------------------------------------------------
// Interfaces
// ----------------------------------------------------------------------------
typedef enum
{
	InterfaceCAN,       
	InterfaceUART_Zotac,
	// Count of items is 2
	NumberOfInterfaces
}
Interface;

// ----------------------------------------------------------------------------
// Interface structures
// ----------------------------------------------------------------------------
typedef struct
{
	Packet ePacket;
	int16_t ulPeriod;
}
InterfaceReceivePacket;

typedef struct
{
	Packet ePacket;
	int16_t ulPeriod;
}
InterfaceTransmitPacket;

typedef struct
{
	InterfaceReceivePacket const * psReceivePacketList;
	uint32_t uiReceivePacketCount;
	InterfaceTransmitPacket const * psTransmitPacketList;
	uint32_t uiTransmitPacketCount;
}
NodeInterfaceDescriptor;

// ----------------------------------------------------------------------------
// Export interface descriptors
// ----------------------------------------------------------------------------
extern const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces];

#endif

