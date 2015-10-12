#include "hog_detect.h"
#include <opencv2/objdetect/objdetect.hpp>
#include <vector>
#include <stdio.h>
#include <chrono>

struct hog_detect {
  cv::CascadeClassifier* descriptor;
  std::vector<cv::Rect> locations;
  std::vector<AABB> boxes;
};

hog_detect* hog_alloc() {
  return (hog_detect*)calloc(1, sizeof(hog_detect));
}

void hog_init(hog_detect* hog) {
  hog->descriptor = new cv::CascadeClassifier();
  hog->locations = std::vector<cv::Rect>();
  hog->boxes = std::vector<AABB>();
}

void hog_destroy(hog_detect* hog) {
  delete hog->descriptor;
  free(hog);
}

void hog_load_cascade(hog_detect* hog, const char* name) {
  if (!hog->descriptor->load(name)) {
    fprintf(stderr, "failed to load cascade %s\n", name);
  }
}

hog_result hog_do_detect(hog_detect* hog, uint8_t* image, int width, int height) {
  cv::Mat img(height, width, CV_8UC4, image);

  hog->locations.clear();
  hog->boxes.clear();

  hog->descriptor->detectMultiScale(img, hog->locations, 1.2, 0);

  for (const cv::Rect& r : hog->locations) {
    AABB aabb;
    aabb.top_left = { float(r.tl().x), float(r.tl().y) };
    aabb.bot_right = { float(r.br().x), float(r.br().y) };
    hog->boxes.push_back(aabb);
  }

  return {
    hog->boxes.data(),
    hog->boxes.size()
  };
}
