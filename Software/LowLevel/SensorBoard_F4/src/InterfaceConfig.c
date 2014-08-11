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

const InterfaceTransmitPacket psNodeInterfaceCANTransmitPacketList[2] = 
{
	{ PacketMotorSpeeds, -1 },
	{ PacketMotorPID,    -1 }
};

const InterfaceReceivePacket psNodeInterfaceUARTReceivePacketList[2] = 
{
	{ PacketMotorSpeeds, -1 },
	{ PacketMotorPID,    -1 }
};

const InterfaceTransmitPacket psNodeInterfaceUARTTransmitPacketList[5] = 
{
	{ PacketDistanceSensors, 100 },
	{ PacketAcceleration,    100 },
	{ PacketMotor1Status,     -1 },
	{ PacketMotor2Status,     -1 },
	{ PacketTrackedObject,    -1 }
};

const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces] = 
{
	/* CAN  */ { psNodeInterfaceCANReceivePacketList,  3, psNodeInterfaceCANTransmitPacketList,  2 },
	/* UART */ { psNodeInterfaceUARTReceivePacketList, 2, psNodeInterfaceUARTTransmitPacketList, 5 }
};

