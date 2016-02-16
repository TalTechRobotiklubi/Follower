// Copyright 2016 TUT Robotics Club

#include "algorithm_move.h"

#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>

#include "AsyncSerial.h"

static CallbackAsyncSerial serial_;

namespace {
static enum State {
  FindEnd,
  Reinit,
  ParseFields
};

const int FieldMaxSize = 10;

void clearData(ParsedData* data)
{
  data->qs[0] = 0;
  data->qs[1] = 0;
  data->qs[2] = 0;
  data->qs[3] = 0;
};

void clearField(char* field)
{
  for (int i = 0; i < FieldMaxSize; ++i)
    field[i] = 0;
}

void clear(ParsedData* data, char* field, int* fieldIndex, int* dataIndex)
{
  clearData(data);
  clearField(field);
  *fieldIndex = 0;
  *dataIndex = 0;
}

void calculateData(char* field, ParsedData* data, int dataIndex)
{
  int sign = 1;
  int index = 0;
  if (field[index] == 0x2D) {
    sign = -1;
    ++index;
  }

  int32_t value = 0;
  while (index < FieldMaxSize && field[index] >= '0') {
    value *= 10;
    value += (int)field[index] - '0';
    ++index;
  }
  value *= sign;

  data->qs[dataIndex] = value;
  if (dataIndex == Quaternion_Size)
    data->timestamp = value;
}

} // namespace

void AlgorithmMove::start() {
  serial_.setCallback(std::bind(&AlgorithmMove::parse, this, std::placeholders::_1, std::placeholders::_2));
  try {
    serial_.open("COM3", 115200u);
    is_running_ = true;
  } catch (boost::system::system_error e) {
    std::cout << "serial port open failed!";
  }
}

void AlgorithmMove::run(CommOutput* outData) {
  calculateEulerDegrees();

  outData->left_speed = 1000;
  outData->right_speed = -1000;
}

bool AlgorithmMove::isRunning() {
  return is_running_;
}

void AlgorithmMove::stop() {
  is_running_ = false;
  serial_.clearCallback();
  serial_.close();
}

void AlgorithmMove::parse(const char* buffer, size_t size) {
  static char field[FieldMaxSize];
  static int fieldIndex = 0;
  static State state = FindEnd;
  static ParsedData data;
  static int dataIndex = 0;

  for (int i = 0; i < size; ++i) {  
    switch (state) {
    case FindEnd:
      if (buffer[i] == 0x0D)
        state = Reinit;
      break;
    case Reinit:
      if (buffer[i] == 0x0A) {
        clear(&data, field, &fieldIndex, &dataIndex);
        state = ParseFields;
      }
      else
        FindEnd;
      break;
    case ParseFields:
      if (buffer[i] == 0x0D)
      {
        calculateData(field, &data, dataIndex);
        pushDataToAlgorithm(data);
        state = Reinit;
      }
      else if (buffer[i] == 0x20) {
        calculateData(field, &data, dataIndex);
        clearField(field);
        fieldIndex = 0;
        ++dataIndex;
      } else {
        field[fieldIndex] = buffer[i];
        ++fieldIndex;
      }
      break;
    }
  }
}

void AlgorithmMove::pushDataToAlgorithm(const ParsedData& data) {
  mutex_.lock();
  data_ = data;
  mutex_.unlock();
}

void AlgorithmMove::calculateEulerDegrees()
{
  ParsedData data;
  mutex_.lock();
  data = data_;
  mutex_.unlock();

  double q0 = data.qs[0];
  double q1 = data.qs[1];
  double q2 = data.qs[2];
  double q3 = data.qs[3];

  double norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3); // normilize
  q0 /= norm;
  q1 /= norm;
  q2 /= norm;
  q3 /= norm;

  yaw_ = atan2(2 * q1 * q2 + 2 * q0 * q3, q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3);
  pitch_ = -asin(2 * q1 * q3 - 2 * q0 * q2);
  roll_ = atan2(2 * q0 * q1 + 2 * q2 * q3, q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3);
}

void AlgorithmMove::runAlgorithm()
{

}