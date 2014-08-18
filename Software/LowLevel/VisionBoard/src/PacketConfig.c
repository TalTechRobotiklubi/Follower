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
// Packet "TrackedObject" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketTrackedObjectParameterList[4] = 
{
	{ DLParamTrackedObjectX,       0, 16 },
	{ DLParamTrackedObjectY,      16, 16 },
	{ DLParamTrackedObjectHeight, 32, 16 },
	{ DLParamTrackedObjectWidth,  48, 16 }
};

// ----------------------------------------------------------------------------
// Packets table
// ----------------------------------------------------------------------------
PacketDescriptor psPacketDescriptorList[NumberOfPackets] =
{
	/* TrackedObject */ { 0xC0, 0, psPacketTrackedObjectParameterList, 4, 8 }
};

