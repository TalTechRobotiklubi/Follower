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

InterfaceTransmitPacket nodeInterfaceUSBTransmitPacketList[3] = 
{
	{ &PacketDescriptorList[PacketMotorSpeeds],   20, 20 },
	{ &PacketDescriptorList[PacketCameraControl], 20, 20 },
	{ &PacketDescriptorList[PacketRobotControl],  20, 20 }
};

NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces] = 
{
	/* USB */ { nodeInterfaceUSBReceivePacketList, 3, nodeInterfaceUSBTransmitPacketList, 3 }
};

