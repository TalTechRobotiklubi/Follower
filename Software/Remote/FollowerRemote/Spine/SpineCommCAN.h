#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <QSerialPort>

#include "CSpineComm.h"
#include "DataLayerCAN.h"
#include "InterfaceHandler.h"

class SpineCommCAN : public QObject, public SpineComm
{
  Q_OBJECT
public:
	SpineCommCAN();
	~SpineCommCAN(void);

  bool open(const QString& name) override;
  void close() override;
  virtual bool communicate() override;
  virtual DataLayerBase* getDataLayer() override;
  virtual void sendControllerCommands() override;

private slots:
  void handleError();

private:
  void clearRxStorageElement(InterfaceMessage* message);
  void sendMessage(InterfaceMessage* message);

  QSerialPort* serialPort_;
  unsigned int bytesInBuf_;
  DataLayerCAN dataLayerCAN_;
};

