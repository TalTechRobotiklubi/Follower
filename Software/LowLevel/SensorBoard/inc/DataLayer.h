// ----------------------------------------------------------------------------
//
// Datalayer header file
//
// ----------------------------------------------------------------------------
#ifndef _DATALAYER_H_
#define _DATALAYER_H_

#include "DataLayerConfig.h"
#include "Typedef.h"

// ----------------------------------------------------------------------------
// Function declarations
// ----------------------------------------------------------------------------
extern Boolean DL_getData(DLParam param, DLValuePointer value);
extern void    DL_peekData(DLParam param, DLValuePointer pValue);
extern Boolean DL_getDataByComm(DLParam param, DLValuePointer value);
extern void    DL_setData(DLParam param, DLValuePointer value);
extern void    DL_setDataByComm(DLParam param, DLValuePointer value);
extern Type    DL_getDataType(DLParam param);
extern void    DL_setDataValidity(DLParam, Boolean validity);

#endif
