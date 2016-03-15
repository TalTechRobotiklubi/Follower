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

const InterfaceTransmitPacket nodeInterfaceCANTransmitPacketList[5] = 
{
	{ PacketMotorSpeeds,            -1 },
	{ PacketDistanceSensors,       100 },
	{ PacketRobotFeedback,          20 },
	{ PacketInertialMeasurement12, 100 },
	{ PacketInertialMeasurement34, 100 }
};

const InterfaceReceivePacket nodeInterfaceUART_RemoteReceivePacketList[3] = 
{
	{ PacketMotorSpeeds,  -1 },
	{ PacketRobotControl, -1 },
	{ PacketPidSetup,     -1 }
};

const InterfaceTransmitPacket nodeInterfaceUART_RemoteTransmitPacketList[4] = 
{
	{ PacketDistanceSensors,       100 },
	{ PacketRobotFeedback,          20 },
	{ PacketInertialMeasurement12, 100 },
	{ PacketInertialMeasurement34, 100 }
};

const NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces] = 
{
	/* CAN         */ { nodeInterfaceCANReceivePacketList,         3, nodeInterfaceCANTransmitPacketList,         5 },
	/* UART_Remote */ { nodeInterfaceUART_RemoteReceivePacketList, 3, nodeInterfaceUART_RemoteTransmitPacketList, 4 },
	/* UART_IMU    */ { 0,                                         0, 0,                                          0 }
};

