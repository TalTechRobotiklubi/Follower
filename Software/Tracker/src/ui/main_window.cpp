// Copyright 2016 TUT Robotics Club

#include "main_window.h"

#include <stdio.h>

#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>
#include <bx/timer.h>
#include <bx/commandline.h>

#include "imgui/imgui.h"
#include "follower_ctx.h"

namespace Fl {

void glfwErrorCallback(int error, const char* description) {
  fprintf(stderr, "GLFW error %i: %s\n", error, description);
}

void on_win_resize(GLFWwindow* window, int w, int h) {
  window_info* win_info = (window_info*)glfwGetWindowUserPointer(window);
  win_info->win_width = float(w);
  win_info->win_height = float(h);
  bgfx::reset(w, h, BGFX_RESET_VSYNC);
}

void on_key(GLFWwindow* window, int key, int, int action, int) {
  ImGuiIO& io = ImGui::GetIO();
  io.KeysDown[key] = action == GLFW_PRESS;

  window_info* win_info = (window_info*)glfwGetWindowUserPointer(window);
  follower_ctx* follower = (follower_ctx*)win_info->user_data;
  switch (key) {
    case GLFW_KEY_ESCAPE:
      if (action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
      break;
    case GLFW_KEY_X:
      if (action == GLFW_PRESS) {
        follower->frame_source->advance();
      }
      break;
    case GLFW_KEY_Z:
      if (action == GLFW_PRESS) {
        follower->hod_debugger->update_enabled =
            !follower->hod_debugger->update_enabled;
      }
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

void on_scroll(GLFWwindow* window, double x, double y) {
  (void)x;
  window_info* win_info = (window_info*)glfwGetWindowUserPointer(window);
  win_info->mouse.scroll += int32_t(y);
}

void on_char(GLFWwindow* window, unsigned int c) {
  (void)window;
  ImGuiIO& io = ImGui::GetIO();
  if (c > 0 && c < 0x10000) io.AddInputCharacter((unsigned short)c);
}

GLFWwindow* create_main_window(window_info* win_info) {
  glfwSetErrorCallback(glfwErrorCallback);
  glfwInit();

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  win_info->win_width = float(mode->width);
  win_info->win_height = float(mode->height);
  win_info->depth_width = 512.f;
  win_info->depth_height = 424.f;

  GLFWwindow* window =
      glfwCreateWindow(mode->width, mode->height, "Follower", nullptr, nullptr);

  glfwSetWindowUserPointer(window, win_info);
  glfwSetWindowSizeCallback(window, on_win_resize);
  glfwSetKeyCallback(window, on_key);
  glfwSetCharCallback(window, on_char);
  glfwSetCursorPosCallback(window, on_mouse_move);
  glfwSetScrollCallback(window, on_scroll);
  glfwSetMouseButtonCallback(window, on_mouse_button);

  bgfx::glfwSetWindow(window);
  bgfx::init(bgfx::RendererType::OpenGL);
  bgfx::reset(win_info->win_width, win_info->win_height, BGFX_RESET_VSYNC);
  bgfx::setDebug(BGFX_DEBUG_TEXT);

  ImGuiIO& io = ImGui::GetIO();
  io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 0.0f;

  return window;
}
}
