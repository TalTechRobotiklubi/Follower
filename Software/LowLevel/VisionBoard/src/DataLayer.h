// ----------------------------------------------------------------------------
//
// Datalayer header file
//
// ----------------------------------------------------------------------------
#ifndef _DATALAYER_H_
#define _DATALAYER_H_

#include "DataLayerConfig.h"
#include "PacketConfig.h"

enum APERIODIC_PACKET_STATUS
{
	PACKET_WAITING = -1,
	PACKET_NEW = -2
};

// ----------------------------------------------------------------------------
// Function declarations
// ----------------------------------------------------------------------------
void DL_init();
void DL_task();
Boolean DL_getData(DLParam param, DLValuePointer value);
void DL_peekData(DLParam param, DLValuePointer pValue);
void DL_setData(DLParam param, DLValuePointer value);
void DL_setDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
void DL_setDataInvalid(PacketDescriptor* packetDesc);
Type DL_getDataType(DLParam param);
void DL_setDataWithForcedAsyncSend(DLParam param, DLValuePointer value);

#endif
