#include "hod.h"
#include <assert.h>
#include "random/pcg_basic.h"
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <string.h>
#include "fl_math.h"
#include <unordered_map>
#include "plane.h"
#include <float.h>
#include "hod_classifier.h"

const float KINECT_CX = 255.8f;
const float KINECT_CY = 203.7f;
const float KINECT_FX = 364.7f;
const float KINECT_FY = 366.1f;

static edge construct_depth_edge(int idx_a, float weight_a, int idx_b,
                                 float weight_b) {
  return edge{idx_a, idx_b, fabsf(weight_a - weight_b)};
}

void hod_regions_remove(hod_region* regions, int len, int idx) {
  for (int k = idx + 1; k < len; k++) {
    regions[k - 1] = regions[k];
  }
}

vec3 depth_to_3d(float depth, float x, float y) {
  return {((x - KINECT_CX) / KINECT_FX) * depth,
          ((y - KINECT_CY) / KINECT_FY) * -depth, depth};
}

vec2 cam_coord_to_depth(vec3 p) {
  vec2 r;
  r.x = fl_clamp((p.x * KINECT_FX) / p.z + KINECT_CX, 0.f, 512.f);
  r.y = fl_clamp(-(p.y * KINECT_FY) / p.z + KINECT_CY, 0.f, 424.f);
  return r;
};

void hod_construct_point_cloud(hod_ctx* hod) {
  // construct P
  memset(hod->point_cloud, 0, hod->cells_len * sizeof(vec3));
  for (int y = 0; y < hod->cells_y; y++) {
    for (int x = 0; x < hod->cells_x; x++) {
      for (int s = 0; s < hod->cell_sample_count; s++) {
        uint32_t cell_offset_x = pcg32_boundedrand(uint32_t(hod->cell_w));
        uint32_t cell_offset_y = pcg32_boundedrand(uint32_t(hod->cell_h));
        int sx = x * hod->cell_w + int(cell_offset_x);
        int sy = y * hod->cell_h + int(cell_offset_y);
        hod->cell_sample_buffer[s] =
            hod->normalized_source.data[sy * hod->source_w + sx];
      }

      const int pivot = hod->cell_sample_count / 2;
      std::nth_element(hod->cell_sample_buffer, hod->cell_sample_buffer + pivot,
                       hod->cell_sample_buffer + hod->cell_sample_count);

      const float v = hod->cell_sample_buffer[pivot];

      const int idx = y * hod->cells_x + x;
      hod->downscaled_depth[idx] = v;

      if (v > 0.f) {
        hod->point_cloud[idx] =
            depth_to_3d(v, float(x) * hod->cell_wf, float(y) * hod->cell_hf);
      }
    }
  }
}

void hod_perform_depth_segmentation(hod_ctx* hod) {
  // build graph
  for (int y = 0; y < hod->cells_y; y++) {
    for (int x = 0; x < hod->cells_x; x++) {
      if (x < hod->cells_x - 1) {
        int idx_a = y * hod->cells_x + x;
        int idx_b = y * hod->cells_x + x + 1;
        const vec3* a = &hod->point_cloud[idx_a];
        const vec3* b = &hod->point_cloud[idx_b];

        hod->depth_graph[hod->num_depth_edges] =
            construct_depth_edge(idx_a, a->z, idx_b, b->z);
        hod->num_depth_edges++;
      }

      if (y < hod->cells_y - 1) {
        int idx_a = y * hod->cells_x + x;
        int idx_b = (y + 1) * hod->cells_x + x;
        const vec3* a = &hod->point_cloud[idx_a];
        const vec3* b = &hod->point_cloud[idx_b];

        hod->depth_graph[hod->num_depth_edges] =
            construct_depth_edge(idx_a, a->z, idx_b, b->z);
        hod->num_depth_edges++;
      }
    }
  }

  segmentation_reset(hod->depth_segmentation);
  segment_graph(hod->depth_segmentation, hod->depth_graph, hod->num_depth_edges,
                hod->depth_segmentation_threshold, hod->min_depth_segment_size);
}

