# Defines:
# bgfx_INCLUDE_DIR
# bgfx_LIBRARY

if (WIN32)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_library(bgfx_LIBRARY bgfxDebug
      PATH_SUFFIXES ".build/win64_vs2013/bin"
    )
    message(STATUS "DEBUG BUILD")
  else()
    find_library(bgfx_LIBRARY bgfxRelease
      PATH_SUFFIXES ".build/win64_vs2013/bin"
    )
    message(STATUS "RELEASE BUILD")
  endif()
else()
  find_library(bgfx_LIBRARY bgfxRelease
	PATH_SUFFIXES ".build/linux64_gcc/bin"
  )
endif()

message(STATUS ${bgfx_LIBRARY})
set(bgfx_LIBRARY_NEEDED bgfx_LIBRARY)

find_path(bgfx_INCLUDE_DIR
  NAMES "bgfx/bgfx.h"
  PATH_SUFFIXES "include"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("bgfx" DEFAULT_MSG
  ${bgfx_LIBRARY_NEEDED}
  bgfx_INCLUDE_DIR
)
