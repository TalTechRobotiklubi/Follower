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
	DLParamScreenRow,           // 21
	DLParamScreenColumn,        // 22
	DLParamScreenTextLen,       // 23
	DLParamScreenTextChar0,     // 24
	DLParamScreenTextChar1,     // 25
	DLParamScreenTextChar2,     // 26
	DLParamScreenTextChar3,     // 27
	DLParamScreenTextChar4,     // 28
	DLParamScreenTextChar5,     // 29
	DLParamConstantAddress,     // 30
	DLParamConstantValue,       // 31
	DLParamButtonTopLeft,       // 32
	DLParamButtonTopRight,      // 33
	DLParamButtonBottomLeft,    // 34
	DLParamButtonBottomRight,   // 35
	DLParamDistanceSensor1,     // 36
	DLParamDistanceSensor2,     // 37
	DLParamDistanceSensor3,     // 38
	DLParamDistanceSensor4,     // 39
	DLParamDistanceSensor5,     // 40
	DLParamDistanceSensor6,     // 41
	DLParamDistanceSensor7,     // 42
	DLParamDistanceSensor8,     // 43
	// Count of items is 44
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

