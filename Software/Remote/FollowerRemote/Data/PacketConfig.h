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
	PacketRobotFeedback,         // 0
	PacketDistanceSensors,       // 1
	PacketInertialMeasurement12, // 2
	PacketInertialMeasurement34, // 3
	PacketMotorSpeeds,           // 4
	PacketRobotControl,          // 5
	// Count of items is 6
	NumberOfPackets
}
Packet;

// ----------------------------------------------------------------------------
// Packet structures
// ----------------------------------------------------------------------------
typedef struct
{
	DLParam param;
	uint16_t startBit;
	uint16_t lengthBits;
}
PacketParameter;

typedef struct
{
	uint16_t id;
	int16_t  period;
	PacketParameter const * parameterList;
	uint16_t parameterCount;
	uint16_t dlc;
}
PacketDescriptor;

// ----------------------------------------------------------------------------
// Export packet descriptors
// ----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
extern PacketDescriptor PacketDescriptorList[NumberOfPackets];
}
#endif

#endif

