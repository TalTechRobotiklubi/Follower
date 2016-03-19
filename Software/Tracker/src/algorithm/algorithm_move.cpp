// Copyright 2016 TUT Robotics Club

#include "algorithm_move.h"

#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>

#include "AsyncSerial.h"

static CallbackAsyncSerial serial_;

namespace {

enum State {
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

AlgorithmMove::AlgorithmMove() {
  serial_.setCallback(std::bind(&AlgorithmMove::parse, this, std::placeholders::_1, std::placeholders::_2));
  try {
    serial_.open("COM3", 115200u);
    is_running_ = true;
  } catch (boost::system::system_error e) {
    printf("IMU serial port open failed!\n");
  }
}

AlgorithmMove::~AlgorithmMove() {
  serial_.clearCallback();
  serial_.close();
}

void AlgorithmMove::start(const CommInput& in_data) {
  init_degrees_ = calculateEulerDegrees();
  set_point_.yaw = init_degrees_.yaw - in_data.degreesToTurn / 180 * M_PI;
  if (set_point_.yaw < -M_PI)
    set_point_.yaw += 2 * M_PI;
  printf("Init degree: %f \n", init_degrees_.yaw, set_point_.yaw);
}

void AlgorithmMove::run(const CommInput& in_data, CommOutput* out_data) {
  runAlgorithm(in_data, out_data);
}

bool AlgorithmMove::isRunning() {
  return is_running_;
}

void AlgorithmMove::stop() {
  is_running_ = false;
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
        state = FindEnd;
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

Euler AlgorithmMove::calculateEulerDegrees()
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

  Euler euler;
  euler.yaw = atan2(2 * q1 * q2 + 2 * q0 * q3, q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3);
  euler.pitch = -asin(2 * q1 * q3 - 2 * q0 * q2);
  euler.roll = atan2(2 * q0 * q1 + 2 * q2 * q3, q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3);
  return euler;
}

void AlgorithmMove::runAlgorithm(const CommInput& in_data, CommOutput* out_data)
{
  //out_data->left_speed = 1000;  otse
  //out_data->right_speed = -1000;

  Euler degrees = calculateEulerDegrees();
  double error = (set_point_.yaw + M_PI) - (degrees.yaw + M_PI);

  if (error < 180 / M_PI * 10) {
    out_data->left_speed = 0;
    out_data->right_speed = 0;
  }
  else if (error > 0) {
    out_data->left_speed = 600;
    out_data->right_speed = 600;
  }
  else if (error < 0) {
    out_data->left_speed = -600;
    out_data->right_speed = -600;
  }
}
