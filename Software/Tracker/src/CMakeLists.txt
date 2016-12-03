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

add_definitions(-DGLFW_INCLUDE_GLEXT -DLODEPNG_NO_COMPILE_CPP)

add_library(lodepng png/lodepng.cpp)

set(CORE_SOURCES
  AsyncSerial.cpp
  BlockDiff.cpp

  classifier/FHDClassifier.cpp
  classifier/TorchClassifier.cpp

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

  fl_sqlite_writer.cpp
  File.cpp
  HistogramDistance.cpp
  Image.cpp

  KinectFrame.cpp
  KinectSqliteFrameSource.cpp

  ScriptLoader.cpp
  UdpHost.cpp

  Yuy2Convert.cpp

  parg/parg.c
)

if (KINECT_ENABLED)
  list(APPEND CORE_SOURCES KinectLiveFrameSource.cpp)
endif()

add_executable(
  follower_core
  ${CORE_SOURCES}
)

set(HEADERS_ONLY CoreObj.h Constants.h Clock.h)

add_executable(
  follower_client
  client.cpp
  Decode.cpp
  File.cpp
  FlMath.cpp
  Image.cpp
  Style.cpp
  Texture.cpp

  ui/Console.cpp

  parg/parg.c

  ${HEADERS_ONLY}
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
  lodepng
  ${GLFW_LIBRARIES}
  ${CMAKE_DL_LIBS}
  ${WINSOCK}
)

if (APPLE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000")
endif()

target_link_libraries(
  follower_core
  ${LUAJIT_LIBRARIES}
  ${Boost_LIBRARIES}
  ${KINECTV2_LIBRARY}
  yuv
  sqlite
  enet
  fhd
  lodepng
  floatfann
  ${VPX_LIBRARY}
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${WINSOCK}
)

add_custom_command(
  TARGET follower_core POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${ARTIFACT_DIR}/scripts
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/scripts ${ARTIFACT_DIR}/scripts
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/classifiers/classifier.nn ${ARTIFACT_DIR}
)

if (WIN32)
  add_custom_command(
    TARGET follower_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${LUAJIT_ROOT}/lib/libluajit.dll ${ARTIFACT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:floatfann>/floatfann.dll ${ARTIFACT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/third_party/torch/lua ${ARTIFACT_DIR}/lua
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/third_party/torch/bin ${ARTIFACT_DIR}
  )
endif()
