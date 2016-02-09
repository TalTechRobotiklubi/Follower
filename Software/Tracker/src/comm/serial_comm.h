// Copyright 2016 TUT Robotics Club

#pragma once

#include <string>
#include <memory>
#include <mutex> // NOLINT

#include "comm/interfacehandler.h" // NOLINT
#include "comm/icomm.h"

class CallbackAsyncSerial;

class SerialComm : public IComm {
 public:
  SerialComm();
  ~SerialComm();

  bool isOpen() const override;
  void send(const CommOutput& data) override;
  
  bool start(const std::string& port, uint32_t baud);
  void takeLock();
  void releaseLock();

  bool get(DLParam param, DLValuePointer value);  // use lock to preserve data integrity

 private:
  void serviceSend();
  void set(DLParam param, DLValuePointer value);
  void parseMessages(const char*, size_t size);
  void clearMessageStorage(InterfaceMessage& message);

  std::mutex mutex_;
};

