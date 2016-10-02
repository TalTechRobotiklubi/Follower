#pragma once

#include "IoVec.h"
#include <stdint.h>

struct ActiveMap {
  int width;
  int height;
  IoVec buf;
};

void ActiveMapReset(ActiveMap* map, int width, int height);
void BlockDiff(const uint8_t* prev, const uint8_t* next, int width, int height, ActiveMap* map);
