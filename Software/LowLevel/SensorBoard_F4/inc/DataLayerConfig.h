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
	DLParamMotor1RequestSpeed,  // 18
	DLParamMotor2RequestSpeed,  // 19
	DLParamMotor3RequestSpeed,  // 20
	DLParamConstantAddress,     // 21
	DLParamConstantValue,       // 22
	DLParamDistanceSensor1,     // 23
	DLParamDistanceSensor2,     // 24
	DLParamDistanceSensor3,     // 25
	DLParamDistanceSensor4,     // 26
	DLParamDistanceSensor5,     // 27
	DLParamDistanceSensor6,     // 28
	DLParamDistanceSensor7,     // 29
	DLParamDistanceSensor8,     // 30
	DLParamAccelerationX,       // 31
	DLParamAccelerationY,       // 32
	DLParamAccelerationZ,       // 33
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
extern const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams];

#endif

