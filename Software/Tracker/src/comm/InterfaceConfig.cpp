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
InterfaceReceivePacket nodeInterfaceUSBReceivePacketList[3] = 
{
	{ &PacketDescriptorList[PacketRobotFeedback],   -1 },
	{ &PacketDescriptorList[PacketDistanceSensors], -1 },
	{ &PacketDescriptorList[PacketQuaternions],     -1 }
};

InterfaceTransmitPacket nodeInterfaceUSBTransmitPacketList[4] = 
{
	{ &PacketDescriptorList[PacketMotorSpeeds],   -1, -1 },
	{ &PacketDescriptorList[PacketCameraControl], 20, 20 },
	{ &PacketDescriptorList[PacketRobotControl],  20, 20 },
	{ &PacketDescriptorList[PacketVisualization], 20, 20 }
};

NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces] = 
{
	/* USB */ { nodeInterfaceUSBReceivePacketList, 3, nodeInterfaceUSBTransmitPacketList, 4 }
};

