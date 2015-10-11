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
const InterfaceReceivePacket psNodeInterfaceCANReceivePacketList[3] = 
{
	{ PacketMotor1Status,  -1 },
	{ PacketMotor2Status,  -1 },
	{ PacketTrackedObject, -1 }
};

const InterfaceTransmitPacket psNodeInterfaceCANTransmitPacketList[1] = 
{
	{ PacketMotorSpeeds, -1 }
};

const InterfaceReceivePacket psNodeInterfaceUART_RemoteReceivePacketList[1] = 
{
	{ PacketMotorSpeeds, -1 }
};

const InterfaceTransmitPacket psNodeInterfaceUART_RemoteTransmitPacketList[6] = 
{
	{ PacketDistanceSensors, 100 },
	{ PacketAcceleration,    100 },
	{ PacketMotor1Status,     -1 },
	{ PacketMotor2Status,     -1 },
	{ PacketTrackedObject,    -1 },
	{ PacketDepthRaw,         -1 }
};

const InterfaceReceivePacket psNodeInterfaceUART_ZotacReceivePacketList[3] = 
{
	{ PacketTrackedObject, -1 },
	{ PacketDepthRaw,      -1 },
	{ PacketMotorSpeeds,   -1 }
};

const InterfaceTransmitPacket psNodeInterfaceUART_ZotacTransmitPacketList[4] = 
{
	{ PacketMotor1Status,     -1 },
	{ PacketMotor2Status,     -1 },
	{ PacketDistanceSensors, 100 },
	{ PacketAcceleration,    100 }
};

const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces] = 
{
	/* CAN         */ { psNodeInterfaceCANReceivePacketList,         3, psNodeInterfaceCANTransmitPacketList,         1 },
	/* UART_Remote */ { psNodeInterfaceUART_RemoteReceivePacketList, 1, psNodeInterfaceUART_RemoteTransmitPacketList, 6 },
	/* UART_Zotac  */ { psNodeInterfaceUART_ZotacReceivePacketList,  3, psNodeInterfaceUART_ZotacTransmitPacketList,  4 }
};

