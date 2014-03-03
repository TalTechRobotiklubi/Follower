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
	/* Motor1Status    */ { 0xD1, 0, PacketMotor1Status },
	/* Motor2Status    */ { 0xD2, 0, PacketMotor2Status },
	/* Motor3Status    */ { 0xD3, 0, PacketMotor3Status },
	/* DistanceSensors */ { 0xF0, 0, PacketDistanceSensors },
    /* Acceleration    */ { 0xF1, 0, PacketAcceleration }
};
#define NUMBER_OF_UART_RX_PACKETS (sizeof(psUARTRxPackets)/sizeof(PacketWithIndex))

// ----------------------------------------------------------------------------
// UART Tx Packets table
// ----------------------------------------------------------------------------
PacketWithIndex psUARTTxPackets[] =
{
    /* MotorSpeeds   */ { 0xD0,  15, PacketMotorSpeeds }
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
        return psPacketDescriptorList[index].iPeriod;
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
		return psPacketDescriptorList[index].uiParameterCount;
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
		return psPacketDescriptorList[index].uiDLC;
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
		return psPacketDescriptorList[index].psParameterList;
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
