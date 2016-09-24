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
InterfaceReceivePacket nodeInterfaceCANReceivePacketList[3] = 
{
	{ &PacketDescriptorList[PacketRobotFeedback],   -1 },
	{ &PacketDescriptorList[PacketDistanceSensors], -1 },
	{ &PacketDescriptorList[PacketQuaternions],     -1 }
};

InterfaceTransmitPacket nodeInterfaceCANTransmitPacketList[3] = 
{
	{ &PacketDescriptorList[PacketMotorSpeeds],  -1, -1 },
	{ &PacketDescriptorList[PacketRobotControl], -1, -1 },
	{ &PacketDescriptorList[PacketPidSetup],     -1, -1 }
};

InterfaceReceivePacket nodeInterfaceUSBReceivePacketList[4] = 
{
	{ &PacketDescriptorList[PacketMotorSpeeds],   -1 },
	{ &PacketDescriptorList[PacketCameraControl], -1 },
	{ &PacketDescriptorList[PacketRobotControl],  -1 },
	{ &PacketDescriptorList[PacketPidSetup],      -1 }
};

InterfaceTransmitPacket nodeInterfaceUSBTransmitPacketList[3] = 
{
	{ &PacketDescriptorList[PacketDistanceSensors], -1, -1 },
	{ &PacketDescriptorList[PacketRobotFeedback],   -1, -1 },
	{ &PacketDescriptorList[PacketQuaternions],     -1, -1 }
};

NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces] = 
{
	/* CAN */ { nodeInterfaceCANReceivePacketList, 3, nodeInterfaceCANTransmitPacketList, 3 },
	/* USB */ { nodeInterfaceUSBReceivePacketList, 4, nodeInterfaceUSBTransmitPacketList, 3 }
};

