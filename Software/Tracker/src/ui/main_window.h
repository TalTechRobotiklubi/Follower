// Copyright 2016 TUT Robotics Club

#pragma once

#include <stdint.h>

#include <GLFW/glfw3.h>

namespace Fl {

struct mouse_state {
  int32_t x = 0;
  int32_t y = 0;
  uint8_t button = 0;
  int32_t scroll = 0;
};

struct window_info {
  void* user_data;
  mouse_state mouse;
  float win_width;
  float win_height;
  float depth_width;
  float depth_height;
};

GLFWwindow* create_main_window(window_info* info);

}

