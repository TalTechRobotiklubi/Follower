#include "hod_debug.h"
#include "hod.h"
#include "fl_math.h"
#include "fl_constants.h"
#include "imgui/imgui.h"
#include "nanovg/nanovg.h"
#include "bgfx_imgui_texture.h"
#include <stdio.h>
#include "random/pcg_basic.h"
#include <math.h>
#include "candidate_db.h"
#include <bx/string.h>

const int HOG_CW = 42;
const int HOG_CH = 90;

void render_image_grid(const char* tag, int width, int height,
                       bgfx::TextureHandle* textures, int len) {
  ImGui::Begin(tag, NULL, ImVec2((width + 10) * 5, height + 40));

  for (int i = 0; i < len; i++) {
    bgfx_imgui_texture t;
    t.handle = textures[i];
    ImGui::Image(t.ptr, ImVec2(width, height));

    if (i % 5 < 4) ImGui::SameLine();
  }

  ImGui::End();
}

void render_button_grid(hod_debug* dbg, const char* tag, int width, int height,
                        bgfx::TextureHandle* textures, int len) {
  ImGui::Begin(tag, NULL, ImVec2((width + 10) * 5, (height + 80) * 2));

  char btn_name[16];
  for (int i = 0; i < len; i++) {
    bgfx_imgui_texture t;
    t.handle = textures[i];

    hod_candidate* candidate = &dbg->hod->candidates[i];
    ImGui::BeginGroup();

    bx::snprintf(btn_name, 16, "+##%d", i);
    if (ImGui::Button(btn_name, ImVec2(width, 20.f))) {
      candidate_db_add_candidate(dbg->candidates_database, candidate, true);
    }

    ImGui::Image(t.ptr, ImVec2(width, height));

    bx::snprintf(btn_name, 16, "-##%d", i);
    if (ImGui::Button(btn_name, ImVec2(width, 20.f))) {
      candidate_db_add_candidate(dbg->candidates_database, candidate, false);
    }

    ImGui::EndGroup();

    if (i % 5 < 4) ImGui::SameLine();
  }

  ImGui::End();
}

void render_image(bgfx::TextureHandle texture, float width, float height) {
  bgfx_imgui_texture t;
  t.handle = texture;
  ImGui::Image(t.ptr, ImVec2(width, height));
}

void hod_debug_init(hod_debug* dbg, hod_ctx* hod, candidate_db* db) {
  dbg->window_open = true;
  dbg->downscaled_depth = bgfx::createTexture2D(hod->cells_x, hod->cells_y, 1,
                                                bgfx::TextureFormat::D32F);

  dbg->normalized_depth_len = hod->cells_len;
  dbg->normalized_depth = (float*)malloc(hod->cells_len * sizeof(float));

  dbg->hod = hod;

  dbg->processing_time = 0.0;

  dbg->depth_segmentation_texture = bgfx::createTexture2D(
      hod->cells_x, hod->cells_y, 1, bgfx::TextureFormat::RGBA8);

  dbg->ds_image_len = hod->cells_x * hod->cells_y * 4;
  dbg->ds_image_data = (uint8_t*)calloc(dbg->ds_image_len, 1);

  dbg->colors_len = hod->cells_x * hod->cells_y;
  dbg->colors = (color*)calloc(dbg->colors_len, sizeof(color));

  for (int i = 0; i < dbg->colors_len; i++) {
    dbg->colors[i].r = uint8_t(pcg32_boundedrand(255));
    dbg->colors[i].g = uint8_t(pcg32_boundedrand(255));
    dbg->colors[i].b = uint8_t(pcg32_boundedrand(255));
  }

  dbg->update_enabled = true;

  dbg->normal_image_len = hod->cells_x * hod->cells_y * 4;
  dbg->normal_image_data = (uint8_t*)calloc(dbg->normal_image_len, 1);
  dbg->normals_texture = bgfx::createTexture2D(hod->cells_x, hod->cells_y, 1,
                                               bgfx::TextureFormat::RGBA8);

  dbg->ns_image_len = hod->cells_x * hod->cells_y * 4;
  dbg->ns_image_data = (uint8_t*)calloc(dbg->ns_image_len, 1);
  dbg->ns_texture = bgfx::createTexture2D(hod->cells_x, hod->cells_y, 1,
                                          bgfx::TextureFormat::RGBA8);

  dbg->fr_image_len = hod->cells_x * hod->cells_y * 4;
  dbg->fr_image_data = (uint8_t*)calloc(dbg->fr_image_len, 1);
  dbg->fr_texture = bgfx::createTexture2D(hod->cells_x, hod->cells_y, 1,
                                          bgfx::TextureFormat::RGBA8);

  dbg->candidate_images =
      (image*)calloc(hod->candidates_capacity, sizeof(image));

  dbg->candidate_textures = (bgfx::TextureHandle*)calloc(
      hod->candidates_capacity, sizeof(bgfx::TextureHandle));

  for (int i = 0; i < hod->candidates_capacity; i++) {
    image_init(&dbg->candidate_images[i], HOG_WIDTH, HOG_HEIGHT);

    dbg->candidate_textures[i] = bgfx::createTexture2D(
        HOG_WIDTH, HOG_HEIGHT, 1, bgfx::TextureFormat::D32F);
  }

  dbg->candidates_database = db;
}

