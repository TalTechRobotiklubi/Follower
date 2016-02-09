if(KINECT_ENABLED)
  add_definitions(-DFL_KINECT_ENABLED=1)
endif()

include_directories(
  ${KINECTV2_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${bx_INCLUDE_DIR}
  "${CMAKE_SOURCE_DIR}/src"
  ${OpenCV_INCLUDE_DIRS}
)

set(IMGUI_SOURCES
  imgui/ocornut-imgui/imgui.cpp
  imgui/ocornut-imgui/imgui_draw.cpp
  imgui/imgui.cpp
  imgui/ocornut_imgui.cpp
)

set(NVG_SOURCES
  nanovg/nanovg.cpp
  nanovg/nanovg_bgfx.cpp
)

set(STB_SOURCES
  stb/stb_image.h
  stb/stb_truetype.h
)

add_library(bgfx_nanovg
  ${NVG_SOURCES}
  ${STB_SOURCES}
)

set(FOLLOWER_SOURCES
  AABB.cpp
  AsyncSerial.cpp
  crc.cpp
  msgpack.cpp
  pos_texcoord_vertex.cpp
  follower_ctx.cpp
  vec2.cpp
  hog_detect.cpp

  filter/particle_filter.cpp

  fl_block_alloc.cpp
  fl_kinect_stream_source.cpp
  fl_math.cpp
  fl_render.cpp
  fl_stream_reader.cpp
  fl_stream_writer.cpp
  fl_video_source.cpp
  fl_yuy2_convert.cpp

  interp/cubic_hermite.cpp
  
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

  ui/main_window.cpp
  ui/ui_layout.cpp
)

set(FOLLOWER_INCLUDES
  AABB.h
  AsyncSerial.h

  fl_constants.h

  crc.h

  fl_block_alloc.h
  fl_kinect_stream_source.h
  fl_math.h
  fl_render.h
  fl_stream.h
  fl_stream_reader.h
  fl_stream_writer.h
  fl_yuy2_convert.h

  follower_ctx.h
  kinect_frame.h
  kinect_frame_source.h
  kinect_null_frame_source.h
  msgpack.h
  shaders.h

  vec2.h
  hog_detect.h
  
  algorithm/algorithm_move.h
  algorithm/algorithm_runner.h
  algorithm/ialgorithm.h

  comm/comm_output.h
  comm/datahandler.h
  comm/datalayer.h
  comm/datalayerconfig.h
  comm/interfaceconfig.h
  comm/interfacehandler.h
  comm/icomm.h
  comm/packetconfig.h
  comm/serial_comm.h
  comm/typedef.h

  ui/main_window.h
  ui/ui_layout.h
)

if (KINECT_ENABLED)
  list(APPEND FOLLOWER_SOURCES kinect_live_source.cpp)
  list(APPEND FOLLOWER_INCLUDES kinect_live_source.h)
endif()

add_library(imgui
  ${IMGUI_SOURCES}
)

add_executable(
  follower
  main.cpp
  ${FOLLOWER_INCLUDES}
  ${FOLLOWER_SOURCES}
)

message(STATUS ${Boost_LIBRARIES})

target_link_libraries(
  follower
  bgfx_nanovg
  imgui
  ${Boost_LIBRARIES}
  ${KINECTV2_LIBRARY}
  bgfx
  glfw
  ${GLFW_LIBRARIES}
  ${OpenCV_LIBS}
  yuv
)