void hod_construct_normals(hod_ctx* hod) {
  vec3 pts_buffer[9];
  int pts_buffer_len = 0;
  for (int y = 1; y < hod->cells_y - 1; y++) {
    for (int x = 1; x < hod->cells_x - 1; x++) {
      const int idx = y * hod->cells_x + x;
      const vec3 center = hod->point_cloud[idx];
      if (center.z > 0.f) {
        int component = segmentation_find(hod->depth_segmentation, idx);
        pts_buffer[pts_buffer_len++] = center;

#define ADD_POINT(neighbour_idx)                                   \
  {                                                                \
    int neighbour_component =                                      \
        segmentation_find(hod->depth_segmentation, neighbour_idx); \
    if (component == neighbour_component) {                        \
      const vec3 neighbour = hod->point_cloud[neighbour_idx];      \
      if (neighbour.z > 0.f) {                                     \
        pts_buffer[pts_buffer_len++] = neighbour;                  \
      }                                                            \
    }                                                              \
  }
        ADD_POINT((y - 1) * hod->cells_x + (x - 1));
        ADD_POINT((y - 1) * hod->cells_x + x);
        ADD_POINT((y - 1) * hod->cells_x + (x + 1));
        ADD_POINT(y * hod->cells_x + (x - 1));
        ADD_POINT(y * hod->cells_x + (x + 1));
        ADD_POINT((y + 1) * hod->cells_x + (x - 1));
        ADD_POINT((y + 1) * hod->cells_x + x);
        ADD_POINT((y + 1) * hod->cells_x + (x + 1));
#undef ADD_POINT
        if (pts_buffer_len > 1) {
          hod->normals[idx] = pcl_normal(pts_buffer, pts_buffer_len);
        }
      }

      pts_buffer_len = 0;
    }
  }
}

void hod_perform_normals_segmentation(hod_ctx* hod) {
  // construct normals graph
  for (int y = 0; y < hod->cells_y; y++) {
    for (int x = 0; x < hod->cells_x; x++) {
      if (x < hod->cells_x - 1) {
        int idx_a = y * hod->cells_x + x;
        int idx_b = y * hod->cells_x + x + 1;
        vec3 a = hod->normals[idx_a];
        vec3 b = hod->normals[idx_b];

        hod->normals_graph[hod->num_normals_edges] =
            edge{idx_a, idx_b, acosf(vec3_dot(a, b))};
        hod->num_normals_edges++;
      }

      if (y < hod->cells_y - 1) {
        int idx_a = y * hod->cells_x + x;
        int idx_b = (y + 1) * hod->cells_x + x;
        vec3 a = hod->normals[idx_a];
        vec3 b = hod->normals[idx_b];

        hod->normals_graph[hod->num_normals_edges] =
            edge{idx_a, idx_b, acosf(vec3_dot(a, b))};
        hod->num_normals_edges++;
      }
    }
  }

  segmentation_reset(hod->normals_segmentation);
  segment_graph(hod->normals_segmentation, hod->normals_graph,
                hod->num_normals_edges, hod->normal_segmentation_threshold,
                hod->min_normal_segment_size);
}

void hod_construct_regions(hod_ctx* hod) {
  // TODO: Optimize this
  std::unordered_map<int64_t, hod_region> regions;
  for (int y = 0; y < hod->cells_y; y++) {
    for (int x = 0; x < hod->cells_x; x++) {
      const int idx = y * hod->cells_x + x;
      const vec3* p = &hod->point_cloud[idx];
      if (p->z < 0.5f) {
        continue;
      }
      const int64_t depth_component =
          segmentation_find(hod->depth_segmentation, idx);
      const int64_t normals_component =
          segmentation_find(hod->normals_segmentation, idx);
      const int64_t key = (depth_component << 32) | normals_component;

      auto it = regions.find(key);
      if (it != regions.end()) {
        hod_region* r = &it->second;
        if (p->x < r->sx) r->sx = p->x;
        if (p->x > r->ex) r->ex = p->x;
        if (p->y > r->sy) r->sy = p->y;
        if (p->y < r->ey) r->ey = p->y;

        aabb_expand(&r->bounds, vec2{float(x), float(y)});
        r->center.x = (r->sx + r->ex) * 0.5f;
        r->center.y = (r->sy + r->ey) * 0.5f;
        r->center.z = (p->z + r->n * r->center.z) / (r->n + 1.f);
        r->n += 1.f;
        r->points.push_back(region_point{x, y, int(depth_component),
                                         int(normals_component), *p});
      } else {
        hod_region r;
        r.bounds.top_left.x = float(x);
        r.bounds.top_left.y = float(y);
        r.bounds.bot_right.x = float(x);
        r.bounds.bot_right.y = float(y);
        r.sx = p->x;
        r.sy = p->y;
        r.ex = p->x;
        r.ey = p->y;
        r.center.x = p->x;
        r.center.y = p->y;
        r.center.z = p->z;
        r.n = 1.f;
        r.points.push_back(region_point{x, y, int(depth_component),
                                        int(normals_component), *p});
        regions.insert(std::make_pair(key, r));
      }
    }
  }

  for (auto& kv : regions) {
    hod_region* r = &kv.second;
    if (r->n > hod->min_region_size)
      hod->filtered_regions[hod->filtered_regions_len++] = *r;
  }
}

