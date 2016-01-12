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
#include "typedef.h"
#include "packetconfig.h"

// ----------------------------------------------------------------------------
// Interfaces
// ----------------------------------------------------------------------------
typedef enum
{
	InterfaceUART,
	// Count of items is 1
	NumberOfInterfaces
}
Interface;

// ----------------------------------------------------------------------------
// Interface structures
// ----------------------------------------------------------------------------
typedef struct
{
	Packet ePacket;
	int32_t ulPeriod;
}
InterfaceReceivePacket;

typedef struct
{
	Packet ePacket;
	int32_t ulPeriod;
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

