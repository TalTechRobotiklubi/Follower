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

const InterfaceReceivePacket psNodeInterfaceUARTtoRemoteReceivePacketList[2] = 
{
	{ PacketMotorSpeeds, -1 },
	{ PacketMotorPID,    -1 }
};

const InterfaceTransmitPacket psNodeInterfaceUARTtoRemoteTransmitPacketList[6] = 
{
	{ PacketDistanceSensors, 100 },
	{ PacketAcceleration,    100 },
	{ PacketMotor1Status,     -1 },
	{ PacketMotor2Status,     -1 },
	{ PacketTrackedObject,    -1 },
	{ PacketDepthArray,       -1 }
};

const InterfaceReceivePacket psNodeInterfaceUARTtoKinectReceivePacketList[2] = 
{
	{ PacketTrackedObject, -1 },
	{ PacketDepthArray,    -1 }
};

const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces] = 
{
	/* CAN          */ { psNodeInterfaceCANReceivePacketList,          3, psNodeInterfaceCANTransmitPacketList,          2 },
	/* UARTtoRemote */ { psNodeInterfaceUARTtoRemoteReceivePacketList, 2, psNodeInterfaceUARTtoRemoteTransmitPacketList, 6 },
	/* UARTtoKinect */ { psNodeInterfaceUARTtoKinectReceivePacketList, 2, 0,                                             0 }
};