void hod_debug_update(hod_debug* dbg) {
  if (!dbg->update_enabled) return;
  const hod_ctx* hod = dbg->hod;

  for (int i = 0; i < hod->cells_len; i++) {
    const float v = hod->downscaled_depth[i];
    if (v > 0.f) {
      dbg->normalized_depth[i] =
          fl_map_range(hod->downscaled_depth[i], 0.5f, 4.5f, 0.f, 1.f);
    } else {
      dbg->normalized_depth[i] = 1.f;
    }
  }

  const bgfx::Memory* hod_downscaled_texture =
      bgfx::makeRef(dbg->normalized_depth, dbg->normalized_depth_len);
  bgfx::updateTexture2D(dbg->downscaled_depth, 0, 0, 0, hod->cells_x,
                        hod->cells_y, hod_downscaled_texture,
                        hod->cells_x * sizeof(float));

  for (int i = 0; i < hod->cells_x * hod->cells_y; i++) {
    int component = segmentation_find(hod->depth_segmentation, i);

    const color* c = &dbg->colors[component];

    dbg->ds_image_data[4 * i] = c->r;
    dbg->ds_image_data[4 * i + 1] = c->g;
    dbg->ds_image_data[4 * i + 2] = c->b;
    dbg->ds_image_data[4 * i + 3] = 255;
  }

  bgfx::updateTexture2D(
      dbg->depth_segmentation_texture, 0, 0, 0, hod->cells_x, hod->cells_y,
      bgfx::makeRef(dbg->ds_image_data, dbg->ds_image_len), hod->cells_x * 4);

  for (int i = 0; i < hod->cells_len; i++) {
    const vec3 n = hod->normals[i];

    dbg->normal_image_data[4 * i] = uint8_t(((1.f + n.x) * 0.5f) * 255.f);
    dbg->normal_image_data[4 * i + 1] = uint8_t(((1.f + n.y) * 0.5f) * 255.f);
    dbg->normal_image_data[4 * i + 2] = uint8_t(((1.f + n.z) * 0.5f) * 255.f);
    dbg->normal_image_data[4 * i + 3] = 255;
  }

  bgfx::updateTexture2D(
      dbg->normals_texture, 0, 0, 0, hod->cells_x, hod->cells_y,
      bgfx::makeRef(dbg->normal_image_data, dbg->normal_image_len),
      hod->cells_x * 4);

  for (int i = 0; i < hod->cells_x * hod->cells_y; i++) {
    int component = segmentation_find(hod->normals_segmentation, i);

    const color* c = &dbg->colors[component];

    dbg->ns_image_data[4 * i] = c->r;
    dbg->ns_image_data[4 * i + 1] = c->g;
    dbg->ns_image_data[4 * i + 2] = c->b;
    dbg->ns_image_data[4 * i + 3] = 255;
  }

  bgfx::updateTexture2D(dbg->ns_texture, 0, 0, 0, hod->cells_x, hod->cells_y,
                        bgfx::makeRef(dbg->ns_image_data, dbg->ns_image_len),
                        hod->cells_x * 4);

  memset(dbg->fr_image_data, 0, dbg->fr_image_len);

  for (int i = 0; i < hod->filtered_regions_len; i++) {
    hod_region* r = &hod->filtered_regions[i];
    color c = dbg->colors[i];
    for (auto& pt : r->points) {
      vec3 p = pt.p_r;
      const vec2 sc = cam_coord_to_depth(p);
      const int sx = int(roundf(sc.x / dbg->hod->cell_wf));
      const int sy = int(roundf(sc.y / dbg->hod->cell_hf));
      int idx = sy * hod->cells_x + sx;
      dbg->fr_image_data[4 * idx] = c.r;
      dbg->fr_image_data[4 * idx + 1] = c.g;
      dbg->fr_image_data[4 * idx + 2] = c.b;
      dbg->fr_image_data[4 * idx + 3] = 255;
    }
  }

  bgfx::updateTexture2D(dbg->fr_texture, 0, 0, 0, hod->cells_x, hod->cells_y,
                        bgfx::makeRef(dbg->fr_image_data, dbg->fr_image_len),
                        hod->cells_x * 4);

  for (int i = 0; i < hod->candidates_len; i++) {
    hod_candidate* candidate = &hod->candidates[i];

    image* candidate_img = &dbg->candidate_images[i];

    image_region src_reg;
    src_reg.x = 1;
    src_reg.y = 1;
    src_reg.width = candidate->depth.width - 1;
    src_reg.height = candidate->depth.height - 1;

    image_region dst_reg;
    dst_reg.x = 0;
    dst_reg.y = 0;
    dst_reg.width = candidate_img->width;
    dst_reg.height = candidate_img->height;

    copy_sub_image(&candidate->depth, &src_reg, candidate_img, &dst_reg);
    image_map_values(candidate_img, 0.5f, 4.5f, 0.f, 1.0f);

    bgfx::TextureHandle handle = dbg->candidate_textures[i];
    bgfx::updateTexture2D(
        handle, 0, 0, 0, candidate_img->width, candidate_img->height,
        bgfx::makeRef(candidate_img->data, candidate_img->bytes),
        candidate_img->pitch);
  }
}

