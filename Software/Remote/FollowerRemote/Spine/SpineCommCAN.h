#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <QSerialPort>

#include "CSpineComm.h"
#include "CANmessages.h"
#include "PacketHandler.h"
#include "DataLayerCAN.h"

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
  void clearRxStorageElement(UART_CANmessage *message);
  void storeDataToDataLayer(UART_CANmessage *message, PacketWithIndex *packet);
  void sendDataLayerDataToUART(PacketWithIndex *packet);
  uint8_t getBitmaskForUARTmessage(uint8_t bitPosition, int16_t length);

  QSerialPort* serialPort_;
  unsigned int bytesInBuf_;
  PacketHandler packethandler_;
  DataLayerCAN dataLayerCAN_;
};

