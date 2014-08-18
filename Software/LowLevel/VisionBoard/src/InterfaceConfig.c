// ----------------------------------------------------------------------------
//
// Casecatcher generated node interfaces configuration source file
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "InterfaceConfig.h"

// ----------------------------------------------------------------------------
// Interface tables
// ----------------------------------------------------------------------------
const InterfaceTransmitPacket psNodeInterfaceCANTransmitPacketList[1] = 
{
	{ PacketTrackedObject, 20 }
};

const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces] = 
{
	/* CAN */ { 0, 0, psNodeInterfaceCANTransmitPacketList, 1 }
};

