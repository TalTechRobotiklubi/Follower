// ----------------------------------------------------------------------------
//
// Casecatcher generated node interfaces configuration source file
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "interfaceconfig.h"

// ----------------------------------------------------------------------------
// Interface tables
// ----------------------------------------------------------------------------
const InterfaceReceivePacket psNodeInterfaceUARTReceivePacketList[4] = 
{
	{ PacketMotor1Status,    -1 },
	{ PacketMotor2Status,    -1 },
	{ PacketDistanceSensors, -1 },
	{ PacketAcceleration,    -1 }
};

const InterfaceTransmitPacket psNodeInterfaceUARTTransmitPacketList[2] = 
{
	{ PacketMotorSpeeds,   20 },
	{ PacketCameraControl, 20 }
};

const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces] = 
{
	/* UART */ { psNodeInterfaceUARTReceivePacketList, 4, psNodeInterfaceUARTTransmitPacketList, 2 }
};

