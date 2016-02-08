#include "fl_render.h"
#include "nanovg/nanovg.h"
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include "pos_texcoord_vertex.h"
#include "shaders.h"
#include <stdlib.h>
#include "follower_ctx.h"
#include "fl_constants.h"
#include "ui/ui_layout.h"
#include "fl_math.h"

#ifdef WIN32
#define snprintf _snprintf
#endif

namespace {

void draw_rect_outline(NVGcontext* nvg, float x, float y, float w, float h,
                       float width, NVGcolor color) {
  nvgSave(nvg);
  nvgBeginPath(nvg);
  nvgRect(nvg, x, y, w, h);
  nvgStrokeWidth(nvg, width);
  nvgStrokeColor(nvg, color);
  nvgStroke(nvg);
  nvgRestore(nvg);
}
}

namespace Fl {

static pos_texcoord_vertex quad[4] = { { 1.f, 1.f, 1.0f, 1.0f },
{ -1.f, 1.f, 0.0f, 1.0f },
{ -1.f, -1.f, 0.0f, 0.0f },
{ 1.f, -1.f, 1.0f, 0.0f } };

static const uint16_t quad_indices[6] = { 0, 1, 2, 2, 3, 0 };

struct fl_render_context {
  NVGcontext* nvg;

  bgfx::UniformHandle unif_tex_color;
  bgfx::ShaderHandle vsh;
  bgfx::ShaderHandle fsh;
  bgfx::ProgramHandle tex_program;

