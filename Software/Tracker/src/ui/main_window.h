// Copyright 2016 TUT Robotics Club

#pragma once

#include <stdint.h>

#include <GLFW/glfw3.h>

#include "ui_layout.h"

namespace Fl {

struct mouse_state {
  int32_t x = 0;
  int32_t y = 0;
  uint8_t button = 0;
  int32_t scroll = 0;
};

struct window_info {
  mouse_state mouse;
  ui_layout layout;
};

GLFWwindow* create_main_window(window_info* info);

}

