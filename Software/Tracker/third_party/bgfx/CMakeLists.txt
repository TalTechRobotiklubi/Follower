project(bgfx)

INCLUDE_DIRECTORIES(
  "third_party/khronos"
  ${bx_INCLUDE_DIR}
  include
)

add_definitions(
  -DBGFX_CONFIG_MULTITHREADED=0
  -DBGFX_CONFIG_RENDERER_OPENGL=1
)

add_library(bgfx src/amalgamated.cpp)
