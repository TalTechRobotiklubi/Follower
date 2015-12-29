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
const InterfaceReceivePacket psNodeInterfaceCANReceivePacketList[2] = 
{
	{ PacketMotor1Status, -1 },
	{ PacketMotor2Status, -1 }
};

const InterfaceTransmitPacket psNodeInterfaceCANTransmitPacketList[3] = 
{
	{ PacketMotorSpeeds,      -1 },
	{ PacketDistanceSensors, 100 },
	{ PacketAcceleration,    100 }
};

const InterfaceReceivePacket psNodeInterfaceUART_RemoteReceivePacketList[1] = 
{
	{ PacketMotorSpeeds, -1 }
};

const InterfaceTransmitPacket psNodeInterfaceUART_RemoteTransmitPacketList[4] = 
{
	{ PacketDistanceSensors, 100 },
	{ PacketAcceleration,    100 },
	{ PacketMotor1Status,     -1 },
	{ PacketMotor2Status,     -1 }
};

const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces] = 
{
	/* CAN         */ { psNodeInterfaceCANReceivePacketList,         2, psNodeInterfaceCANTransmitPacketList,         3 },
	/* UART_Remote */ { psNodeInterfaceUART_RemoteReceivePacketList, 1, psNodeInterfaceUART_RemoteTransmitPacketList, 4 }
};

