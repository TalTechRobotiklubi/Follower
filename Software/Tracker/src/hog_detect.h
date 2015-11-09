#ifndef HOG_DETECT_H
#define HOG_DETECT_H

#include "AABB.h"
#include <stdint.h>

struct hog_detect;

struct hog_result {
  AABB* boxes;
  size_t len;
};

hog_detect* hog_alloc();
void hog_init(hog_detect* hog);
void hog_destroy(hog_detect* hog);

void hog_load_cascade(hog_detect* hog, const char* name);

hog_result hog_do_detect(hog_detect* hog, uint8_t* image, int width,
                         int height);

#endif
