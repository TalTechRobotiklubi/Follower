# Find libvpx
#
# This module defines:
#
#  VPX_FOUND
#  VPX_LIBRARY
#  VPX_INCLUDE_DIR
#

find_library(VPX_LIBRARY vpx)
set(VPX_LIBRARY_NEEDED VPX_LIBRARY)

find_path(VPX_INCLUDE_DIR
    NAMES vp8.h
    PATH_SUFFIXES vpx)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args("vpx" DEFAULT_MSG
    ${VPX_LIBRARY_NEEDED}
    VPX_INCLUDE_DIR)
