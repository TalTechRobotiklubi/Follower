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
	DLParamMotor3ActualSpeed,    // 12
	DLParamMotor3EncoderClicks,  // 13
	DLParamMotor3CurrentDraw,    // 14
	DLParamMotor3DriverTemp,     // 15
	DLParamMotor3BridgeAFault,   // 16
	DLParamMotor3BridgeBFault,   // 17
	DLParamBallContact,          // 18
	DLParamBallInSight,          // 19
	DLParamBallCapacitorCharged, // 20
	DLParamButtonTopLeft,        // 21
	DLParamButtonTopRight,       // 22
	DLParamButtonBottomLeft,     // 23
	DLParamButtonBottomRight,    // 24
	DLParamMotor1RequestSpeed,   // 25
	DLParamMotor2RequestSpeed,   // 26
	DLParamMotor3RequestSpeed,   // 27
	DLParamBallCharge,           // 28
	DLParamBallDribblerPower,    // 29
	DLParamBallKickTime,         // 30
	DLParamBallKick,             // 31
	DLParamScreenRow,            // 32
	DLParamScreenColumn,         // 33
	DLParamScreenTextLen,        // 34
	DLParamScreenTextChar0,      // 35
	DLParamScreenTextChar1,      // 36
	DLParamScreenTextChar2,      // 37
	DLParamScreenTextChar3,      // 38
	DLParamScreenTextChar4,      // 39
	DLParamScreenTextChar5,      // 40
	DLParamConstantAddress,      // 41
	DLParamConstantValue,        // 42
	// Count of items is 43
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

