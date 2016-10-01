#pragma once
#include <atomic>
#include <thread>
#include "comm/serial_comm.h"
#include "comm/comm_input.h"
#include "comm/comm_output.h"

struct core {
  CommInput in_data;
  CommOutput out_data;
  SerialComm serial;
  std::thread kinect_frame_thread;
  std::atomic_bool running;
  struct kinect_frame* current_frame = nullptr;
  struct kinect_frame_source* frame_source = nullptr;
  struct UdpHost* udp = nullptr;

  core() { running = ATOMIC_VAR_INIT(true); }
  ~core();
};

void core_start(core* c);
