// ----------------------------------------------------------------------------
//
// Casecatcher generated packet configuration source file
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "PacketHandler.h"

#if (ENABLE_CAN_INTERFACE == 1)
// ----------------------------------------------------------------------------
// CAN Rx Packets table
// ----------------------------------------------------------------------------
PacketWithIndex psCANRxPackets[] =
{

};
#define NUMBER_OF_CAN_RX_PACKETS (sizeof(psCANRxPackets)/sizeof(PacketWithIndex))

// ----------------------------------------------------------------------------
// CAN Tx Packets table
// ----------------------------------------------------------------------------
PacketWithIndex psCANTxPackets[] =
{

};
#define NUMBER_OF_CAN_TX_PACKETS (sizeof(psCANTxPackets)/sizeof(PacketWithIndex))
#endif

#if (ENABLE_UART_INTERFACE == 1)
// ----------------------------------------------------------------------------
// UART Rx Packets table
// ----------------------------------------------------------------------------
PacketWithIndex psUARTRxPackets[] =
{

};
#define NUMBER_OF_UART_RX_PACKETS (sizeof(psUARTRxPackets)/sizeof(PacketWithIndex))

// ----------------------------------------------------------------------------
// UART Tx Packets table
// ----------------------------------------------------------------------------
PacketWithIndex psUARTTxPackets[] =
{
	/* Sensors       */ { 0xF0, -1, PacketDistanceSensors }
};
#define NUMBER_OF_UART_TX_PACKETS (sizeof(psUARTTxPackets)/sizeof(PacketWithIndex))

#endif

#if (ENABLE_GATEWAYING == 1)
// ----------------------------------------------------------------------------
// Gateway Rx Packets table
// ----------------------------------------------------------------------------
PacketWithIndex psGatewayCANtoUARTPackets[] =
{
	/* Motor1Status  */ { 0xD1, -1, PacketMotor1Status },
	/* Motor2Status  */ { 0xD2, -1, PacketMotor2Status },
	/* Motor3Status  */ { 0xD3, -1, PacketMotor3Status }
};
#define NUMBER_OF_CAN_TO_UART_PACKETS (sizeof(psGatewayCANtoUARTPackets)/sizeof(PacketWithIndex))

// ----------------------------------------------------------------------------
// Gateway Tx Packets table
// ----------------------------------------------------------------------------
PacketWithIndex psGatewayUARTtoCANPackets[] =
{
	/* MotorSpeeds   */ { 0xD0, -1, PacketMotorSpeeds },
	/* MotorPID      */ { 0xD4, -1, PacketMotorPID,   }
};
#define NUMBER_OF_UART_TO_CAN_PACKETS (sizeof(psGatewayUARTtoCANPackets)/sizeof(PacketWithIndex))
#endif

// ----------------------------------------------------------------------------
// Function definitions
// ----------------------------------------------------------------------------

/*Get message period, does not depend on packet type*/
int16_t Packet_getMessagePeriod(uint8_t index)
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
uint16_t Packet_getMessageParameterCount(uint8_t index)
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
uint16_t Packet_getMessageDLC(uint8_t index)
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
const PacketParameter* Packet_getMessageParameterList(uint8_t index)
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

#if (ENABLE_CAN_INTERFACE == 1)
/*CAN message functions*/
uint8_t Packet_getNumOfCANRxPackets(void)
{
	return NUMBER_OF_CAN_RX_PACKETS;
}

PacketWithIndex* Packet_getCANRxPacketsData(void)
{
	return psCANRxPackets;
}

uint8_t Packet_getNumOfCANTxPackets(void)
{
	return NUMBER_OF_CAN_TX_PACKETS;
}

PacketWithIndex* Packet_getCANTxPacketsData(void)
{
	return psCANTxPackets;
}
#endif

#if (ENABLE_UART_INTERFACE == 1)
/*UART message functions*/
uint8_t Packet_getNumOfUARTRxPackets(void)
{
	return NUMBER_OF_UART_RX_PACKETS;
}

PacketWithIndex* Packet_getUARTRxPacketsData(void)
{
	return psUARTRxPackets;
}

uint8_t Packet_getNumOfUARTTxPackets(void)
{
	return NUMBER_OF_UART_TX_PACKETS;
}

PacketWithIndex* Packet_getUARTTxPacketsData(void)
{
	return psUARTTxPackets;
}
#endif

#if (ENABLE_GATEWAYING == 1)
/*Gateway message functions*/
uint8_t Packet_getNumOfGatewayUARTtoCANPackets(void)
{
	return NUMBER_OF_UART_TO_CAN_PACKETS;
}

PacketWithIndex* Packet_getGatewayUARTtoCANPackets(void)
{
	return psGatewayUARTtoCANPackets;
}

uint8_t Packet_getNumOfGatewayCANtoUARTPackets(void)
{
	return NUMBER_OF_CAN_TO_UART_PACKETS;
}

PacketWithIndex* Packet_getGatewayCANtoUARTPackets(void)
{
	return psGatewayCANtoUARTPackets;
}

#endif
