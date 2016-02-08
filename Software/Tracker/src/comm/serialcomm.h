// Copyright 2016 TUT Robotics Club

#pragma once

#include <string>
#include <memory>
#include <mutex> // NOLINT

#include "comm\interfacehandler.h" // NOLINT

class CallbackAsyncSerial;

class SerialComm {
 public:
  SerialComm();
  ~SerialComm();

  bool start(const std::string& port, uint32_t baud);
  bool isOpen();
  void serviceSend();
  void takeLock();
  void releaseLock();
  bool get(DLParam param, DLValuePointer value);  // use lock to preserve data integrity
  void set(DLParam param, DLValuePointer value);

 private:
  void parseMessages(const char*, size_t size);
  void clearMessageStorage(InterfaceMessage& message);

  std::mutex mutex_;
};

