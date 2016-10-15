cmake_minimum_required(VERSION 3.1)
project(imgui)

add_library(imgui
  imgui.cpp
  imgui_draw.cpp
  imgui_impl_glfw.cpp
)
