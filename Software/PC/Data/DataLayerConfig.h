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
	DLParamButtonTopLeft,       // 18
	DLParamButtonTopRight,      // 19
	DLParamButtonBottomLeft,    // 20
	DLParamButtonBottomRight,   // 21
	DLParamDistanceSensor1,     // 22
	DLParamDistanceSensor2,     // 23
	DLParamDistanceSensor3,     // 24
	DLParamDistanceSensor4,     // 25
	DLParamDistanceSensor5,     // 26
	DLParamDistanceSensor6,     // 27
	DLParamMotor1RequestSpeed,  // 28
	DLParamMotor2RequestSpeed,  // 29
	DLParamMotor3RequestSpeed,  // 30
	DLParamScreenRow,           // 31
	DLParamScreenColumn,        // 32
	DLParamScreenTextLen,       // 33
	DLParamScreenTextChar0,     // 34
	DLParamScreenTextChar1,     // 35
	DLParamScreenTextChar2,     // 36
	DLParamScreenTextChar3,     // 37
	DLParamScreenTextChar4,     // 38
	DLParamScreenTextChar5,     // 39
	DLParamConstantAddress,     // 40
	DLParamConstantValue,       // 41
	// Count of items is 42
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

