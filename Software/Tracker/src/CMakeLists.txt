include_directories(
  ${KINECTV2_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  "${CMAKE_SOURCE_DIR}/src"
)

#add_library(follower_common
#  AABB.cpp
#  fl_math.cpp
#vec2.cpp
#  vec3.cpp
#)

#set(FOLLOWER_SOURCES
#  AsyncSerial.cpp
#  crc.cpp
#  msgpack.cpp
#  follower_ctx.cpp

#  fl_sqlite_source.cpp
#  fl_sqlite_writer.cpp
#  fl_yuy2_convert.cpp

#  algorithm/algorithm_move.cpp
#  algorithm/algorithm_runner.cpp

#  comm/datahandler.cpp
#  comm/datalayer.cpp
#  comm/datalayerconfig.cpp
#  comm/interfaceconfig.cpp
#  comm/interfacehandler.cpp
#  comm/packetconfig.cpp
#  comm/serial_comm.cpp
#  comm/typedef.cpp

#  ui/cam_window.cpp
#  ui/main_window.cpp

#  kinect_frame.cpp
#)

#if (KINECT_ENABLED)
#  list(APPEND FOLLOWER_SOURCES kinect_live_source.cpp)
#endif()

#add_library(imgui
#${IMGUI_SOURCES}
#)

set(CORE_SOURCES
  algorithm/algorithm_move.cpp
  algorithm/algorithm_runner.cpp

  AsyncSerial.cpp

  core.cpp
  core_opt.cpp

  comm/datalayer.cpp
  comm/DataLayerConfig.cpp
  comm/InterfaceConfig.cpp
  comm/InterfaceHandler.cpp
  comm/PacketConfig.cpp
  comm/serial_comm.cpp
  comm/typedef.cpp

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
  sqlite
  floatfann
  enet
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
)

#add_executable(
#  follower
#  main.cpp
#  ${FOLLOWER_SOURCES}
#)
#target_link_libraries(
#  follower
#  bgfx_nanovg
#  imgui
#  ${Boost_LIBRARIES}
#  ${KINECTV2_LIBRARY}
#  bgfx
#  glfw
#  ${GLFW_LIBRARIES}
#  yuv
#  follower_common
#  sqlite3
#  floatfann
#  ${CMAKE_DL_LIBS}
#)
