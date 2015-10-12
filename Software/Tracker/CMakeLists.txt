cmake_minimum_required(VERSION 3.0)
project(Follower)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.55 COMPONENTS
  system
  program_options
  date_time
  regex
  thread
  chrono
  REQUIRED)

find_package(bx REQUIRED)
find_package(bgfx REQUIRED)
find_package(OpenCV REQUIRED)

add_subdirectory("third_party/glfw-3.1.1")

if (WIN32)
  find_package(KinectV2 REQUIRED)
  add_definitions(
    -D_WIN32_WINNT=0x0501
    -D_SCL_SECURE_NO_WARNINGS
    -D_CRT_SECURE_NO_WARNINGS
  )
else()
  add_definitions(-std=c++11 -Wall -Wextra -D_GLIBCXX_USE_CXX11_ABI=0)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

INCLUDE_DIRECTORIES(
  "third_party/glfw-3.1.1/include"
  "third_party/eigen"
)

add_subdirectory(src)