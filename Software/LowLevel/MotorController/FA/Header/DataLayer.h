// ----------------------------------------------------------------------------
//
// Datalayer header file
//
// ----------------------------------------------------------------------------
#ifndef _DATALAYER_H_
#define _DATALAYER_H_

#include "DataLayerConfig.h"

// ----------------------------------------------------------------------------
// Function declarations
// ----------------------------------------------------------------------------
Boolean DL_getDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
void    DL_setDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
void    DL_setDataValidity(DLParam, Boolean validity);

#endif
