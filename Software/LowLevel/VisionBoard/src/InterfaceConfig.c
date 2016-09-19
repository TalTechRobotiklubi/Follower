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
const InterfaceReceivePacket nodeInterfaceCANReceivePacketList[3] = 
{
	{ &PacketDescriptorList[PacketRobotFeedback],   -1 },
	{ &PacketDescriptorList[PacketDistanceSensors], -1 },
	{ &PacketDescriptorList[PacketQuaternions],     -1 }
};

InterfaceTransmitPacket nodeInterfaceCANTransmitPacketList[2] = 
{
	{ &PacketDescriptorList[PacketMotorSpeeds],  -1, -1 },
	{ &PacketDescriptorList[PacketRobotControl], -1, -1 }
};

const InterfaceReceivePacket nodeInterfaceUSBReceivePacketList[3] = 
{
	{ &PacketDescriptorList[PacketMotorSpeeds],   -1 },
	{ &PacketDescriptorList[PacketCameraControl], -1 },
	{ &PacketDescriptorList[PacketRobotControl],  -1 }
};

InterfaceTransmitPacket nodeInterfaceUSBTransmitPacketList[3] = 
{
	{ &PacketDescriptorList[PacketDistanceSensors], -1, -1 },
	{ &PacketDescriptorList[PacketRobotFeedback],   -1, -1 },
	{ &PacketDescriptorList[PacketQuaternions],     -1, -1 }
};

NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces] = 
{
	/* CAN */ { nodeInterfaceCANReceivePacketList, 3, nodeInterfaceCANTransmitPacketList, 2 },
	/* USB */ { nodeInterfaceUSBReceivePacketList, 3, nodeInterfaceUSBTransmitPacketList, 3 }
};

