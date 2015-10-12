# bx_INCLUDE_DIR

set(PATH_SUFFIXES bx)
find_path(bx_INCLUDE_DIR
  NAMES "bx/bx.h"
  PATH_SUFFIXES "include"
)
