// ----------------------------------------------------------------------------
//
// Datalayer header file
//
// ----------------------------------------------------------------------------
#ifndef _DATALAYER_H_
#define _DATALAYER_H_

#include "DataLayerConfig.h"
#include "Typedef.h"

enum APERIODIC_PACKET_STATUS
{
	PACKET_WAITING = -1,
	PACKET_READY_TO_SEND = -2
};

// ----------------------------------------------------------------------------
// Function declarations
// ----------------------------------------------------------------------------
extern Boolean DL_getData(DLParam param, DLValuePointer value);
extern void    DL_peekData(DLParam param, DLValuePointer pValue);
extern Boolean DL_getDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
extern void    DL_setData(DLParam param, DLValuePointer value);
extern void    DL_setDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
extern Type    DL_getDataType(DLParam param);
extern void    DL_setDataValidity(DLParam, Boolean validity);

#endif
