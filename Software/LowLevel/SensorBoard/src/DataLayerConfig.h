// ----------------------------------------------------------------------------
//
// Casecatcher generated datalayer configuration header file
//
// ----------------------------------------------------------------------------
#ifndef _DATALAYER_CONFIG_
#define _DATALAYER_CONFIG_

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "Typedef.h"

// ----------------------------------------------------------------------------
// Parameters
// ----------------------------------------------------------------------------
typedef enum
{
	DLParamMotor1ActualSpeed,   // 0
	DLParamMotor1EncoderClicks, // 1
	DLParamMotor1CurrentDraw,   // 2
	DLParamMotor1DriverTemp,    // 3
	DLParamMotor1BridgeAFault,  // 4
	DLParamMotor1BridgeBFault,  // 5
	DLParamMotor2ActualSpeed,   // 6
	DLParamMotor2EncoderClicks, // 7
	DLParamMotor2CurrentDraw,   // 8
	DLParamMotor2DriverTemp,    // 9
	DLParamMotor2BridgeAFault,  // 10
	DLParamMotor2BridgeBFault,  // 11
	DLParamTrackedObjectX,      // 12
	DLParamTrackedObjectY,      // 13
	DLParamTrackedObjectHeight, // 14
	DLParamTrackedObjectWidth,  // 15
	DLParamMotor1RequestSpeed,  // 16
	DLParamMotor2RequestSpeed,  // 17
	DLParamDistanceSensor1,     // 18
	DLParamDistanceSensor2,     // 19
	DLParamDistanceSensor3,     // 20
	DLParamDistanceSensor4,     // 21
	DLParamDistanceSensor5,     // 22
	DLParamDistanceSensor6,     // 23
	DLParamDistanceSensor7,     // 24
	DLParamDistanceSensor8,     // 25
	DLParamAccelerationX,       // 26
	DLParamAccelerationY,       // 27
	DLParamAccelerationZ,       // 28
	DLParamDepthRaw0,           // 29
	DLParamDepthRaw1,           // 30
	DLParamDepthRaw2,           // 31
	DLParamDepthRaw3,           // 32
	DLParamDepthRaw4,           // 33
	DLParamDepthRaw5,           // 34
	DLParamDepthRaw6,           // 35
	DLParamDepthRaw7,           // 36
	// Count of items is 37
	DLNumberOfParams
}
DLParam;
#define DLSizeOfParam ((DLNumberOfParams <= 255) ? 1 : 2)

// ----------------------------------------------------------------------------
// General datalayer parameter value pointer
// ----------------------------------------------------------------------------
typedef void* DLValuePointer;

// ----------------------------------------------------------------------------
// Parameter descriptor
// ----------------------------------------------------------------------------
typedef struct
{
	Type eType;
	DLValuePointer pValue;
}
DLParamDescriptor;

// ----------------------------------------------------------------------------
// Export parameters descriptors
// ----------------------------------------------------------------------------
extern const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams];

#endif

