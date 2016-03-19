cmake_minimum_required(VERSION 3.0)
project(Follower)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.55 COMPONENTS
  system
  thread
  date_time
  chrono
  regex
  REQUIRED)

find_package(Threads)

set(bx_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/bx/include")
if (WIN32)
  include_directories("${bx_INCLUDE_DIR}/compat/msvc")
endif()

add_subdirectory("third_party/libyuv")
add_subdirectory("third_party/glfw-3.1.1")
add_subdirectory("third_party/bgfx")
add_subdirectory("third_party/fann")

option(KINECT_ENABLED "Enable using Kinect SDK" OFF)

if (WIN32)
  add_definitions(
    -D_WIN32_WINNT=0x0501
    -D_SCL_SECURE_NO_WARNINGS
    -D_CRT_SECURE_NO_WARNINGS
  )

  if (KINECT_ENABLED)
    find_package(KinectV2 REQUIRED)
    add_definitions(
      -DFL_KINECT_ENABLED=1
    )
  endif()
else()
  add_definitions(-std=c++11 -Wall -Wextra -march=native)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

INCLUDE_DIRECTORIES(SYSTEM
  "third_party/libyuv/include"
  "third_party/glfw-3.1.1/include"
  "third_party/bgfx/include"
  "third_party/eigen"
  "third_party/fann/src/include"
)

add_subdirectory(src)
