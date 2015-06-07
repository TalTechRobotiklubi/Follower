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
	DLParamMotor3RequestSpeed,  // 18
	DLParamConstantAddress,     // 19
	DLParamConstantValue,       // 20
	DLParamDistanceSensor1,     // 21
	DLParamDistanceSensor2,     // 22
	DLParamDistanceSensor3,     // 23
	DLParamDistanceSensor4,     // 24
	DLParamDistanceSensor5,     // 25
	DLParamDistanceSensor6,     // 26
	DLParamDistanceSensor7,     // 27
	DLParamDistanceSensor8,     // 28
	DLParamAccelerationX,       // 29
	DLParamAccelerationY,       // 30
	DLParamAccelerationZ,       // 31
	DLParamDepthArray,          // 32
	// Count of items is 33
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

