// Copyright 2016 TUT Robotics Club

#include "main_window.h"

#include <stdio.h>

#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>
#include <bx/timer.h>
#include <bx/commandline.h>

#include "imgui/imgui.h"

namespace Fl {

void glfwErrorCallback(int error, const char* description) {
  fprintf(stderr, "GLFW error %i: %s\n", error, description);
}

void on_win_resize(GLFWwindow* window, int w, int h) {
  window_info* win_info = (window_info*)glfwGetWindowUserPointer(window);
  ui_layout_calculate(&win_info->layout, w, h);
  bgfx::reset(w, h, BGFX_RESET_NONE);
}

void on_key(GLFWwindow* window, int key, int, int action, int) {
  switch (key) {
  case GLFW_KEY_ESCAPE:
    if (action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    break;
  default:
    break;
  }
}

void on_mouse_move(GLFWwindow* window, double x, double y) {
  window_info* win_info = (window_info*)glfwGetWindowUserPointer(window);
  win_info->mouse.x = int32_t(x);
  win_info->mouse.y = int32_t(y);
}

void on_mouse_button(GLFWwindow* window, int button, int action, int) {
  window_info* win_info = (window_info*)glfwGetWindowUserPointer(window);

  switch (button) {
  case GLFW_MOUSE_BUTTON_LEFT:
    win_info->mouse.button ^=
      (-action ^ win_info->mouse.button) & IMGUI_MBUT_LEFT;
    break;
  default:
    break;
  }
}

GLFWwindow* create_main_window(window_info* win_info) {
  glfwSetErrorCallback(glfwErrorCallback);
  glfwInit();

  ui_layout_calculate(&win_info->layout, 1000, 600);

  GLFWwindow* window =
    glfwCreateWindow(win_info->layout.win_width, win_info->layout.win_height,
    "Follower", nullptr, nullptr);

  glfwSetWindowUserPointer(window, win_info);
  glfwSetWindowSizeCallback(window, on_win_resize);
  glfwSetKeyCallback(window, on_key);
  glfwSetCursorPosCallback(window, on_mouse_move);
  glfwSetMouseButtonCallback(window, on_mouse_button);

  bgfx::glfwSetWindow(window);
  bgfx::init(bgfx::RendererType::OpenGL);
  bgfx::reset(win_info->layout.win_width, win_info->layout.win_height,
    BGFX_RESET_NONE);
  bgfx::setDebug(BGFX_DEBUG_TEXT);

  return window;
}

}