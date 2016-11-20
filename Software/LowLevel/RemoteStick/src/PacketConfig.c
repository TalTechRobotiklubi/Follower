// ----------------------------------------------------------------------------
//
// Casecatcher generated packet configuration source file
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "PacketConfig.h"

// ----------------------------------------------------------------------------
// Packet "Emergency" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetEmergencyParameterList[2] = 
{
	{ DLParamEmergencySetEvent,   0, 1 },
	{ DLParamEmergencyClearEvent, 1, 1 }
};

// ----------------------------------------------------------------------------
// Packets table
// ----------------------------------------------------------------------------
PacketDescriptor PacketDescriptorList[NumberOfPackets] = 
{
	/* Emergency */ { 0xCF, packetEmergencyParameterList, 2, 1 }
};

