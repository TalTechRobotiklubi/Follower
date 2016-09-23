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
const InterfaceReceivePacket nodeInterfaceUARTReceivePacketList[3] = 
{
	{ PacketRobotFeedback,   -1 },
	{ PacketDistanceSensors, -1 },
	{ PacketQuaternions,     -1 }
};

InterfaceTransmitPacket nodeInterfaceUARTTransmitPacketList[4] = 
{
  { PacketMotorSpeeds,   -1, -1 },
  { PacketRobotControl,  20, 20 },
	{ PacketPidSetup,      -1, -1 },
	{ PacketCameraControl, -1, -1 }
};

NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces] = 
{
	/* UART */ { nodeInterfaceUARTReceivePacketList, 3, nodeInterfaceUARTTransmitPacketList, 4 }
};

