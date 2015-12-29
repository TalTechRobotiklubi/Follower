// ----------------------------------------------------------------------------
//
// Datalayer header file
//
// ----------------------------------------------------------------------------
#ifndef _DATALAYER_H_
#define _DATALAYER_H_

#include "DataLayerConfig.h"

enum APERIODIC_PACKET_STATUS
{
	PACKET_WAITING = -1,
	PACKET_READY_TO_SEND = -2
};

// ----------------------------------------------------------------------------
// Function declarations
// ----------------------------------------------------------------------------
Boolean DL_getData(DLParam param, DLValuePointer value);
void    DL_peekData(DLParam param, DLValuePointer pValue);
Boolean DL_getDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
void    DL_setData(DLParam param, DLValuePointer value);
void    DL_setDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
Type    DL_getDataType(DLParam param);
void    DL_setDataValidity(DLParam, Boolean validity);

#endif