void hod_debug_draw(hod_debug* dbg, NVGcontext* nvg) {
  (void)nvg;
  if (!dbg->window_open) return;

  const float graph_w = 512.f;
  const float graph_h = 424.f;

  ImGui::Begin("HOD", &dbg->window_open);

  ImGui::BeginGroup();

  render_image(dbg->downscaled_depth, graph_w * 0.5f, graph_h * 0.5f);

  ImGui::SameLine();

  render_image(dbg->depth_segmentation_texture, graph_w * 0.5f, graph_h * 0.5f);
  render_image(dbg->normals_texture, graph_w * 0.5f, graph_h * 0.5f);

  ImGui::SameLine();

  render_image(dbg->ns_texture, graph_w * 0.5f, graph_h * 0.5f);
  render_image(dbg->fr_texture, graph_w, graph_h);

  ImGui::EndGroup();

  ImGui::SameLine();

  ImGui::BeginGroup();

  ImGui::BeginChild("params", ImVec2(600.f, 0));
  ImGui::Text("time: %.2f ms", dbg->processing_time);
  ImGui::Checkbox("update enabled", &dbg->update_enabled);
  ImGui::InputFloat("k depth threshold",
                    &dbg->hod->depth_segmentation_threshold);
  ImGui::InputFloat("k norm threshold",
                    &dbg->hod->normal_segmentation_threshold);
  ImGui::SliderFloat("min region size", &dbg->hod->min_region_size, 8.f, 100.f);
  ImGui::SliderFloat("max merge distance (m)", &dbg->hod->max_merge_distance,
                     0.1f, 2.f);
  ImGui::SliderFloat("max vertical merge distance (m)",
                     &dbg->hod->max_vertical_merge_distance, 0.1f, 3.f);
  ImGui::SliderFloat("RANSAC min inlier fraction",
                     &dbg->hod->min_inlier_fraction, 0.5f, 1.f);
  ImGui::SliderFloat("RANSAC max plane dist",
                     &dbg->hod->ransac_max_plane_distance, 0.01f, 0.1f);
  ImGui::SliderInt("RANSAC steps", &dbg->hod->ransac_steps, 1, 36);
  ImGui::SliderFloat("min region height", &dbg->hod->min_region_height, 0.1f,
                     3.f);
  ImGui::SliderFloat("max region height", &dbg->hod->max_region_height, 0.1f,
                     3.f);
  ImGui::SliderFloat("min region width", &dbg->hod->min_region_width, 0.1f,
                     1.f);
  ImGui::SliderFloat("max region width", &dbg->hod->max_region_width, 0.1f,
                     1.5f);
  ImGui::SliderInt("min depth seg size", &dbg->hod->min_depth_segment_size, 4,
                   200);
  ImGui::SliderInt("min normal seg size", &dbg->hod->min_normal_segment_size, 4,
                   200);
  ImGui::EndChild();
  ImGui::EndGroup();

  ImGui::End();

  if (dbg->candidates_database) {
    render_button_grid(dbg, "candidates", HOG_WIDTH * 2, HOG_HEIGHT * 2,
                       dbg->candidate_textures, dbg->hod->candidates_len);
  } else {
    render_image_grid("candidates", HOG_WIDTH * 2, HOG_HEIGHT * 2,
                      dbg->candidate_textures, dbg->hod->candidates_len);

    ImGui::Begin("weights");
    for (int i = 0; i < dbg->hod->candidates_len; i++) {
      hod_candidate* candidate = &dbg->hod->candidates[i];
      bgfx_imgui_texture t;
      t.handle = dbg->candidate_textures[i];
      ImGui::BeginGroup();
      ImGui::Text("%.3f", candidate->weight);
      ImGui::Image(t.ptr, ImVec2(HOG_WIDTH * 2, HOG_HEIGHT * 2));
      ImGui::EndGroup();

      if (i % 5 < 4) ImGui::SameLine();
    }

    ImGui::End();
  }
}
