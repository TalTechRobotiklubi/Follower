// ----------------------------------------------------------------------------
//
// Data layer parameter types
//
// ----------------------------------------------------------------------------
#include "typedef.h"

#ifdef __cplusplus
extern "C" {
// ----------------------------------------------------------------------------
// Types size in bits
// ----------------------------------------------------------------------------
const U32 psTypeSize[NumberOfTypes] =
{
	/* TypeNone   */ 0,
	/* TypeBool   */ 1,
	/* TypeU2     */ 2,
	/* TypeS2     */ 2,
	/* TypeU4     */ 4,
	/* TypeS4     */ 4,
	/* TypeU8     */ 8,
	/* TypeS8     */ 8,
	/* TypeU16    */ 16,
	/* TypeS16    */ 16,
	/* TypeU32    */ 32,
	/* TypeS32    */ 32,
	/* TypeFloat  */ sizeof(Float) * 8
};

}
#endif
