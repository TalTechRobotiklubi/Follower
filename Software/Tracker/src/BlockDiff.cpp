#include "BlockDiff.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static const int kActiveMapBlockSize = 16;

void ActiveMapReset(ActiveMap* map, int image_width, int image_height) {
  const double blockSize = double(kActiveMapBlockSize);
  const int newWidth = int(ceil(double(image_width) / blockSize));
  const int newHeight = int(ceil(double(image_height) / blockSize));

  if (newWidth != map->width || newHeight != map->height) {
    if (map->buf.data) {
      free(map->buf.data);
    }
    map->width = newWidth;
    map->height = newHeight;
    map->buf.len = map->width * map->height;
    map->buf.data = (uint8_t*)calloc(map->buf.len, 1);
  } else {
    memset(map->buf.data, 0, map->buf.len);
  }
}

void BlockDiff(const uint8_t* prev, const uint8_t* next, int width, int height,
               ActiveMap* map) {
  ActiveMapReset(map, width, height);

  const int stride = width * 4;
  const int blockStride = kActiveMapBlockSize * 4;

  const int xBlocks = map->width;
  const int yBlocks = map->height;

  // TODO (w - 1, h - 1)
  for (int y = 0; y < yBlocks; y++) {
    uint8_t* dirty = &map->buf.data[y * map->width];
    for (int row = 0; row < kActiveMapBlockSize; row++) {
      for (int x = 0; x < xBlocks; x++) {
        int innerY = y * kActiveMapBlockSize + row;
        int innerX = x * blockStride;
        int idx = innerY * stride + innerX;

        int64_t* p = (int64_t*)&prev[idx];
        int64_t* n = (int64_t*)&next[idx];

        for (int mc = 0; mc < 8; mc++) {
          if (*p != *n) {
            dirty[x] = 1;
            break;
          }
          p++;
          n++;
        }
      }
    }
  }
}
