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
InterfaceTransmitPacket nodeInterfaceUARTTransmitPacketList[2] = 
{
	{ &PacketDescriptorList[PacketEmergency],    -1, -1 },
	{ &PacketDescriptorList[PacketRobotControl], -1, -1 }
};

NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces] = 
{
	/* UART */ { 0, 0, nodeInterfaceUARTTransmitPacketList, 2 }
};

