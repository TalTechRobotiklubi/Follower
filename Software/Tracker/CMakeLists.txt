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

option(KINECT_ENABLED "Enable Kinect SDK" OFF)

add_subdirectory("third_party/sqlite")
add_subdirectory("third_party/glfw-3.1.2")

INCLUDE_DIRECTORIES(SYSTEM
  "third_party/sqlite"
  "third_party/glfw-3.1.2/include"
  "src/gl3w"
)

add_subdirectory("third_party/imgui")
add_subdirectory("third_party/libyuv")
add_subdirectory("third_party/enet-1.3.13")
add_subdirectory("third_party/FastHumanDetection")

if (WIN32)
  add_definitions(
    -DNOMINMAX
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

  set(VPX_LIBRARY "${CMAKE_SOURCE_DIR}/third_party/vpx/lib/vpx.lib")
  set(VPX_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/vpx/include")
else()
  find_package(vpx REQUIRED)
  add_definitions(-std=c++11 -Wall -Wextra -march=native)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

INCLUDE_DIRECTORIES(SYSTEM
  "third_party/libyuv/include"
  "third_party/enet-1.3.13/include"
  "third_party/imgui"
)

add_subdirectory(src)
