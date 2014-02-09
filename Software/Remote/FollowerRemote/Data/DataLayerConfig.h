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
	DLParamMotor3ActualSpeed,   // 12
	DLParamMotor3EncoderClicks, // 13
	DLParamMotor3CurrentDraw,   // 14
	DLParamMotor3DriverTemp,    // 15
	DLParamMotor3BridgeAFault,  // 16
	DLParamMotor3BridgeBFault,  // 17
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
	DLParamMotor1RequestSpeed,  // 29
	DLParamMotor2RequestSpeed,  // 30
	DLParamMotor3RequestSpeed,  // 31
	DLParamConstantAddress,     // 32
	DLParamConstantValue,       // 33
	// Count of items is 34
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

