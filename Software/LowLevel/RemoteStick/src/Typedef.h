// ----------------------------------------------------------------------------
//
// Data types
//
// ----------------------------------------------------------------------------
#ifndef _TYPEDEF_
#define _TYPEDEF_

// ----------------------------------------------------------------------------
// Include standard types
// ----------------------------------------------------------------------------
#include "stdint.h"

// ----------------------------------------------------------------------------
// Project types
// ----------------------------------------------------------------------------
typedef enum
{
	FALSE = 0,
	TRUE  = 1
}
Boolean;

typedef Boolean  Bool;
typedef uint8_t  U2;
typedef int8_t   S2;
typedef uint8_t  U4;
typedef int8_t   S4;
typedef uint8_t  U8;
typedef int8_t   S8;
typedef uint16_t U16;
typedef int16_t  S16;
typedef uint32_t U32;
typedef int32_t  S32;
typedef float    Float;

// ----------------------------------------------------------------------------
// Parameter types enumeration
// ----------------------------------------------------------------------------
typedef enum
{
	TypeNone,
	TypeBool,
	TypeU2,
	TypeS2,
	TypeU4,
	TypeS4,
	TypeU8,
	TypeS8,
	TypeU16,
	TypeS16,
	TypeU32,
	TypeS32,
	TypeFloat,
	NumberOfTypes
}
Type;

// ----------------------------------------------------------------------------
// Types sizes in bits
// ----------------------------------------------------------------------------
extern const U32 psTypeSize[NumberOfTypes];
extern U32 Type_getSize(Type eType);


#endif
