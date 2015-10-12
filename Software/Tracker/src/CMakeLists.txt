if (WIN32)
  include_directories(
    "${bx_INCLUDE_DIR}/compat/msvc"
  )
endif()

include_directories(
  ${KINECTV2_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${bx_INCLUDE_DIR}
  ${bgfx_INCLUDE_DIR}
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
  kinect_recording.cpp
  kinect_record_source.cpp
  msgpack.cpp
  pos_texcoord_vertex.cpp
  follower_ctx.cpp
  vec2.cpp
  hog_detect.cpp

  filter/kalman.cpp
  filter/particle_filter.cpp

  fl_math.cpp

  interp/cubic_hermite.cpp

  stb_image_resize.cpp
)

set(FOLLOWER_INCLUDES
  AABB.h
  AsyncSerial.h
  crc.h

  fl_math.h

  follower_ctx.h
  kinect_recording.h
  kinect_frame.h
  kinect_frame_source.h
  kinect_null_frame_source.h
  kinect_record_source.h
  msgpack.h
  shaders.h

  stb_image_resize.h

  vec2.h
  hog_detect.h
)

if (WIN32)
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
  ${bgfx_LIBRARY}
  glfw
  ${GLFW_LIBRARIES}
  ${OpenCV_LIBS}
)