float region_inlier_fraction(const hod_region* r, float max_plane_distance,
                             int steps) {
  float inlier_fraction = 0.f;
  const uint32_t n_points = r->points.size();
  const float total = float(n_points);
  for (int k = 0; k < steps; k++) {
    // pick 3 random points
    uint32_t a_i, b_i, c_i;
    do {
      a_i = pcg32_boundedrand(n_points);
      b_i = pcg32_boundedrand(n_points);
      c_i = pcg32_boundedrand(n_points);
    } while (a_i == b_i || a_i == c_i || b_i == c_i);
    const region_point* a = &r->points[a_i];
    const region_point* b = &r->points[b_i];
    const region_point* c = &r->points[c_i];

    const plane pi_k = plane_construct(a->p_r, b->p_r, c->p_r);

    float num_fitting = 0.f;

    for (size_t i = 0; i < r->points.size(); i++) {
      const region_point* cur_point = &r->points[i];
      const vec3 P = cur_point->p_r;
      const float distance = fabsf(plane_point_dist(pi_k, P));
      if (distance < max_plane_distance) {
        num_fitting += 1.f;
      }
    }

    float iter_fitting = num_fitting / total;
    if (iter_fitting > inlier_fraction) inlier_fraction = iter_fitting;
  }

  return inlier_fraction;
}

void hod_merge_regions(hod_ctx* hod) {
  const float min_height = hod->min_region_height;
  const float max_height = hod->max_region_height;
  const float min_width = hod->min_region_width;
  const float max_width = hod->max_region_width;

  // TODO: Optimize region removal
  bool needs_merge = true;
  int iterations = 0;
  do {
    std::sort(
        hod->filtered_regions,
        hod->filtered_regions + hod->filtered_regions_len,
        [](const hod_region& a, const hod_region& b) { return a.n > b.n; });

    int merges = 0;
    int len = hod->filtered_regions_len;
    for (int i = 0; i < len; i++) {
      const hod_region* r = &hod->filtered_regions[i];
      const float width = r->ex - r->sx;
      const float height = r->sy - r->ey;

      if (width / height > 2.f) {
        hod_regions_remove(hod->filtered_regions, len, i);
        i--;
        len--;
        continue;
      }

      const bool properly_sized = width >= min_width && width <= max_width &&
                                  height >= min_height && height <= max_height;

      if (!properly_sized) {
        const bool is_small = width < min_width || height < min_height;
        if (is_small &&
            region_inlier_fraction(r, hod->ransac_max_plane_distance,
                                   hod->ransac_steps) >=
                hod->min_inlier_fraction) {
          // find a region to merge into
          for (int j = 0; j < i; j++) {
            hod_region* dst_r = &hod->filtered_regions[j];
            const vec2 mu_xz_i = {r->center.x, r->center.z};
            const vec2 mu_xz_j = {dst_r->center.x, dst_r->center.z};

            const float sigma_xz = vec2_length(vec2_sub(mu_xz_i, mu_xz_j));
            const float dist_y = fabs(r->center.y - dst_r->center.y);

            if (sigma_xz < hod->max_merge_distance &&
                dist_y < hod->max_vertical_merge_distance) {
              // MERGE
              const float total_points = r->n + dst_r->n;
              const float w_i = r->n / total_points;
              const float w_j = dst_r->n / total_points;
              assert(w_i + w_j == 1.f);
              aabb_expand(&dst_r->bounds, r->bounds.top_left);
              aabb_expand(&dst_r->bounds, r->bounds.bot_right);
              dst_r->sx = r->sx < dst_r->sx ? r->sx : dst_r->sx;
              dst_r->sy = r->sy > dst_r->sy ? r->sy : dst_r->sy;
              dst_r->ex = r->ex > dst_r->ex ? r->ex : dst_r->ex;
              dst_r->ey = r->ey < dst_r->ey ? r->ey : dst_r->ey;
              dst_r->center.x = w_i * r->center.x + w_j * dst_r->center.x;
              dst_r->center.y = w_i * r->center.y + w_j * dst_r->center.y;
              dst_r->center.z = w_i * r->center.z + w_j * dst_r->center.z;
              dst_r->n = total_points;
              dst_r->points.insert(dst_r->points.end(), r->points.begin(),
                                   r->points.end());

              hod_regions_remove(hod->filtered_regions, len, i);
              i--;
              len--;
              merges++;

              break;
            }
          }
        } else {
          hod_regions_remove(hod->filtered_regions, len, i);
          i--;
          len--;
        }
      }
    }
    hod->filtered_regions_len = len;
    needs_merge = merges > 0;
    iterations++;
  } while (needs_merge);
}

