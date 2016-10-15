include_directories(SYSTEM
  ${KINECTV2_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  "${CMAKE_SOURCE_DIR}/src"
  ${VPX_INCLUDE_DIR}
)

if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(CORE_SOURCES
  algorithm/algorithm_move.cpp
  algorithm/algorithm_runner.cpp

  AsyncSerial.cpp
  BlockDiff.cpp

  core.cpp
  core_opt.cpp

  comm/datalayer.cpp
  comm/DataLayerConfig.cpp
  comm/InterfaceConfig.cpp
  comm/interfacehandler.cpp
  comm/PacketConfig.cpp
  comm/serial_comm.cpp
  comm/typedef.cpp

  Encode.cpp

  fl_sqlite_source.cpp
  fl_yuy2_convert.cpp

  image.cpp

  kinect_frame.cpp

  UdpHost.cpp

  vec2.cpp

  parg/parg.c
)

if (KINECT_ENABLED)
  list(APPEND CORE_SOURCES kinect_live_source.cpp)
endif()

add_executable(
  follower_core
  ${CORE_SOURCES}
)

add_executable(
  follower_client
  AABB.cpp
  client.cpp
  Decode.cpp
  fl_math.cpp
  image.cpp
  Style.cpp
  Texture.cpp
  vec2.cpp
)

if (WIN32)
  set(WINSOCK
    wsock32
    ws2_32
  )
endif()

target_link_libraries(
  follower_client
  enet
  imgui
  glfw
  ${VPX_LIBRARY}
  yuv
  ${GLFW_LIBRARIES}
  ${CMAKE_DL_LIBS}
  ${WINSOCK}
)

target_link_libraries(
  follower_core
  ${Boost_LIBRARIES}
  ${KINECTV2_LIBRARY}
  yuv
  sqlite
  enet
  fhd
  floatfann
  ${VPX_LIBRARY}
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${WINSOCK}
)
