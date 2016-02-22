// Copyright 2016 TUT Robotics Club

#pragma once

#include <string>
#include <memory>
#include <mutex> // NOLINT

#include "comm/interfacehandler.h" // NOLINT
#include "comm/icomm.h"

class SerialComm : public IComm {
 public:
  SerialComm();
  ~SerialComm();

  bool start(const std::string& port, uint32_t baud);
  bool isOpen() const override;
  
  void receive(CommInput* data) override;
  void send(const CommOutput& data) override;

 private:
  void takeLock();
  void releaseLock();
  bool get(DLParam param, DLValuePointer value);  // use lock to preserve data integrity
  void set(DLParam param, DLValuePointer value);
  void serviceSend();
  
  void parseMessages(const char*, size_t size);  // called from serial port thread
  void clearMessageStorage(InterfaceMessage& message); // called from serial port thread

  std::mutex mutex_;
};

