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
	InterfaceUART_Remote,
	InterfaceUART_IMU,   
	// Count of items is 3
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
}
InterfaceTransmitPacket;

typedef struct
{
	InterfaceReceivePacket const * receivePacketList;
	uint32_t receivePacketCount;
	InterfaceTransmitPacket const * transmitPacketList;
	uint32_t transmitPacketCount;
}
NodeInterfaceDescriptor;

// ----------------------------------------------------------------------------
// Export interface descriptors
// ----------------------------------------------------------------------------
extern const NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces];

#endif

