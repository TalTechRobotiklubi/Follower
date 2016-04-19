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
  : serialPort_(0)
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
}

bool SpineCommCAN::communicate()
{
  static MessageAnalyseState analyseState = noData;
  static UART_CANmessage message;
  static unsigned char length;
  static int rxMessageIndex;
  static int dataIndex;
  static splitU16 messageCrc;
  static unsigned int maxData = 0;
  static PacketWithIndex *packet;
  uint8_t numOfPackets;
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
          message.crc = (unsigned char)readBuf[i];
          analyseState = preambulaFound;
        }
        break;
      case preambulaFound:
        // check if it is a message we expect
        numOfPackets = packethandler_.Packet_getNumOfUARTRxPackets();
        packet = packethandler_.Packet_getUARTRxPacketsData();
        for (int j = 0; j < numOfPackets; j++)
        {
          if (readBuf[i] == char(packet[j].uiID))
          {
            // remember the index of the suitable message
            rxMessageIndex = j;
            message.canMessage.id = (unsigned char)readBuf[i];
            message.crc += (unsigned char)readBuf[i];
            analyseState = idOK;
            break;
          }
        }
        // if not ok message then start from beginning
        if (analyseState != idOK)
        {
          analyseState = noData;
        }
        break;
      case idOK:
        // check valid length (1-8)
        length = (unsigned char)readBuf[i];
        //if (length > 0 && length < 9)
        if (length == packethandler_.Packet_getMessageDLC(packet[rxMessageIndex].index))
        {
          message.canMessage.dlc = (unsigned char)readBuf[i];
          message.crc += (unsigned char)readBuf[i];
          analyseState = lengthOK;
        }
        else
        {
          // not valid message, continue with seaching next preambula
          analyseState = noData;
        }
        break;
      case lengthOK:
        // copy data
        if (length != 0)
        {
          message.canMessage.data[dataIndex] = (unsigned char)readBuf[i];
          message.crc += (unsigned char)readBuf[i];
          dataIndex++;
          length--;
          if (length == 0)
          {
            // continue with CRC
            analyseState = crcFirstbyte;
          }
        }
        else
        {
          // length should not be 0 from the beginning -> logic mistake, start from the beginning
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
        if (message.crc == messageCrc.u16)
        {
          // new data received
          hasNewData = true;
          // message OK, store data
          storeDataToDataLayer(&message, &packet[rxMessageIndex]);
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
  return hasNewData;
}

// clears the element from UART_CAN Rx message storage
void SpineCommCAN::clearRxStorageElement(UART_CANmessage *message)
{
  message->canMessage.id = 0;
  message->canMessage.dlc = 0;
  for (int i = 0; i < DATA_BUFFER_SIZE; ++i)
    message->canMessage.data[i] = 0;
  message->crc = 0;
}

void SpineCommCAN::storeDataToDataLayer(UART_CANmessage *message, PacketWithIndex *packet)
{
  uint8_t byteIndex, bitPosition, j, type;
  int16_t length;
  uint16_t dataLayerOk;
  uint32_t data;

  QMutexLocker locker(&dataLayerCAN_.getMutex());

  dataLayerOk = 0;
  /*go through all the parameters in the message and store them into data layer*/
  for (j = 0; j < packethandler_.Packet_getMessageParameterCount(packet->index); j++)
  {
    byteIndex = ((packethandler_.Packet_getMessageParameterList(packet->index) + j)->startBit / 8);
    bitPosition = ((packethandler_.Packet_getMessageParameterList(packet->index) + j)->startBit % 8);
    length = (int16_t)((packethandler_.Packet_getMessageParameterList(packet->index) + j)->lengthBits);

    /*get parameter type from data layer*/
    type = dataLayerCAN_.DL_getDataType((packethandler_.Packet_getMessageParameterList(packet->index) + j)->param);
    switch(type)
    {
      case TypeBool:
      case TypeU4:
      case TypeS4:
      case TypeU8:
      case TypeS8:
        /*sanity check*/
        if (((bitPosition + length) <= 8) && ((bitPosition + length) >= 0))
        {
          /*involves only one byte */
          data = message->canMessage.data[byteIndex];
          /*data into normal value*/
          data = (data >> (8 - bitPosition - length)) & (0xFF >> (8 - length));
          dataLayerCAN_.DL_setDataByComm((packethandler_.Packet_getMessageParameterList(packet->index) + j)->param, &data);
          dataLayerOk++;
        }
        break;
      case TypeU16:
      case TypeS16:
        /*sanity check*/
        if ((length <= 16) && (length > 8))
        {
          /*involves two bytes */
          /*first byte, bit position is assumed to be 0 and the byte is fully for this parameter*/
          data = (message->canMessage.data[byteIndex] << 8) & 0xFF00;
          length = length - 8;
          /*second byte, bit position is still 0, shift if length is not full byte*/
          data |= (message->canMessage.data[byteIndex + 1] & 0xFF);
          data = (uint16_t)(data >> (8 - length));
          dataLayerCAN_.DL_setDataByComm((packethandler_.Packet_getMessageParameterList(packet->index) + j)->param, &data);
          dataLayerOk++;
        }
        break;
      case TypeU32:
      case TypeS32:
      case TypeFloat:
        /*sanity check*/
        if ((length <= 32) && (length > 24))
        {
          /*involves 4 bytes */
          /*first 3 bytes, bit position is assumed to be 0 and the bytes are fully for this parameter*/
          data = (message->canMessage.data[byteIndex] << 24) & 0xFF000000;
          data += (message->canMessage.data[byteIndex + 1] << 16) & 0xFF0000;
          data += (message->canMessage.data[byteIndex + 2] << 8) & 0xFF00;
          length = length - 24;
          /*fourth byte, bit position is still 0, shift if length is not full byte*/
          data |= (message->canMessage.data[byteIndex + 3] & 0xFF);
          data = (uint32_t)(data >> (8 - length));
          dataLayerCAN_.DL_setDataByComm((packethandler_.Packet_getMessageParameterList(packet->index) + j)->param, &data);
          dataLayerOk++;
        }
        break;
      default:
        break;
    }
  }
  /*check if data storing to data layer was ok, if yes then update period depending if it is sporiadic or
     * periodic message*/
  if (dataLayerOk == packethandler_.Packet_getMessageParameterCount(packet->index))
  {
    if (packet->iperiod >= 0)
    {
      packet->iperiod = 0;
    }
    else
    {
      /*mark it received*/
      packet->iperiod = PACKET_RECEIVED;
    }
  }
}



DataLayerBase* SpineCommCAN::getDataLayer() 
{
  return &dataLayerCAN_;
}


// send data to UART
void SpineCommCAN::sendControllerCommands()
{
  int numOfPackets, i;
  PacketWithIndex* packet;

  numOfPackets = packethandler_.Packet_getNumOfUARTTxPackets();
  packet = packethandler_.Packet_getUARTTxPacketsData();

  for (i = 0; i < numOfPackets; i++)
  {
    /*is it a periodic message?*/
    if ((packet[i].iperiod >= 0) && (packethandler_.Packet_getMessagePeriod(packet[i].index) >= 0))
    {
      /*yes, increase the period counter by timer period*/
      packet[i].iperiod += getPeriodInMs();
      /*check periodic messages, if the period is full*/
      if (packet[i].iperiod >= packethandler_.Packet_getMessagePeriod(packet[i].index))
      {
        /*send package*/
        sendDataLayerDataToUART(&packet[i]);
        /*set period counter back to 0*/
        packet[i].iperiod = 0;
      }
    }
    /*sporiadic message*/
    if ((packet[i].iperiod < 0) && (packethandler_.Packet_getMessagePeriod(packet[i].index) < 0))
    {
      /*check sporiadic message is ready to send*/
      if (packet[i].iperiod == PACKET_READY_TO_SEND)
      {
        /*send package*/
        sendDataLayerDataToUART(&packet[i]);
        /*come back here only if new data*/
        packet[i].iperiod = PACKET_WAITING;
      }
    }
  }
}

void SpineCommCAN::handleError()
{
  qDebug() << "[Serial port] error:" << serialPort_->error();
}

/*sets data layer data to UART Tx buffer*/
void SpineCommCAN::sendDataLayerDataToUART(PacketWithIndex *packet)
{
  uint8_t byteIndex, bitPosition;
  uint16_t j;
  int16_t length;
  uint8_t bitmask;
  Type type;
  uint32_t data;
  UART_CANmessage message;
  splitU16 crc;
  QByteArray buffer;
  DLParam param;

  QMutexLocker locker(&dataLayerCAN_.getMutex());

  /*set id and DLC, empty data*/
  message.canMessage.id = packet->uiID;
  message.canMessage.dlc = packethandler_.Packet_getMessageDLC(packet->index);
  for (j = 0; j < DATA_BUFFER_SIZE; j++)
  {
    message.canMessage.data[j] = 0;
  }

  /*set data - take all parameters from packet*/
  for (j = 0; j < packethandler_.Packet_getMessageParameterCount(packet->index); j++)
  {
    byteIndex = ((packethandler_.Packet_getMessageParameterList(packet->index) + j)->startBit / 8);
    bitPosition = ((packethandler_.Packet_getMessageParameterList(packet->index) + j)->startBit % 8);
    length = (int16_t)((packethandler_.Packet_getMessageParameterList(packet->index) + j)->lengthBits);
    /*get parameter type from data layer*/
    param = (packethandler_.Packet_getMessageParameterList(packet->index) + j)->param;
    type = dataLayerCAN_.DL_getDataType(param);

    switch(type)
    {
      case TypeBool:
      case TypeU4:
      case TypeS4:
      case TypeU8:
      case TypeS8:
        /*involves only one byte */
        bitmask = getBitmaskForUARTmessage(bitPosition, length);
        dataLayerCAN_.DL_getDataByComm((packethandler_.Packet_getMessageParameterList(packet->index) + j)->param, &data);
        message.canMessage.data[byteIndex] |= (((((uint8_t)data) & 0xFF) << (8 - length - bitPosition)) & bitmask);
        break;
      case TypeU16:
      case TypeS16:
        /*involves two bytes */
        dataLayerCAN_.DL_getDataByComm((packethandler_.Packet_getMessageParameterList(packet->index) + j)->param, &data);
        /*first byte, bit position is assumed to be 0 and the byte is fully for this parameter*/
        message.canMessage.data[byteIndex] |= (uint8_t)((data >> (length - 8)) & 0xFF);
        /*second byte, bit position is still 0*/
        bitmask = getBitmaskForUARTmessage(0, length - 8);
        message.canMessage.data[byteIndex + 1] |= (uint8_t)(((data & 0xFF) << (16 - length)) & bitmask);
        break;
      case TypeU32:
      case TypeS32:
      case TypeFloat:
        /*involves four bytes */
        dataLayerCAN_.DL_getDataByComm((packethandler_.Packet_getMessageParameterList(packet->index) + j)->param, &data);
        /*first 3 bytes, bit position is assumed to be 0 and the bytes are fully for this parameter*/
        message.canMessage.data[byteIndex] |= (uint8_t)((data >> (length - 8)) & 0xFF);
        message.canMessage.data[byteIndex + 1] |= (uint8_t)((data >> (length - 16)) & 0xFF);
        message.canMessage.data[byteIndex + 2] |= (uint8_t)((data >> (length - 24)) & 0xFF);
        /*fourth byte, bit position is still 0*/
        bitmask = getBitmaskForUARTmessage(0, length - 8);
        message.canMessage.data[byteIndex + 3] |= (uint8_t)(((data & 0xFF) << (32 - length)) & bitmask);
        break;
      default:
        break;
    }
  }
  /*calculate crc and send data*/
  crc.u16 = 0xAA;
  buffer.push_back(char(0xAA));
  crc.u16 += message.canMessage.id;
  buffer.push_back(message.canMessage.id);
  crc.u16 += message.canMessage.dlc;
  buffer.push_back(message.canMessage.dlc);
  for (j = 0; j < message.canMessage.dlc; j++)
  {
    crc.u16 += message.canMessage.data[j];
    buffer.push_back(message.canMessage.data[j]);
  }
  message.crc = crc.u16;
  // j is increased here
  buffer.push_back(crc.u8.byteLow);
  buffer.push_back(crc.u8.byteHigh);

//  if (message.canMessage.id == 0xD5)
//  {
//    qDebug() << message.canMessage.dlc << message.canMessage.data[0] << message.canMessage.data[1]
//        << message.canMessage.data[2] << message.canMessage.data[3]
//        << message.canMessage.data[4] << message.canMessage.data[5];
    serialPort_->write(buffer);
//  }
}

/*Finds which mask to use for data setting in message when bit position and data length is known
 Params: bitPosition - bit position in byte, NB! not absolute position in message
         length - remaining data length
 */
uint8_t SpineCommCAN::getBitmaskForUARTmessage(uint8_t bitPosition, int16_t length)
{
  uint8_t mask = 0xFF;
  uint8_t zerosFromRight, i;

  // shift to left so upper bits are set to zero
  mask = mask >> bitPosition;

  if (length > 0)
  {
    // check how many bits should be set to 0 from right side
    if (length > 8) length = 8;
    zerosFromRight = (8 - length - bitPosition);
    for (i = 0; i < zerosFromRight; i++)
    {
      mask = mask & ~(1 << i);
    }
  }
  else
  {
    mask = 0;
  }
  return mask;
}

#if 0
// data changed on logic layer is stored into data layer
void SpineCommCAN::SetLogicDataToDataLayer( SpineCmd* spineCmd)
{
  int motor1, motor2, motor3;
  bool charge, kick;
  int kickTime, dribblerPower;
  SpineCmdCAN* tempCmd;
  int row, column, length;
  char text[10];

  tempCmd = dynamic_cast<SpineCmdCAN*>(spineCmd);
  tempCmd->GetWheelSpeeds(&motor1, &motor2, &motor3);
  tempCmd->GetKickCommandData(&charge, &dribblerPower, &kickTime, &kick);
  tempCmd->GetScreenWriteCommand(&row, &column, &length, text);

  dataLayerCAN_.DL_setData(DLParamMotor1RequestSpeed, &motor1);
  dataLayerCAN_.DL_setData(DLParamMotor2RequestSpeed, &motor2);
  dataLayerCAN_.DL_setData(DLParamMotor3RequestSpeed, &motor3);
  dataLayerCAN_.DL_setData(DLParamScreenRow, &row);
  dataLayerCAN_.DL_setData(DLParamScreenColumn, &column);
  dataLayerCAN_.DL_setData(DLParamScreenTextLen, &length);
  if (length <= 6 && length > 0)
  {
    for (int i = 0; i < length; i++)
    {
      // hope that enum char values are in correct order
      dataLayerCAN_.DL_setData((DLParam)(DLParamScreenTextChar0 + i), &text[i]);
    }
  }
}
#endif
