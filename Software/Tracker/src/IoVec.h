#pragma once

#include <stdint.h>
#include <stdlib.h>

struct IoVec {
  uint8_t* data = nullptr;
  size_t len = 0;
};
