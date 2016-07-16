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
	{ PacketMotor1Status, -1 },
	{ PacketMotor2Status, -1 },
	{ PacketRobotControl, -1 }
};

InterfaceTransmitPacket nodeInterfaceCANTransmitPacketList[4] =
{
	{ PacketMotorSpeeds,      -1,  -1 },
	{ PacketDistanceSensors, 100, 100 },
	{ PacketRobotFeedback,    -1,  -1 },
	{ PacketQuaternions,     100, 100 }
};

const InterfaceReceivePacket nodeInterfaceUART_RemoteReceivePacketList[3] = 
{
	{ PacketMotorSpeeds,  -1 },
	{ PacketRobotControl, -1 },
	{ PacketPidSetup,     -1 }
};

InterfaceTransmitPacket nodeInterfaceUART_RemoteTransmitPacketList[3] =
{
	{ PacketDistanceSensors, 100, 100 },
	{ PacketRobotFeedback,    -1, -1  },
	{ PacketQuaternions,     100, 100 }
};

const InterfaceReceivePacket nodeInterfaceUART_IMUReceivePacketList[2] = 
{
	{ PacketQuaternions, -1 },
	{ PacketGyro,        -1 }
};

NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces] =
{
	/* CAN         */ { nodeInterfaceCANReceivePacketList,         3, nodeInterfaceCANTransmitPacketList,         4 },
	/* UART_Remote */ { nodeInterfaceUART_RemoteReceivePacketList, 3, nodeInterfaceUART_RemoteTransmitPacketList, 3 },
	/* UART_IMU    */ { nodeInterfaceUART_IMUReceivePacketList,    2, 0,                                          0 }
};

