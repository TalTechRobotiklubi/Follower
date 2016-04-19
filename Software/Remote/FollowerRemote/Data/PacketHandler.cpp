// ----------------------------------------------------------------------------
//
// Casecatcher generated packet configuration source file
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "PacketHandler.h"


// ----------------------------------------------------------------------------
// UART Rx Packets table
// ----------------------------------------------------------------------------
PacketWithIndex psUARTRxPackets[] =
{
  /* DistanceSensors       */ { 0xF0, 0, PacketDistanceSensors },
  /* RobotFeedback         */ { 0xD4, 0, PacketRobotFeedback },
  /* DistanceSensors       */ { 0xF0, 0, PacketDistanceSensors },
  /* InertialMeasurement12 */ { 0xF2, 0, PacketQuaternions },
  /* CameraControl         */ { 0xE0, 0, PacketCameraControl }
};
#define NUMBER_OF_UART_RX_PACKETS (sizeof(psUARTRxPackets)/sizeof(PacketWithIndex))

// ----------------------------------------------------------------------------
// UART Tx Packets table
// ----------------------------------------------------------------------------
PacketWithIndex psUARTTxPackets[] =
{
  /* MotorSpeeds   */ { 0xD0, 15, PacketMotorSpeeds },
  /* RobotControl  */ { 0xD3, 15, PacketRobotControl },
  /* PidSetup      */ { 0xD5, -1, PacketPidSetup }
};
#define NUMBER_OF_UART_TX_PACKETS (sizeof(psUARTTxPackets)/sizeof(PacketWithIndex))


// ----------------------------------------------------------------------------
// Function definitions
// ----------------------------------------------------------------------------

/*Get message period, does not depend on packet type*/
int16_t PacketHandler::Packet_getMessagePeriod(uint16_t index)
{
    if ((index < NumberOfPackets) && (index >= 0))
	{
        return PacketDescriptorList[index].period;
	}
	else
	{
		return 0;
	}
}

/*Get number of parameters in packet*/
uint16_t PacketHandler::Packet_getMessageParameterCount(uint16_t index)
{
	if ((index < NumberOfPackets) && (index >= 0))
	{
    return PacketDescriptorList[index].parameterCount;
	}
	else
	{
		return 0;
	}
}

/*Get packet length*/
uint16_t PacketHandler::Packet_getMessageDLC(uint16_t index)
{
	if ((index < NumberOfPackets) && (index >= 0))
	{
    return PacketDescriptorList[index].dlc;
	}
	else
	{
		return 0;
	}
}

/*Gets the pointer to parameter list in packet*/
const PacketParameter* PacketHandler::Packet_getMessageParameterList(uint16_t index)
{
	if ((index < NumberOfPackets) && (index >= 0))
	{
    return PacketDescriptorList[index].parameterList;
	}
	else
	{
		return 0;
	}
}

/*UART message functions*/
uint8_t PacketHandler::Packet_getNumOfUARTRxPackets(void)
{
	return NUMBER_OF_UART_RX_PACKETS;
}

PacketWithIndex* PacketHandler::Packet_getUARTRxPacketsData(void)
{
	return psUARTRxPackets;
}

uint8_t PacketHandler::Packet_getNumOfUARTTxPackets(void)
{
	return NUMBER_OF_UART_TX_PACKETS;
}

PacketWithIndex* PacketHandler::Packet_getUARTTxPacketsData(void)
{
	return psUARTTxPackets;
}
