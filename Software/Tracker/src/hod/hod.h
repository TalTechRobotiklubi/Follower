#ifndef HOD_H
#define HOD_H

#include <stdint.h>
#include <stddef.h>
#include "vec3.h"
#include "vec2.h"
#include "segmentation.h"
#include <vector>
#include "AABB.h"
#include "hod_candidate.h"

struct hod_classifier;

const int HOG_WIDTH = 64;
const int HOG_HEIGHT = 128;
const int HOG_BLOCK_SIZE = 2;  // 2x2 cells per block
const int HOG_CELL_SIZE = 8;  // 8x8 pixels per cell
const int HOG_BINS = 9;

struct region_point {
  int x;
  int y;
  int depth_comp;
  int normals_comp;
  vec3 p_r;
};

vec2 cam_coord_to_depth(vec3 p);

struct hod_region {

  AABB bounds; // x, y depth space coordinates

  // Bounding rectangle, metric space
  float sx;
  float sy;
  float ex;
  float ey;

  vec3 center; // metric space
  float n;

  std::vector<region_point> points;
};

struct hod_ctx {
  int source_w;
  int source_h;
  int source_len;
  float source_wf;
  float source_hf;
  int cell_w;
  int cell_h;
  float cell_wf;
  float cell_hf;
  int cells_x;
  int cells_y;
  int cells_len;

  float min_region_size;
  float max_merge_distance;
  float max_vertical_merge_distance;
  float min_inlier_fraction;
  float min_region_height;
  float max_region_height;
  float min_region_width;
  float max_region_width;
  float ransac_max_plane_distance;
  int ransac_steps;
  int cell_sample_count;
  int min_depth_segment_size;
  int min_normal_segment_size;
  float depth_segmentation_threshold;
  float normal_segmentation_threshold;

  image normalized_source;
  float* downscaled_depth;
  float* cell_sample_buffer;

  vec3* point_cloud;
  vec3* normals;

  edge* depth_graph;
  edge* normals_graph;

  segmentation* normals_segmentation;
  segmentation* depth_segmentation;

  int num_depth_edges;
  int num_normals_edges;

  int filtered_regions_capacity;
  int filtered_regions_len;
  hod_region* filtered_regions;

  int* output_cell_indices;

  image output_depth;

  int blocks_x;
  int blocks_y;
  int block_size;

  int candidates_capacity;
  int candidates_len;
  hod_candidate* candidates;

  hod_classifier* classifier;
};

void hod_ctx_init(hod_ctx* hod, int source_w, int source_h, int cell_w, int cell_h);
void hod_grid_sample(hod_ctx* hod, const uint16_t* source);

#endif
