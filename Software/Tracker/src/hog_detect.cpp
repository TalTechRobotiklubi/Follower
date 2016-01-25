#include "hog_detect.h"
#include <vector>
#include <stdio.h>
#include <chrono>

std::unique_ptr<hog_detect> hog_create() {
  return std::unique_ptr<hog_detect>(new hog_detect());
}

void hog_load_cascade(hog_detect* hog, const char* name) {
  if (!hog->descriptor.load(name)) {
    fprintf(stderr, "failed to load cascade %s\n", name);
  }
}

hog_result hog_do_detect(hog_detect* hog, uint8_t* image, int width,
                         int height) {
  cv::Mat img(height, width, CV_8UC4, image);

  hog->locations.clear();
  hog->boxes.clear();

  hog->descriptor.detectMultiScale(img, hog->locations, hog->detect_scale_factor, 0);

  for (const cv::Rect& r : hog->locations) {
    AABB aabb;
    aabb.top_left = {float(r.tl().x), float(r.tl().y)};
    aabb.bot_right = {float(r.br().x), float(r.br().y)};
    hog->boxes.push_back(aabb);
  }

  return {hog->boxes.data(), hog->boxes.size()};
}
