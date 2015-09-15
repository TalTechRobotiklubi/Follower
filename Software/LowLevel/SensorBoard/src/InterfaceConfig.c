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

const InterfaceReceivePacket psNodeInterfaceUART_RemoteReceivePacketList[2] = 
{
	{ PacketMotorSpeeds, -1 },
	{ PacketMotorPID,    -1 }
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

const InterfaceReceivePacket psNodeInterfaceUART_KinectReceivePacketList[2] = 
{
	{ PacketTrackedObject, -1 },
	{ PacketDepthRaw,      -1 }
};

const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces] = 
{
	/* CAN         */ { psNodeInterfaceCANReceivePacketList,         3, psNodeInterfaceCANTransmitPacketList,         2 },
	/* UART_Remote */ { psNodeInterfaceUART_RemoteReceivePacketList, 2, psNodeInterfaceUART_RemoteTransmitPacketList, 6 },
	/* UART_Kinect */ { psNodeInterfaceUART_KinectReceivePacketList, 2, 0,                                            0 }
};