void hod_copy_regions(hod_ctx* hod) {
  memset(hod->output_cell_indices, -1, hod->cells_len * sizeof(int));
  image_clear(&hod->output_depth, 0.f);

  const int cells_len = hod->cells_len;
  for (int i = 0; i < hod->filtered_regions_len; i++) {
    hod_region* r = &hod->filtered_regions[i];

    for (const region_point& p : r->points) {
      for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
          int idx = (p.y + y) * hod->cells_x + (p.x + x);
          if (idx > -1 && idx < cells_len) {
            hod->output_cell_indices[idx] = 1;
          }
        }
      }
    }

    if (r->bounds.top_left.x >= 1.f) r->bounds.top_left.x -= 1.f;
    if (r->bounds.top_left.y >= 1.f) r->bounds.top_left.y -= 1.f;
    if (r->bounds.bot_right.x <= float(hod->cells_x) - 1.f)
      r->bounds.bot_right.x += 1.f;
    if (r->bounds.bot_right.y <= float(hod->cells_y) - 1.f)
      r->bounds.bot_right.y += 1.f;
  }

  for (int i = 0; i < hod->cells_len; i++) {
    int v = hod->output_cell_indices[i];
    if (v > -1) {
      int cell_x = i % hod->cells_x;
      int cell_y = i / hod->cells_x;

      int start_x = cell_x * hod->cell_w;
      int start_y = cell_y * hod->cell_h;

      for (int y = 0; y < hod->cell_h; y++) {
        for (int x = 0; x < hod->cell_w; x++) {
          const int depth_idx = (start_y + y) * hod->source_w + (x + start_x);
          hod->output_depth.data[depth_idx] =
              hod->normalized_source.data[depth_idx];
        }
      }
    }
  }

  hod->candidates_len = 0;
  for (int i = 0; i < hod->filtered_regions_len; i++) {
    const hod_region* r = &hod->filtered_regions[i];

    const float tl_x = r->bounds.top_left.x * hod->cell_wf;
    const float tl_y = r->bounds.top_left.y * hod->cell_hf;
    const float br_x = r->bounds.bot_right.x * hod->cell_wf + hod->cell_wf;
    const float br_y = r->bounds.bot_right.y * hod->cell_hf + hod->cell_hf;
    const float width = br_x - tl_x;
    const float height = br_y - tl_y;
    const vec2 center = cam_coord_to_depth(r->center);

    const float z_tl_x = tl_x - center.x;
    const float z_tl_y = tl_y - center.y;
    const float z_br_x = br_x - center.x;
    const float z_br_y = br_y - center.y;

    const float sx_p = fabsf(float(HOG_WIDTH) * 0.5f / z_br_x);
    const float sx_n = fabsf(float(HOG_WIDTH) * 0.5f / z_tl_x);
    const float sy_p = fabsf(float(HOG_HEIGHT) * 0.5f / z_br_y);
    const float sy_n = fabsf(float(HOG_HEIGHT) * 0.5f / z_tl_y);

    image_region reg;
    reg.x = tl_x;
    reg.y = tl_y;
    reg.width = int(width);
    reg.height = int(height);

    hod_candidate* candidate = &hod->candidates[hod->candidates_len];
    image_clear(&candidate->depth, 0.f);
    candidate->depth_position = reg;

    const float scale_factor =
        std::min(std::min(sy_p, sy_n), std::min(sx_p, sx_n));

    const int dst_width = int(width * scale_factor);
    const int dst_height = int(height * scale_factor);

    image_region dst_reg;
    dst_reg.x = ((HOG_WIDTH - dst_width) / 2) + 1;
    dst_reg.y = ((HOG_HEIGHT - dst_height) / 2) + 1;
    dst_reg.width = dst_width;
    dst_reg.height = dst_height;

    if (scale_factor < 1.0f) {
      copy_sub_image_scale(&hod->output_depth, &reg, &candidate->depth,
                           &dst_reg);
    } else {
      copy_sub_image(&hod->output_depth, &reg, &candidate->depth, &dst_reg);
    }

    hod->candidates_len++;
  }
}

