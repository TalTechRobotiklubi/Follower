#pragma once

#include "DataLayerConfig.h"

class DataLayerBase
{
public:
  DataLayerBase();
	virtual ~DataLayerBase();

  virtual bool DL_getData(DLParam param, DLValuePointer value) = 0;
  virtual void DL_setData(DLParam param, DLValuePointer value) = 0;
};
