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
const PacketParameter packetEmergencyParameterList[4] = 
{
	{ DLParamEmergencySetEvent,    0,  1 },
	{ DLParamEmergencyClearEvent,  1,  1 },
	{ DLParamEmergencyReserve1,    2,  8 },
	{ DLParamEmergencyReserve2,   10, 16 }
};

// ----------------------------------------------------------------------------
// Packet "RobotControl" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetRobotControlParameterList[2] = 
{
	{ DLParamRequestTranslationSpeed,  0, 16 },
	{ DLParamRequestRotationSpeed,    16, 16 }
};

// ----------------------------------------------------------------------------
// Packets table
// ----------------------------------------------------------------------------
PacketDescriptor PacketDescriptorList[NumberOfPackets] = 
{
	/* Emergency    */ { 0xCF, packetEmergencyParameterList,    4, 4 },
	/* RobotControl */ { 0xD3, packetRobotControlParameterList, 2, 4 }
};