  bgfx::VertexBufferHandle vbh;
  bgfx::IndexBufferHandle ibh;
};

fl_render_context* fl_renderer_create(NVGcontext* nvg) {
  fl_render_context* renderer =
    (fl_render_context*)calloc(1, sizeof(fl_render_context));

  renderer->nvg = nvg;

  renderer->unif_tex_color =
    bgfx::createUniform("u_texColor", bgfx::UniformType::Int1);

  renderer->vsh = bgfx::createShader(
    bgfx::makeRef(vs_pos_tex_color, sizeof(vs_pos_tex_color)));

  renderer->fsh = bgfx::createShader(
    bgfx::makeRef(fs_pos_tex_color, sizeof(fs_pos_tex_color)));

  renderer->tex_program =
    bgfx::createProgram(renderer->vsh, renderer->fsh, true);

  renderer->vbh = bgfx::createVertexBuffer(bgfx::makeRef(quad, sizeof(quad)),
    pos_texcoord_vertex::ms_decl);

  renderer->ibh = bgfx::createIndexBuffer(
    bgfx::makeRef(quad_indices, sizeof(quad_indices)));

  return renderer;
}

void draw_tracked_bodies(fl_render_context* ctx, const ui_layout* layout,
  const follower_ctx* follower) {
  NVGcontext* nvg = ctx->nvg;
  const float x = layout->toolbar_width;
  const float y = 0.f;

  const float sx = layout->kinect_image_width / float(fl::KINECT_DEPTH_W);
  const float sy = layout->kinect_image_height / float(fl::KINECT_DEPTH_H);
  const vec2 scale = { sx, sy };

  const float border_width = 3.f;
  char tex_buf[32];

  for (const auto& kv : follower->bodies) {
    nvgSave(nvg);
    const body* body = &kv.second;
    const AABB* bbox = &body->kbody.body_box;
    const vec2 tl = vec2_mul_pcw(bbox->top_left, scale);
    const vec2 br = vec2_mul_pcw(bbox->bot_right, scale);
    const float w = br.x - tl.x;
    const float h = br.y - tl.y;

    // AABB
    nvgBeginPath(nvg);
    nvgRect(nvg, x + tl.x, y + tl.y, w, h);
    nvgStrokeWidth(nvg, border_width);
    nvgStrokeColor(nvg, nvgRGBA(255, 20, 0, 200));
    nvgStroke(nvg);

    // Textbox
    nvgBeginPath(nvg);
    nvgRect(nvg, x + tl.x, y + tl.y, 65.f, 40);
    nvgFillColor(nvg, nvgRGBA(0, 0, 0, 255));
    nvgFill(nvg);

    // Text
    snprintf(tex_buf, 32, "TTL: %.2f", body->time_to_live);
    nvgFillColor(nvg, nvgRGBA(225, 225, 225, 255));
    nvgTextAlign(nvg, NVG_ALIGN_TOP);
    nvgText(nvg, x + tl.x + border_width, y + tl.y + border_width, tex_buf,
      nullptr);
    snprintf(tex_buf, 32, "ID: %u", uint32_t(body->kbody.identifier));
    nvgText(nvg, x + tl.x + border_width, y + tl.y + border_width + 20, tex_buf,
      nullptr);

    nvgRestore(nvg);
  }
}

void draw_prediction_plots(fl_render_context* ctx, const ui_layout* layout,
  const follower_ctx* follower) {
  NVGcontext* nvg = ctx->nvg;
  const float x = layout->toolbar_width;
  const float y = layout->kinect_image_height + layout->depth_bar_height;

  const float num_plots = 2.0f;
  const float single_plot_height = layout->plot_height / num_plots;

  for (const auto& kv : follower->bodies) {
    nvgSave(nvg);
    const body* body = &kv.second;

    const float spacing = 5.f;
    float x_pos = spacing;
    const size_t num_possible_points = body->prev_positions.size();
    size_t beginIdx = 0;

    const float render_width = float(num_possible_points) * spacing;
    if (render_width > layout->kinect_image_width) {
      const size_t num_fitted_points =
        size_t(std::floor(layout->kinect_image_width / spacing));
      beginIdx = num_possible_points - num_fitted_points;
    }

    for (size_t i = beginIdx; i < num_possible_points; i++) {
      const vec2& pos = body->prev_positions[i].top_left;
      const float x_y_pos =
        fl_map_range(pos.x, 0.f, float(fl::KINECT_DEPTH_W), single_plot_height, 0.f);
      const float y_y_pos =
        fl_map_range(pos.y, float(fl::KINECT_DEPTH_H), 0.f, single_plot_height, 0.f);

      nvgFillColor(nvg, nvgRGBA(255, 255, 255, 220));

      nvgBeginPath(nvg);
      nvgCircle(nvg, x + x_pos, y + x_y_pos, 2);
      nvgFill(nvg);

      nvgBeginPath(nvg);
      nvgCircle(nvg, x + x_pos, y + single_plot_height + y_y_pos, 2);
      nvgFill(nvg);

      const vec2& kpos = body->prev_kalmans[i];
      const float kx_y_pos =
        fl_map_range(kpos.x, 0, float(fl::KINECT_DEPTH_W), single_plot_height, 0);
      const float ky_y_pos =
        fl_map_range(kpos.y, float(fl::KINECT_DEPTH_H), 0, single_plot_height, 0);

      nvgFillColor(nvg, nvgRGBA(255, 255, 0, 220));

      nvgBeginPath(nvg);
      nvgCircle(nvg, x + x_pos, y + kx_y_pos, 2);
      nvgFill(nvg);

      nvgBeginPath(nvg);
      nvgCircle(nvg, x + x_pos, y + single_plot_height + ky_y_pos, 2);
      nvgFill(nvg);

      x_pos += spacing;
    }

    nvgRestore(nvg);
  }
}

void draw_hog_detections(fl_render_context* ctx, float x, float y,
  const follower_ctx* follower) {
  NVGcontext* nvg = ctx->nvg;

  for (const AABB& aabb : follower->hog_boxes) {
    const float w = aabb.bot_right.x - aabb.top_left.x;
    const float h = aabb.bot_right.y - aabb.top_left.y;
    draw_rect_outline(nvg, x + aabb.top_left.x, y + aabb.top_left.y, w, h, 1.f,
      nvgRGBA(191, 85, 236, 50));
  }

  for (const merged_aabb& node : follower->combined_hogs) {
    const float w = node.aabb.bot_right.x - node.aabb.top_left.x;
    const float h = node.aabb.bot_right.y - node.aabb.top_left.y;
    draw_rect_outline(nvg, x + node.aabb.top_left.x, y + node.aabb.top_left.y,
      w, h, 1.f, nvgRGBA(255, 0, 0, 255));
  }

  if (follower->has_target) {
    const float max_r = 15.f;
    const float r = follower->possible_target.time_to_live /
      follower->body_time_to_live * max_r;

    const vec2 target = aabb_center(&follower->possible_target.location);
    nvgBeginPath(nvg);
    nvgCircle(nvg, x + target.x, y + target.y, r * 0.8f);
    nvgFillColor(nvg, nvgRGBA(65, 131, 215, 255));
    nvgFill(nvg);

    const vec2 predicted_position = aabb_center(&follower->possible_target.kf_prediction);
    nvgBeginPath(nvg);
    nvgCircle(nvg, x + predicted_position.x, y + predicted_position.y, r);
    nvgFillColor(nvg, nvgRGBA(207, 0, 15, 255));
    nvgFill(nvg);
  }
}

void draw_depth_summary(fl_render_context* ctx, const ui_layout* layout,
  const follower_ctx* follower) {
  const float max_dist = float(MAX_RELIABLE_DIST);
  const float min_dist = float(MIN_RELIABLE_DIST);
  const float max_height = layout->depth_bar_height;
  const depth_window* window = &follower->depth_map;
  const float x = layout->toolbar_width;
  const float y = layout->kinect_image_height;

  NVGcontext* nvg = ctx->nvg;
  nvgBeginPath(nvg);
  nvgRect(nvg, x, y, layout->kinect_image_width, max_height);
  nvgFillColor(nvg, nvgRGBA(0, 179, 106, 255));
  nvgFill(nvg);

  for (size_t i = 0; i < window->size(); i++) {
    const float v = float(window->at(i));
    const float clamped = std::min(std::max(v, min_dist), max_dist);

    assert(clamped >= min_dist && clamped <= max_dist);

    const float height =
      max_height * (1.f - (clamped - min_dist) / (max_dist - min_dist));

    nvgBeginPath(nvg);
    nvgRect(nvg, x + float(i) * layout->depth_bar_width, y,
      layout->depth_bar_width, height);
    nvgFillColor(nvg, nvgRGBA(30, 30, 30, 255));
    nvgFill(nvg);
  }
}

void render(fl_render_context* ctx, ui_layout* layout,
  const follower_ctx* follower) {
  float ortho[16];
  bx::mtxOrtho(ortho, 0, layout->win_widthf, layout->win_heightf, 0, -1.0f,
    1.0f);
  bgfx::setViewTransform(0, 0, ortho);

  const float half_img_w = layout->kinect_image_width * 0.5f;
  const float half_img_h = layout->kinect_image_height * 0.5f;

  float transform[16];
  float translation[16];
  float scale[16];

  bx::mtxTranslate(translation, layout->toolbar_width + half_img_w, half_img_h,
    0);
  bx::mtxScale(scale, half_img_w, half_img_h, 0);
  bx::mtxMul(transform, scale, translation);
  bgfx::setTransform(transform);

  bgfx::setVertexBuffer(ctx->vbh);
  bgfx::setIndexBuffer(ctx->ibh);
  bgfx::setTexture(0, ctx->unif_tex_color, follower->depth_texture);
  bgfx::submit(0, ctx->tex_program);

  bx::mtxTranslate(translation, layout->toolbar_width + 3.f * half_img_w,
    3 * half_img_h, 0);
  bx::mtxMul(transform, scale, translation);
  bgfx::setTransform(transform);

  bgfx::setVertexBuffer(ctx->vbh);
  bgfx::setIndexBuffer(ctx->ibh);
  bgfx::setTexture(0, ctx->unif_tex_color, follower->color_texture);
  bgfx::submit(0, ctx->tex_program);

  bx::mtxTranslate(translation, layout->toolbar_width + 3.f * half_img_w,
    half_img_h, 0);
  bx::mtxMul(transform, scale, translation);
  bgfx::setTransform(transform);

  bgfx::setVertexBuffer(ctx->vbh);
  bgfx::setIndexBuffer(ctx->ibh);
  bgfx::setTexture(0, ctx->unif_tex_color, follower->infrared_texture);
  bgfx::submit(0, ctx->tex_program);

  nvgBeginFrame(ctx->nvg, layout->win_width, layout->win_height, 1.f);
  draw_depth_summary(ctx, layout, follower);
  draw_tracked_bodies(ctx, layout, follower);
  draw_hog_detections(ctx, layout->toolbar_width, 0.f, follower);
  draw_prediction_plots(ctx, layout, follower);
  nvgEndFrame(ctx->nvg);
}

} // namespace Fl