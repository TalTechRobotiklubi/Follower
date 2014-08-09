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
const InterfaceReceivePacket psNodeInterfaceUART_CANReceivePacketList[2] = 
{
	{ PacketMotorSpeeds, -1 },
	{ PacketMotorPID,    -1 }
};

const InterfaceTransmitPacket psNodeInterfaceUART_CANTransmitPacketList[2] = 
{
	{ PacketMotorSpeeds, -1 },
	{ PacketMotorPID,    -1 }
};

const InterfaceReceivePacket psNodeInterfaceCAN_UARTReceivePacketList[3] = 
{
	{ PacketMotor1Status,  -1 },
	{ PacketMotor2Status,  -1 },
	{ PacketTrackedObject, -1 }
};

const InterfaceTransmitPacket psNodeInterfaceCAN_UARTTransmitPacketList[3] = 
{
	{ PacketMotor1Status,  -1 },
	{ PacketMotor2Status,  -1 },
	{ PacketTrackedObject, -1 }
};

const InterfaceTransmitPacket psNodeInterfaceUARTTransmitPacketList[2] = 
{
	{ PacketDistanceSensors, 100 },
	{ PacketAcceleration,    100 }
};

const NodeInterfaceDescriptor psInterfaceList[NumberOfInterfaces] = 
{
	/* UART_CAN */ { psNodeInterfaceUART_CANReceivePacketList, 2, psNodeInterfaceUART_CANTransmitPacketList, 2 },
	/* CAN_UART */ { psNodeInterfaceCAN_UARTReceivePacketList, 3, psNodeInterfaceCAN_UARTTransmitPacketList, 3 },
	/* UART     */ { 0,                                        0, psNodeInterfaceUARTTransmitPacketList,     2 }
};