void hod_calculate_hog_cells(hod_ctx* hod) {
  for (int i = 0; i < hod->candidates_len; i++) {
    hod_candidate* candidate = &hod->candidates[i];
    const int width = candidate->depth.width;
    memset(candidate->cells, 0, candidate->num_cells * sizeof(hod_cell));
    for (int y = 0; y < HOG_HEIGHT; y++) {
      for (int x = 0; x < HOG_WIDTH; x++) {
        const int idx = (y + 1) * width + (x + 1);
        const float pv_x = candidate->depth.data[idx - 1];
        const float nv_x = candidate->depth.data[idx + 1];
        const float pv_y = candidate->depth.data[idx - width];
        const float nv_y = candidate->depth.data[idx + width];

        vec2 grad;
        if (pv_x == 0.f || nv_x == 0.f) {
          grad.x = 0.f;
        } else {
          grad.x = nv_x - pv_x;
        }

        if (pv_y == 0.f || nv_y == 0.f) {
          grad.y = 0.f;
        } else {
          grad.y = nv_y - pv_y;
        }

        const float magnitude = vec2_length(grad);
        const float num_bins = float(HOG_BINS);
        const float angle = (std::atan2(grad.y, grad.x) + F_PI) / (2.f * F_PI);
        const int bin = int(angle * num_bins + FLT_EPSILON) % HOG_BINS;
        assert(bin >= 0 && bin < HOG_BINS);

        const int cell_x = x / HOG_CELL_SIZE;
        const int cell_y = y / HOG_CELL_SIZE;
        const int cell_idx = cell_y * (HOG_WIDTH / HOG_CELL_SIZE) + cell_x;

        candidate->cells[cell_idx].bins[bin] += magnitude;
      }
    }
  }
}

void hod_create_features(hod_ctx* hod) {
  for (int i = 0; i < hod->candidates_len; i++) {
    hod_candidate* candidate = &hod->candidates[i];
    for (int y = 0; y < hod->blocks_y; y++) {
      for (int x = 0; x < hod->blocks_x; x++) {
        int block_idx = y * hod->blocks_x + x;
        int features_offset = hod->block_size * block_idx;

        float squared_sum = 1.f;
        int block_elem_idx = 0;
        for (int cy = 0; cy < HOG_BLOCK_SIZE; cy++) {
          for (int cx = 0; cx < HOG_BLOCK_SIZE; cx++) {
            int cell_x = x + cx;
            int cell_y = y + cy;
            int cell_idx = cell_y * (HOG_WIDTH / HOG_CELL_SIZE) + cell_x;
            hod_cell* cell = &candidate->cells[cell_idx];
            for (int bin = 0; bin < HOG_BINS; bin++) {
              float v = cell->bins[bin];
              squared_sum += v * v;
              candidate->features[features_offset + block_elem_idx] = v;
              block_elem_idx++;
            }
          }
        }

        float normalization_factor = 1.f / sqrtf(squared_sum);
        for (int j = 0; j < hod->block_size; j++) {
          const int idx = features_offset + j;
          const float v =
              (candidate->features[idx] * normalization_factor) * 2.f - 1.f;
          candidate->features[idx] = v;
        }
      }
    }
  }
}

