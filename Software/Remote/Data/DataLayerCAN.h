#pragma once

#include <QMutex>
//#include "DataLayerConfig.h"
//#include "Typedef.h"
#include "PacketHandler.h"
#include "DataLayerBase.h"


class DataLayerCAN : public DataLayerBase
{
public:
    DataLayerCAN();
	~DataLayerCAN();

    virtual bool    DL_getData(DLParam param, DLValuePointer value);
    virtual void    DL_setData(DLParam param, DLValuePointer value);

    bool            DL_getDataByComm(DLParam param, DLValuePointer value);
    void            DL_setDataByComm(DLParam param, DLValuePointer value);
    Type            DL_getDataType(DLParam param);
    void            DL_setDataValidity(DLParam, bool validity);
    void            DL_setDefaultValuesForParameters(void);

    QMutex&         getMutex();
private:
    void            SetDataAccordingToType(DLParam param, DLValuePointer value, Type type);
    void            GetDataAccordingToType(DLParam param, DLValuePointer value, Type type);

    //variables
    bool            pbValidFlags_[DLNumberOfParams];
    PacketHandler   packetHandler_;
    QMutex          mutex_;
};