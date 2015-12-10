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
const InterfaceReceivePacket psNodeInterfaceCANReceivePacketList[4] = 
{
	{ PacketMotor1Status,    -1 },
	{ PacketMotor2Status,    -1 },
	{ PacketDistanceSensors, -1 },
	{ PacketAcceleration,    -1 }
};

const InterfaceTransmitPacket psNodeInterfaceCANTransmitPacketList[1] = 
{
	{ PacketMotorSpeeds, -1 }
};

const InterfaceReceivePacket psNodeInterfaceUART_ZotacReceivePacketList[1] = 
{
	{ PacketMotorSpeeds, -1 }
};

const InterfaceTransmitPacket psNodeInterfaceUART_ZotacTransmitPacketList[4] = 
{
	{ PacketMotor1Status,    -1 },
	{ PacketMotor2Status,    -1 },
	{ PacketDistanceSensors, -1 },
	{ PacketAcceleration,    -1 }
};

const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces] = 
{
	/* CAN        */ { psNodeInterfaceCANReceivePacketList,        4, psNodeInterfaceCANTransmitPacketList,        1 },
	/* UART_Zotac */ { psNodeInterfaceUART_ZotacReceivePacketList, 1, psNodeInterfaceUART_ZotacTransmitPacketList, 4 }
};

