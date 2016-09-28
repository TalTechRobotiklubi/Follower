if(KINECT_ENABLED)
  add_definitions(-DFL_KINECT_ENABLED=1)
endif()

include_directories(
  ${KINECTV2_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${bx_INCLUDE_DIR}
  "${CMAKE_SOURCE_DIR}/src"
  "stb"
  "imgui"
)

add_library(sqlite3 "sqlite3/sqlite3.c")

set(IMGUI_SOURCES
  imgui/ocornut-imgui/imgui.cpp
  imgui/ocornut-imgui/imgui_draw.cpp
  imgui/ocornut-imgui/imgui_wm.cpp
  imgui/imgui.cpp
  imgui/ocornut_imgui.cpp
)

set(NVG_SOURCES
  nanovg/nanovg.cpp
  nanovg/nanovg_bgfx.cpp
)

set(STB_SOURCES
  stb/stb_image.c
  stb/stb_truetype.h
)

add_library(follower_common
  AABB.cpp
  fl_math.cpp
  fnv1a.cpp
  plane.cpp
  random/pcg_basic.c
  vec2.cpp
  vec3.cpp
)

add_library(follower_hod
  hod/candidate_db.cpp
  hod/hod.cpp
  hod/hod_classifier.cpp
  hod/image.cpp
  hod/segmentation.cpp
)

add_library(bgfx_nanovg
  ${NVG_SOURCES}
  ${STB_SOURCES}
)

set(FOLLOWER_SOURCES
  AsyncSerial.cpp
  crc.cpp
  msgpack.cpp
  follower_ctx.cpp

  fl_sqlite_source.cpp
  fl_sqlite_writer.cpp
  fl_yuy2_convert.cpp

  algorithm/algorithm_move.cpp
  algorithm/algorithm_runner.cpp

  comm/datahandler.cpp
  comm/datalayer.cpp
  comm/datalayerconfig.cpp
  comm/interfaceconfig.cpp
  comm/interfacehandler.cpp
  comm/packetconfig.cpp
  comm/serial_comm.cpp
  comm/typedef.cpp

  hod/hod_debug.cpp

  ui/cam_window.cpp
  ui/main_window.cpp

  kinect_frame.cpp
)

if (KINECT_ENABLED)
  list(APPEND FOLLOWER_SOURCES kinect_live_source.cpp)
endif()

add_library(imgui
  ${IMGUI_SOURCES}
)


add_executable(
  follower
  main.cpp
  ${FOLLOWER_SOURCES}
)

set(CORE_SOURCES
  core.cpp
  core_opt.cpp

  fl_sqlite_source.cpp

  kinect_frame.cpp

  UdpHost.cpp

  parg/parg.c
)

if (KINECT_ENABLED)
  list(APPEND CORE_SOURCES kinect_live_source.cpp)
endif()

add_executable(
  follower_core
  ${CORE_SOURCES}
)

target_link_libraries(
  follower_core
  ${Boost_LIBRARIES}
  ${KINECTV2_LIBRARY}
  yuv
  follower_common
  sqlite3
  floatfann
  enet
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
)

add_executable(
  train_classifier
  hod/hod_train.cpp
)

add_executable(
  test_classifier
  hod/hod_test.cpp
)

target_link_libraries(
  follower
  bgfx_nanovg
  imgui
  ${Boost_LIBRARIES}
  ${KINECTV2_LIBRARY}
  bgfx
  glfw
  ${GLFW_LIBRARIES}
  yuv
  follower_hod
  follower_common
  sqlite3
  floatfann
  ${CMAKE_DL_LIBS}
)

set(UTIL_COMMON_LIBS
  follower_hod
  follower_common
  sqlite3
  floatfann
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_DL_LIBS}
)

target_link_libraries(
  train_classifier
  ${UTIL_COMMON_LIBS}
)

target_link_libraries(
  test_classifier
  ${UTIL_COMMON_LIBS}
)
