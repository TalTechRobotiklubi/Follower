cmake_minimum_required(VERSION 3.0)
include(ExternalProject)
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
find_package(OpenMP)

option(KINECT_ENABLED "Enable Kinect SDK" OFF)

add_subdirectory("third_party/sqlite")
add_subdirectory("third_party/glfw-3.1.2")

INCLUDE_DIRECTORIES(SYSTEM
  "third_party/sqlite"
  "third_party/glfw-3.1.2/include"
)

add_subdirectory("third_party/imgui")
add_subdirectory("third_party/libyuv")
add_subdirectory("third_party/enet-1.3.13")
add_subdirectory("third_party/FastHumanDetection")

if (WIN32)
  set (LUAJIT_ROOT "${CMAKE_SOURCE_DIR}/third_party/luajit")
  set (LUAJIT_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/third_party/luajit/include")
  set (LUAJIT_LIBRARIES "${CMAKE_SOURCE_DIR}/third_party/luajit/lib/lua51.lib")
else()
	find_package(LuaJit REQUIRED)
endif()

set(ARTIFACT_DIR "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_CXX_STANDARD 11)
if (WIN32)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${ARTIFACT_DIR}")
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${ARTIFACT_DIR}")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${ARTIFACT_DIR}")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${ARTIFACT_DIR}")
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${ARTIFACT_DIR}")
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${ARTIFACT_DIR}")
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

  if ("${MSVC_VERSION}" EQUAL "1900")
    set(VPX_LIBRARY "${CMAKE_SOURCE_DIR}/third_party/vpx/lib/vpx_vs14_x64.lib")
  else()
    set(VPX_LIBRARY "${CMAKE_SOURCE_DIR}/third_party/vpx/lib/vpx.lib")
  endif()
  
  set(VPX_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/vpx/include")
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ARTIFACT_DIR})
  find_package(vpx REQUIRED)
  add_definitions(-Wall -Wextra -march=native)
endif()

INCLUDE_DIRECTORIES(SYSTEM
  "third_party/libyuv/include"
  "third_party/enet-1.3.13/include"
  "third_party/imgui"
  "third_party/FastHumanDetection/src"
  ${LUAJIT_INCLUDE_DIRS}
)

add_subdirectory(src)
