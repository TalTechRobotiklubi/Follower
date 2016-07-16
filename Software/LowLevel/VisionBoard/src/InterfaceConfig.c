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
	{ PacketRobotFeedback,   -1 },
	{ PacketDistanceSensors, -1 },
	{ PacketQuaternions,     -1 }
};

const InterfaceTransmitPacket nodeInterfaceCANTransmitPacketList[2] = 
{
	{ PacketMotorSpeeds,  -1 },
	{ PacketRobotControl, -1 }
};

const InterfaceReceivePacket nodeInterfaceUSBReceivePacketList[3] = 
{
	{ PacketMotorSpeeds,   -1 },
	{ PacketCameraControl, -1 },
	{ PacketRobotControl,  -1 }
};

const InterfaceTransmitPacket nodeInterfaceUSBTransmitPacketList[3] = 
{
	{ PacketDistanceSensors, -1 },
	{ PacketRobotFeedback,   -1 },
	{ PacketQuaternions,     -1 }
};

const NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces] = 
{
	/* CAN */ { nodeInterfaceCANReceivePacketList, 3, nodeInterfaceCANTransmitPacketList, 2 },
	/* USB */ { nodeInterfaceUSBReceivePacketList, 3, nodeInterfaceUSBTransmitPacketList, 3 }
};

