#pragma once

#include <QMutex>

#include "DataLayerBase.h"
#include "PacketConfig.h"

enum APERIODIC_PACKET_STATUS
{
  PACKET_WAITING = -1,
  PACKET_NEW = -2
};

class DataLayerCAN : public DataLayerBase
{
public:
  DataLayerCAN();
  ~DataLayerCAN();
  void DL_task(uint16_t timeElapsedMs);

  virtual bool DL_getData(DLParam param, DLValuePointer value);
  virtual void DL_setData(DLParam param, DLValuePointer value);

  bool DL_getDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
  void DL_setDataWithoutAffectingStatus(DLParam param, DLValuePointer value);
  Type DL_getDataType(DLParam param);
  void DL_setDataValidity(DLParam, bool validity);
  void DL_setDefaultValuesForParameters(void);
  void DL_setAsyncPacketInvalid(PacketDescriptor* packetDesc);

  QMutex& getMutex();
private:
  void SetDataAccordingToType(DLParam param, DLValuePointer value, Type type);
  void GetDataAccordingToType(DLParam param, DLValuePointer value, Type type);

  //variables
  bool            pbValidFlags_[DLNumberOfParams];
  QMutex          mutex_;
};
