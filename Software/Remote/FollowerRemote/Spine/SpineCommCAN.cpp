#include "SpineCommCAN.h"

#include <assert.h>
#include <cstdio>
#include <string>
#include <qdebug.h>

// splitted u16 type, which can be accessed in separate byte manner
typedef union
{
  struct
  {
    quint8 byteLow;
    quint8 byteHigh;
  } u8;
  quint16 u16;
} splitU16;

// UART-CAN message format
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+
// | Byte 1  | Byte 2 | Byte 3              | Byte 4  | Byte 5  | ... | Byte 3+N | Byte 4+N | Byte 5+N |
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+
// | 0xAA    | Msg ID | length (n), n = 1-8 | Data[0] | Data[1] | ... | Data[N]  | Checksum | Checksum |
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+

// For UART_CAN message parsing a local state machine is used
enum MessageAnalyseState
{
  noData,
  preambulaFound,
  idOK,
  lengthOK,
  crcFirstbyte,
  crcCheck,
  messageOk
};

SpineCommCAN::SpineCommCAN(void)
  : serialPort_(nullptr)
{
}

SpineCommCAN::~SpineCommCAN(void)
{
  delete serialPort_;
}

bool SpineCommCAN::open(const QString& name)
{
  delete serialPort_;
  serialPort_ = new QSerialPort(name);
  serialPort_->setBaudRate(QSerialPort::Baud115200);
  serialPort_->setDataBits(QSerialPort::Data8);
  serialPort_->setParity(QSerialPort::NoParity);
  serialPort_->setStopBits(QSerialPort::OneStop);
  serialPort_->setFlowControl(QSerialPort::NoFlowControl);

  connect(serialPort_, SIGNAL(error(QSerialPort::SerialPortError)), this,
              SLOT(handleError()));

  if (!serialPort_->open(QIODevice::ReadWrite))
  {
    qDebug() << "[Serial port] open:" << serialPort_->errorString();
    return false;
  }
  return true;
}

void SpineCommCAN::close()
{
  serialPort_->close();
  delete serialPort_;
  serialPort_ = nullptr;
}

bool SpineCommCAN::communicate()
{
  static MessageAnalyseState analyseState = noData;
  InterfaceMessage message;
  static unsigned char length;
  static int rxMessageIndex;
  static int dataIndex;
  static splitU16 messageCrc;
  uint16_t calcCrc;
  bool hasNewData = false;

  if(!serialPort_->isOpen())
    return false;

  QByteArray readBuf = serialPort_->readAll();
  if(readBuf.isEmpty())
    return false;

  // go through all the buffer, search valid messages
  for (int i = 0; i < readBuf.size(); i++)
  {
    switch (analyseState)
    {
      case noData:
        // search preambula '0xAA'
        if (readBuf[i] == char(0xaa) || readBuf[i] == char(0xAA))
        {
          // clear variables for storing next packet
          length = 0;
          rxMessageIndex = 0;
          dataIndex = 0;
          messageCrc.u16 = 0;
          clearRxStorageElement(&message);
          // count also preambula into CRC calculation
          calcCrc = (unsigned char)readBuf[i];
          analyseState = preambulaFound;
        }
        break;
      case preambulaFound:
        if (readBuf[i] == char(0xaa) || readBuf[i] == char(0xAA))
          analyseState = preambulaFound;
        else
        {
          message.id = (unsigned char)readBuf[i];
          calcCrc += message.id;
          analyseState = idOK;
        }
        break;
      case idOK:
        message.length = (unsigned char)readBuf[i];
        {
          QMutexLocker locker(&dataLayerCAN_.getMutex());
          if (InterfaceHandler_checkIfReceivedMessageExists(InterfaceUART, &message))
          {
            calcCrc += message.length;
            length = message.length;
            analyseState = lengthOK;
          }
          else
            analyseState = noData;
        }
        break;
      case lengthOK:
        // copy data
        if (length != 0)
        {
          message.data[dataIndex] = (unsigned char)readBuf[i];
          calcCrc += (unsigned char)readBuf[i];
          dataIndex++;
          length--;
          if (length == 0)
            analyseState = crcFirstbyte;
        }
        else
        {
          analyseState = noData;
        }
        break;
      case crcFirstbyte:
        // store crc first byte
        messageCrc.u8.byteLow = (unsigned char)readBuf[i];
        analyseState = crcCheck;
        break;
      case crcCheck:
        // store 2nd byte
        messageCrc.u8.byteHigh = (unsigned char)readBuf[i];
        // check CRC
        if (calcCrc == messageCrc.u16)
        {
          // new data received
          hasNewData = true;
          QMutexLocker locker(&dataLayerCAN_.getMutex());
          InterfaceHandler_storeReceivedData(&dataLayerCAN_, &message);
          if (message.id == 212)
          {
            int16_t f1;
            int16_t f2;
            int16_t f3;
            dataLayerCAN_.DL_getDataWithoutAffectingStatus(DLParamRobotFeedback1, &f1);
            dataLayerCAN_.DL_getDataWithoutAffectingStatus(DLParamRobotFeedback2, &f2);
            dataLayerCAN_.DL_getDataWithoutAffectingStatus(DLParamRobotFeedback3, &f3);
            qDebug() << "Feedback" << f1 << f2 << f3;
          }

          // ready for next packet
          analyseState = noData;
        }
        else
        {
          analyseState = noData;
        }
        break;
      default:
        break;
    }
  }
  dataLayerCAN_.DL_task(getPeriodInMs());

  return hasNewData;
}

// clears the element from UART_CAN Rx message storage
void SpineCommCAN::clearRxStorageElement(InterfaceMessage* message)
{
  message->id = 0;
  message->length = 0;
  for (int i = 0; i < DATA_BUFFER_SIZE; ++i)
    message->data[i] = 0;
}

DataLayerBase* SpineCommCAN::getDataLayer() 
{
  return &dataLayerCAN_;
}

// send data to UART
void SpineCommCAN::sendControllerCommands()
{
  InterfaceHandler_transmitData(&dataLayerCAN_, InterfaceUART,
                                std::bind(&SpineCommCAN::sendMessage, this, std::placeholders::_1));
}

void SpineCommCAN::handleError()
{
  qDebug() << "[Serial port] error:" << serialPort_->error();
}

void SpineCommCAN::sendMessage(InterfaceMessage* message)
{
  /*calculate crc and send data*/
  QByteArray buffer;
  splitU16 crc;
  crc.u16 = 0xAA;
  buffer.push_back(char(0xAA));
  crc.u16 += message->id;
  buffer.push_back(message->id);
  crc.u16 += message->length;
  buffer.push_back(message->length);
  for (int j = 0; j < message->length; j++)
  {
    crc.u16 += message->data[j];
    buffer.push_back(message->data[j]);
  }
  buffer.push_back(crc.u8.byteLow);
  buffer.push_back(crc.u8.byteHigh);

  serialPort_->write(buffer);
}
