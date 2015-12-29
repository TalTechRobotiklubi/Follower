#pragma once

#include "DataLayerConfig.h"

enum AperiodicPacketStatus
{
	PACKET_WAITING = -1,
	PACKET_READY_TO_SEND = -2
};

void	DL_init();
bool DL_getData(DLParam param, DLValuePointer value);
void DL_peekData(DLParam param, DLValuePointer pValue);
bool DL_getDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
void DL_setData(DLParam param, DLValuePointer value);
void DL_setDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
Type DL_getDataType(DLParam param);
void DL_setDataValidity(DLParam, bool validity);

