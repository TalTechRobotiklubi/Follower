#pragma once

#include "DataLayerConfig.h"
#include "Typedef.h"
#include "PacketHandler.h"


class DataLayerCAN
{
public:
    DataLayerCAN();
	~DataLayerCAN();

    bool            DL_getData(DLParam param, DLValuePointer value);
    bool            DL_getDataByComm(DLParam param, DLValuePointer value);
    void            DL_setData(DLParam param, DLValuePointer value);
    void            DL_setDataByComm(DLParam param, DLValuePointer value);
    Type            DL_getDataType(DLParam param);
    void            DL_setDataValidity(DLParam, bool validity);
    void            DL_setDefaultValuesForParameters(void);

private:
    void            SetDataAccordingToType(DLParam param, DLValuePointer value, Type type);
    void            GetDataAccordingToType(DLParam param, DLValuePointer value, Type type);

    //variables
    bool            pbValidFlags_[DLNumberOfParams];
    PacketHandler   packetHandler_;
};