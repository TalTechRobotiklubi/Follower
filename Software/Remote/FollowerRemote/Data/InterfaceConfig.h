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
	Packet packet;
	int16_t period;
}
InterfaceReceivePacket;

typedef struct
{
	Packet packet;
	int16_t period;
	int16_t elapsed;
}
InterfaceTransmitPacket;

typedef struct
{
	InterfaceReceivePacket const * receivePacketList;
	uint32_t receivePacketCount;
	InterfaceTransmitPacket* transmitPacketList;
	uint32_t transmitPacketCount;
}
NodeInterfaceDescriptor;

// ----------------------------------------------------------------------------
// Export interface descriptors
// ----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
extern NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces];
}
#endif

#endif

