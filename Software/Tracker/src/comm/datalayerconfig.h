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
#include "typedef.h"

// ----------------------------------------------------------------------------
// Parameters
// ----------------------------------------------------------------------------
typedef enum
{
	DLParamMotor1ActualSpeed,    // 0
	DLParamMotor1EncoderClicks,  // 1
	DLParamMotor1CurrentDraw,    // 2
	DLParamMotor1DriverTemp,     // 3
	DLParamMotor1BridgeAFault,   // 4
	DLParamMotor1BridgeBFault,   // 5
	DLParamMotor2ActualSpeed,    // 6
	DLParamMotor2EncoderClicks,  // 7
	DLParamMotor2CurrentDraw,    // 8
	DLParamMotor2DriverTemp,     // 9
	DLParamMotor2BridgeAFault,   // 10
	DLParamMotor2BridgeBFault,   // 11
	DLParamDistanceSensor1,      // 12
	DLParamDistanceSensor2,      // 13
	DLParamDistanceSensor3,      // 14
	DLParamDistanceSensor4,      // 15
	DLParamDistanceSensor5,      // 16
	DLParamDistanceSensor6,      // 17
	DLParamDistanceSensor7,      // 18
	DLParamDistanceSensor8,      // 19
	DLParamAccelerationX,        // 20
	DLParamAccelerationY,        // 21
	DLParamAccelerationZ,        // 22
	DLParamMotor1RequestSpeed,   // 23
	DLParamMotor2RequestSpeed,   // 24
	DLParamCameraRequestXDegree, // 25
	DLParamCameraRequestZDegree, // 26
	// Count of items is 27
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

