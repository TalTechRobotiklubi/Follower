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
#include "typedef.h"
#include "datalayerconfig.h"

// ----------------------------------------------------------------------------
// Packets enumeration
// ----------------------------------------------------------------------------
typedef enum
{
	PacketMotor1Status,    // 0
	PacketMotor2Status,    // 1
	PacketDistanceSensors, // 2
	PacketAcceleration,    // 3
	PacketMotorSpeeds,     // 4
	PacketCameraControl,   // 5
	// Count of items is 6
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

#ifdef __cplusplus
extern "C" {
	// ----------------------------------------------------------------------------
	// Export packet descriptors
	// ----------------------------------------------------------------------------
	extern PacketDescriptor psPacketDescriptorList[NumberOfPackets];
}
#endif

#endif

