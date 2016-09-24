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
InterfaceReceivePacket nodeInterfaceCANReceivePacketList[4] = 
{
	{ &PacketDescriptorList[PacketMotor1Status], -1 },
	{ &PacketDescriptorList[PacketMotor2Status], -1 },
	{ &PacketDescriptorList[PacketRobotControl], -1 },
	{ &PacketDescriptorList[PacketPidSetup],     -1 }
};

InterfaceTransmitPacket nodeInterfaceCANTransmitPacketList[4] = 
{
	{ &PacketDescriptorList[PacketMotorSpeeds],      -1,  -1 },
	{ &PacketDescriptorList[PacketDistanceSensors], 100, 100 },
	{ &PacketDescriptorList[PacketRobotFeedback],    -1,  -1 },
	{ &PacketDescriptorList[PacketQuaternions],     100, 100 }
};

InterfaceReceivePacket nodeInterfaceUART_RemoteReceivePacketList[3] = 
{
	{ &PacketDescriptorList[PacketMotorSpeeds],  -1 },
	{ &PacketDescriptorList[PacketRobotControl], -1 },
	{ &PacketDescriptorList[PacketPidSetup],     -1 }
};

InterfaceTransmitPacket nodeInterfaceUART_RemoteTransmitPacketList[3] = 
{
	{ &PacketDescriptorList[PacketDistanceSensors], 100, 100 },
	{ &PacketDescriptorList[PacketRobotFeedback],    -1,  -1 },
	{ &PacketDescriptorList[PacketQuaternions],     100, 100 }
};

InterfaceReceivePacket nodeInterfaceUART_IMUReceivePacketList[2] = 
{
	{ &PacketDescriptorList[PacketQuaternions], -1 },
	{ &PacketDescriptorList[PacketGyro],        -1 }
};

NodeInterfaceDescriptor InterfaceList[NumberOfInterfaces] = 
{
	/* CAN         */ { nodeInterfaceCANReceivePacketList,         4, nodeInterfaceCANTransmitPacketList,         4 },
	/* UART_Remote */ { nodeInterfaceUART_RemoteReceivePacketList, 3, nodeInterfaceUART_RemoteTransmitPacketList, 3 },
	/* UART_IMU    */ { nodeInterfaceUART_IMUReceivePacketList,    2, 0,                                          0 }
};

