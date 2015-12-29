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

const InterfaceReceivePacket psNodeInterfaceUSBReceivePacketList[2] = 
{
	{ PacketMotorSpeeds,   -1 },
	{ PacketCameraControl, -1 }
};

const InterfaceTransmitPacket psNodeInterfaceUSBTransmitPacketList[4] = 
{
	{ PacketMotor1Status,    -1 },
	{ PacketMotor2Status,    -1 },
	{ PacketDistanceSensors, -1 },
	{ PacketAcceleration,    -1 }
};

const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces] = 
{
	/* CAN */ { psNodeInterfaceCANReceivePacketList, 4, psNodeInterfaceCANTransmitPacketList, 1 },
	/* USB */ { psNodeInterfaceUSBReceivePacketList, 2, psNodeInterfaceUSBTransmitPacketList, 4 }
};

