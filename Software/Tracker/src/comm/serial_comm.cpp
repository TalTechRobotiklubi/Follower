// Copyright 2016 TUT Robotics Club

#include "comm/serial_comm.h"

#include <assert.h>
#include <cstdio>
#include <string>

#include "AsyncSerial.h"

#include "comm/datalayer.h"

// splitted u16 type, which can be accessed in separate byte manner
typedef union {
    struct {
        uint8_t byteLow;
        uint8_t byteHigh;
    } u8;
    uint16_t u16;
} splitU16;

// UART-CAN message format
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+
// | Byte 1  | Byte 2 | Byte 3              | Byte 4  | Byte 5  | ... | Byte 3+N | Byte 4+N | Byte 5+N |
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+
// | 0xAA    | Msg ID | length (n), n = 1-8 | Data[0] | Data[1] | ... | Data[N]  | Checksum | Checksum |
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+

// For UART_CAN message parsing a local state machine is used
enum MessageAnalyseState {
    noData,
    preambulaFound,
    idOK,
    lengthOK,
    crcFirstbyte,
    crcCheck,
    messageOk
};

static void sendMessage(InterfaceMessage* msg);

static std::unique_ptr<CallbackAsyncSerial> serial_ = nullptr;

SerialComm::SerialComm(void) {
  DL_init();
}

SerialComm::~SerialComm(void) {
  if (serial_) {
    serial_->clearCallback();
    serial_->close();
  }
}

bool SerialComm::start(const std::string& port, uint32_t baud) {
  try {
    serial_.reset(new CallbackAsyncSerial(port, baud));
    serial_->setCallback(std::bind(&SerialComm::parseMessages,
      this,
      std::placeholders::_1,
      std::placeholders::_2));
  } catch (std::exception& e) {
    printf("failed to open serial: %s\n", e.what());
    return false;
  }
  return true;
}

bool SerialComm::isOpen() const {
  return serial_ ? serial_->isOpen() : false;
}

void SerialComm::serviceSend() {
  if (isOpen())
    InterfaceHandler_transmitData(InterfaceUART, sendMessage);
}

void SerialComm::takeLock() {
  mutex_.lock();
}

void SerialComm::releaseLock() {
  mutex_.unlock();
}

void SerialComm::receive(CommInput* data)
{
  if (isOpen()) {
    takeLock();
    get(DLParamMotor1ActualSpeed, &data->left_motor_speed);
    get(DLParamMotor1EncoderClicks, &data->left_motor_encoder);
    get(DLParamMotor1ActualSpeed, &data->right_motor_speed);
    get(DLParamMotor1EncoderClicks, &data->right_motor_encoder);
    get(DLParamDistanceSensor1, &data->distance_sensor1);
    get(DLParamDistanceSensor1, &data->distance_sensor2);
    get(DLParamDistanceSensor1, &data->distance_sensor3);
    get(DLParamDistanceSensor1, &data->distance_sensor4);
    get(DLParamDistanceSensor1, &data->distance_sensor5);
    get(DLParamDistanceSensor1, &data->distance_sensor6);
    get(DLParamDistanceSensor1, &data->distance_sensor7);
    get(DLParamDistanceSensor1, &data->distance_sensor8);
    releaseLock();
  }
}

void SerialComm::send(const CommOutput& data) {
  if (isOpen()) {
    int8_t x = (int8_t)data.camera_degrees.x;
    int8_t z = (int8_t)data.camera_degrees.y;
    set(DLParamCameraRequestXDegree, &x);
    set(DLParamCameraRequestZDegree, &z);
    int16_t left = data.left_speed;
    int16_t right = data.right_speed;
    set(DLParamMotor1RequestSpeed, &left);
    set(DLParamMotor2RequestSpeed, &right);
    serviceSend();
  }
}

bool SerialComm::get(DLParam param, DLValuePointer value) {
  return DL_getData(param, value);
}

void SerialComm::set(DLParam param, DLValuePointer value) {
  DL_setData(param, value);
}

void SerialComm::parseMessages(const char* buffer, size_t size) {
  static MessageAnalyseState analyseState = noData;
  static InterfaceMessage message;
  static uint16_t length;
  static uint16_t dataIndex;
  static splitU16 messageCrc;
  static uint16_t calcCrc;

  // go through all the buffer, search valid messages
  for (unsigned int i = 0; i < size; i++) {
    switch (analyseState) {
    case noData:
      // search preambula '0xAA'
      if ((uint8_t)buffer[i] == 0xaa || (uint8_t)buffer[i] == 0xAA) {
        // clear message and variables for next data
        clearMessageStorage(message);
        length = 0;
        dataIndex = 0;
        messageCrc.u16 = 0;
        // preambula is part of CRC
        calcCrc = (uint8_t)buffer[i];
        analyseState = preambulaFound;
      }
      break;
    case preambulaFound:
      message.id = (uint8_t)buffer[i];
      calcCrc += message.id;
      analyseState = idOK;
      break;
    case idOK:
      message.length = (uint8_t)buffer[i];
      if (InterfaceHandler_checkIfReceivedMessageExists(InterfaceUART, &message)) {
        calcCrc += message.length;
        length = message.length;
        analyseState = lengthOK;
      } else {
        // not valid message, continue with seaching next preambula
        analyseState = noData;
      }
      break;
    case lengthOK:
      // copy data
      if (length != 0) {
        message.data[dataIndex] = (uint8_t)buffer[i];
        calcCrc += (uint8_t)buffer[i];
        dataIndex++;
        length--;
        if (length == 0)
          analyseState = crcFirstbyte;
      } else {
        // length should not be 0 from the beginning -> logic mistake, start from the beginning
        analyseState = noData;
      }
      break;
    case crcFirstbyte:
      // store crc first byte
      messageCrc.u8.byteHigh = (uint8_t)buffer[i];
      analyseState = crcCheck;
      break;
    case crcCheck:
      // store 2nd byte
      messageCrc.u8.byteLow = (uint8_t)buffer[i];
      // check CRC
      if (calcCrc == messageCrc.u16) {
        mutex_.lock();
        // new valid message received, copy data into data layer
        InterfaceHandler_storeReceivedData(&message);
        mutex_.unlock();
        // ready for next packet
        analyseState = noData;
      } else {
        analyseState = noData;
      }
      break;
    default:
      break;
    }
  }
}

// clears the element from UART_CAN Rx message storage
void SerialComm::clearMessageStorage(InterfaceMessage& message) {
  message.id = 0;
  message.length = 0;
  for (int i = 0; i < INTERFACE_MSG_SIZE; ++i)
    message.data[i] = 0;
}

void sendMessage(InterfaceMessage* msg) {
  splitU16 crc;
  char data = static_cast<char>(0xAA);
  serial_->write(&data, 1);
  crc.u16 = 0xAA;
  data = msg->id;
  serial_->write(&data, 1);
  crc.u16 += msg->id;
  data = static_cast<char>(msg->length);
  serial_->write(&data, 1);
  crc.u16 += msg->length;
  for (int i = 0; i < msg->length; ++i) {
    data = msg->data[i];
    serial_->write(&data, 1);
    crc.u16 += msg->data[i];
  }
  data = crc.u8.byteHigh;
  serial_->write(&data, 1);
  data = crc.u8.byteLow;
  serial_->write(&data, 1);
}
