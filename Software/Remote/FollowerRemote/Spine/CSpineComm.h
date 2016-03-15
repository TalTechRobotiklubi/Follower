#pragma once

//#include "Serial.h"

#include <QString>

class DataLayerBase;

class SpineComm
{
public:
  SpineComm() {}
  virtual ~SpineComm() {}

  virtual bool open(const QString& name) = 0;
  virtual void close() = 0;
  virtual int getPeriodInMs() { return 10; }
  virtual bool communicate() = 0;
  virtual void sendControllerCommands() = 0;
  virtual DataLayerBase* getDataLayer() = 0;
};
