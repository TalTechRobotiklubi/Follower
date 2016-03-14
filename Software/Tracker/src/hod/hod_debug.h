#ifndef HOD_DEBUG_H
#define HOD_DEBUG_H

#include <bgfx/bgfx.h>
#include "image.h"

struct hod_ctx;
struct NVGcontext;
struct candidate_db;

struct color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct hod_debug {
  bool train_mode;
  bool window_open;
  bool show_gradients;
  bgfx::TextureHandle downscaled_depth;

  int normalized_depth_len;
  float* normalized_depth;

  hod_ctx* hod;

  NVGcontext* nvg;

  double processing_time;

  bgfx::TextureHandle depth_segmentation_texture;

  int ds_image_len;
  uint8_t* ds_image_data;

  int colors_len;
  color* colors;

  bool update_enabled;

  int normal_image_len;
  uint8_t* normal_image_data;
  bgfx::TextureHandle normals_texture;

  int ns_image_len;
  uint8_t* ns_image_data;
  bgfx::TextureHandle ns_texture;

  // filtered regions
  int fr_image_len;
  uint8_t* fr_image_data;
  bgfx::TextureHandle fr_texture;

  image* candidate_images;
  bgfx::TextureHandle* candidate_textures;

  bgfx::TextureHandle* x_gradient_textures;
  bgfx::TextureHandle* y_gradient_textures;

  candidate_db* candidates_database;
};

void hod_debug_init(hod_debug* dbg, hod_ctx* hod, candidate_db* db);
void hod_debug_update(hod_debug* dbg);
void hod_debug_draw(hod_debug* dbg, NVGcontext* nvg);

#endif
