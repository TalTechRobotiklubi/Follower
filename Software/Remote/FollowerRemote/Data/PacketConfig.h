// ----------------------------------------------------------------------------
//
// Casecatcher generated packet configuration header file
//
// ----------------------------------------------------------------------------
#ifndef _PACKET_CONFIG_
#define _PACKET_CONFIG_

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "Typedef.h"
#include "DataLayerConfig.h"

// ----------------------------------------------------------------------------
// Packets enumeration
// ----------------------------------------------------------------------------
typedef enum
{
	PacketMotor1Status,    // 0
	PacketMotor2Status,    // 1
	PacketDistanceSensors, // 2
	PacketAcceleration,    // 3
	PacketTrackedObject,   // 4
	PacketMotorSpeeds,     // 5
	PacketMotorPID,        // 6
	// Count of items is 7
	NumberOfPackets
}
Packet;

// ----------------------------------------------------------------------------
// Packet structures
// ----------------------------------------------------------------------------
typedef struct
{
	DLParam eParam;
	uint16_t uiStartBit;
	uint16_t uiLengthBits;
}
PacketParameter;

typedef struct
{
	uint16_t uiID;
	int16_t  iPeriod;
	PacketParameter const * psParameterList;
	uint16_t uiParameterCount;
	uint16_t uiDLC;
}
PacketDescriptor;

// ----------------------------------------------------------------------------
// Export packet descriptors
// ----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
extern const PacketDescriptor psPacketDescriptorList[NumberOfPackets];
}
#endif

#endif

