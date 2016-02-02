#ifndef HOG_DETECT_H
#define HOG_DETECT_H

#include "AABB.h"
#include <stdint.h>
#include <vector>
#include <opencv2/objdetect/objdetect.hpp>
#include <memory>

struct hog_detect {
  cv::CascadeClassifier descriptor;
  float detect_scale_factor = 1.1f;
  std::vector<cv::Rect> locations;
  std::vector<AABB> boxes;
};

struct hog_result {
  AABB* boxes;
  size_t len;
};

std::unique_ptr<hog_detect> hog_create();

void hog_load_cascade(hog_detect* hog, const char* name);

hog_result hog_do_detect(hog_detect* hog, uint8_t* image, int width,
                         int height);

#endif
