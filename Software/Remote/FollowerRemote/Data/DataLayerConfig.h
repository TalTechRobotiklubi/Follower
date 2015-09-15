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
	DLParamDistanceSensor1,     // 12
	DLParamDistanceSensor2,     // 13
	DLParamDistanceSensor3,     // 14
	DLParamDistanceSensor4,     // 15
	DLParamDistanceSensor5,     // 16
	DLParamDistanceSensor6,     // 17
	DLParamDistanceSensor7,     // 18
	DLParamDistanceSensor8,     // 19
	DLParamAccelerationX,       // 20
	DLParamAccelerationY,       // 21
	DLParamAccelerationZ,       // 22
	DLParamTrackedObjectX,      // 23
	DLParamTrackedObjectY,      // 24
	DLParamTrackedObjectHeight, // 25
	DLParamTrackedObjectWidth,  // 26
	DLParamDepthRaw0,           // 27
	DLParamDepthRaw1,           // 28
	DLParamDepthRaw2,           // 29
	DLParamDepthRaw3,           // 30
	DLParamDepthRaw4,           // 31
	DLParamDepthRaw5,           // 32
	DLParamDepthRaw6,           // 33
	DLParamDepthRaw7,           // 34
	DLParamMotor1RequestSpeed,  // 35
	DLParamMotor2RequestSpeed,  // 36
	DLParamMotor3RequestSpeed,  // 37
	DLParamConstantAddress,     // 38
	DLParamConstantValue,       // 39
	// Count of items is 40
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
#ifdef __cplusplus
extern "C" {
extern const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams];
}
#endif

#endif

