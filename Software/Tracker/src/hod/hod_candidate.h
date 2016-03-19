#ifndef HOD_CANDIDATE_H
#define HOD_CANDIDATE_H

#include "image.h"
#include <AABB.h>

struct hod_cell {
  float bins[9];
};

struct hod_candidate {
  int candidate_width;
  int candidate_height;
  image depth;

  int num_cells;
  hod_cell* cells;

  int num_features;
  // feature vector, concatenated blocks
  float* features;

  float weight;

  image_region depth_position;
};

struct hod_result {
  int num_features;
  int human;
  float* features;
};

#endif