void hod_ctx_init(hod_ctx* hod, int source_w, int source_h, int cell_w,
                  int cell_h) {
  hod->source_w = source_w;
  hod->source_h = source_h;
  hod->source_len = source_w * source_h;
  hod->source_wf = float(source_w);
  hod->source_hf = float(source_h);
  hod->cell_w = cell_w;
  hod->cell_h = cell_h;
  hod->cell_wf = float(cell_w);
  hod->cell_hf = float(cell_h);

  hod->cells_x = source_w / cell_w;
  hod->cells_y = source_h / cell_h;
  hod->cells_len = hod->cells_x * hod->cells_y;

  hod->min_region_size = 13.f;
  hod->max_merge_distance = 0.5f;
  hod->max_vertical_merge_distance = 1.5f;
  hod->min_inlier_fraction = 0.80f;
  hod->min_region_height = 1.f;
  hod->max_region_height = 3.f;
  hod->min_region_width = 0.3f;
  hod->max_region_width = 1.f;
  hod->ransac_max_plane_distance = 0.05f;
  hod->ransac_steps = 20;
  hod->cell_sample_count = 13;
  hod->min_depth_segment_size = 13;
  hod->min_normal_segment_size = 13;
  hod->depth_segmentation_threshold = 50.f;
  hod->normal_segmentation_threshold = 5.f;

  image_init(&hod->normalized_source, source_w, source_h);
  hod->downscaled_depth = (float*)calloc(hod->cells_len, sizeof(float));

  hod->cell_sample_buffer =
      (float*)calloc(hod->cell_sample_count, sizeof(float));

  // The point cloud has sentinel on each side
  hod->point_cloud = (vec3*)calloc(hod->cells_len, sizeof(vec3));
  hod->normals = (vec3*)calloc(hod->cells_len, sizeof(vec3));

  hod->depth_graph = (edge*)calloc(hod->cells_len * 4, sizeof(edge));
  hod->normals_graph = (edge*)calloc(hod->cells_len * 4, sizeof(edge));

  hod->normals_segmentation = (segmentation*)calloc(1, sizeof(segmentation));
  segmentation_init(hod->normals_segmentation, hod->cells_len);

  hod->depth_segmentation = (segmentation*)calloc(1, sizeof(segmentation));
  segmentation_init(hod->depth_segmentation, hod->cells_len);

  hod->num_depth_edges = 0;
  hod->num_normals_edges = 0;

  hod->filtered_regions_capacity = 512;
  hod->filtered_regions_len = 0;
  hod->filtered_regions =
      (hod_region*)calloc(hod->filtered_regions_capacity, sizeof(hod_region));

  hod->output_cell_indices = (int*)calloc(hod->cells_len, sizeof(int));

  image_init(&hod->output_depth, source_w, source_h);

  hod->blocks_x = (HOG_WIDTH - HOG_CELL_SIZE * 2) / HOG_CELL_SIZE + 1;
  hod->blocks_y = (HOG_HEIGHT - HOG_CELL_SIZE * 2) / HOG_CELL_SIZE + 1;
  hod->block_size = HOG_BINS * HOG_BLOCK_SIZE * HOG_BLOCK_SIZE;

  hod->candidates_capacity = 16;
  hod->candidates_len = 0;
  hod->candidates =
      (hod_candidate*)calloc(hod->candidates_capacity, sizeof(hod_candidate));

  for (int i = 0; i < hod->candidates_capacity; i++) {
    hod_candidate* candidate = &hod->candidates[i];
    candidate->candidate_width = HOG_WIDTH;
    candidate->candidate_height = HOG_HEIGHT;
    image_init(&candidate->depth, HOG_WIDTH + 2, HOG_HEIGHT + 2);

    candidate->num_cells =
        (HOG_WIDTH / HOG_CELL_SIZE) * (HOG_HEIGHT / HOG_CELL_SIZE);
    candidate->cells =
        (hod_cell*)calloc(candidate->num_cells, sizeof(hod_cell));

    candidate->num_features = hod->blocks_x * hod->blocks_y * hod->block_size;
    candidate->features =
        (float*)calloc(candidate->num_features, sizeof(float));
    candidate->weight = 0.f;
  }
}

void hod_grid_sample(hod_ctx* hod, const uint16_t* source) {
  hod->num_depth_edges = 0;
  hod->num_normals_edges = 0;
  hod->filtered_regions_len = 0;

  memset(hod->normals, 0, hod->cells_len * sizeof(vec3));

  for (int i = 0; i < hod->source_len; i++) {
    const uint16_t v = source[i];
    if (v >= 500 && v <= 4500) {
      hod->normalized_source.data[i] = float(v) / 1000.f;
    } else {
      hod->normalized_source.data[i] = 0.f;
    }
  }

  hod_construct_point_cloud(hod);
  hod_perform_depth_segmentation(hod);
  hod_construct_normals(hod);
  hod_perform_normals_segmentation(hod);
  hod_construct_regions(hod);
  hod_merge_regions(hod);
  hod_copy_regions(hod);
  hod_calculate_hog_cells(hod);
  hod_create_features(hod);

  if (hod->classifier) {
    for (int i = 0; i < hod->candidates_len; i++) {
      hod_candidate* candidate = &hod->candidates[i];
      candidate->weight = hod_classify(hod->classifier, candidate);
    }
  }
}
