#pragma once
#include <atomic>
#include <thread>

struct core {
  std::thread kinect_frame_thread;
  std::atomic_bool running = ATOMIC_VAR_INIT(true);
  struct kinect_frame* current_frame = nullptr;
  struct kinect_frame_source* frame_source = nullptr;
  struct UdpHost* udp = nullptr;

  ~core();
};

void core_start(core* c);
