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
	PacketRobotFeedback,   // 0
	PacketDistanceSensors, // 1
	PacketQuaternions,     // 2
	PacketMotorSpeeds,     // 3
	PacketRobotControl,    // 4
	PacketPidSetup,        // 5
	PacketCameraControl,   // 6
	PacketVisualization,   // 7
	// Count of items is 8
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
	PacketParameter const * parameterList;
	uint16_t parameterCount;
	uint16_t dlc;
}
PacketDescriptor;

// ----------------------------------------------------------------------------
// Export packet descriptors
// ----------------------------------------------------------------------------
extern PacketDescriptor PacketDescriptorList[NumberOfPackets];

#endif

