#include "cam_window.h"
#include "follower_ctx.h"
#include "bgfx_imgui_texture.h"
#include "fl_constants.h"
#include "fl_math.h"
#include "kinect_frame.h"
#include "nanovg/nanovg.h"
#include <string.h>

static uint8_t fl_depth_to_byte(uint16_t value, uint16_t min, uint16_t max) {
  const uint32_t v = fl_clamp(value, min, max);
  const uint32_t lmin = min;
  const uint32_t lmax = max;
  return static_cast<uint8_t>((v - lmin) * 255 / (lmax - lmin));
}

static void update_depth_texture(uint8_t* texture_data, const uint16_t* depth,
                                 size_t len) {
  uint8_t* pixelData = (uint8_t*)texture_data;
  for (uint32_t i = 0; i < len; i++) {
    uint16_t reading = depth[i];
    uint8_t normalized =
        fl_depth_to_byte(reading, fl::MIN_RELIABLE_DIST, fl::MAX_RELIABLE_DIST);

    uint32_t idx = 4 * i;

    if (reading >= fl::MIN_RELIABLE_DIST && reading <= fl::MAX_RELIABLE_DIST) {
      // depth grey
      pixelData[idx] = normalized;
      pixelData[idx + 1] = 255 - normalized;
      pixelData[idx + 2] = 255 - normalized;
    } else {
      // blue
      pixelData[idx] = 0;
      pixelData[idx + 1] = 0;
      pixelData[idx + 2] = 0;
    }
    pixelData[idx + 3] = 255;
  }
}

void cam_window_init(cam_window* window) {
  window->open = false;

  window->texture_pitch = fl::KINECT_DEPTH_W * 4;
  window->depth_texture_len = window->texture_pitch * fl::KINECT_DEPTH_H;
  window->depth_texture_data = (uint8_t*)calloc(window->depth_texture_len, 1);
  memset(window->depth_texture_data, 255, window->depth_texture_len);

  window->depth_texture = bgfx::createTexture2D(
      fl::KINECT_DEPTH_W, fl::KINECT_DEPTH_H, 1, bgfx::TextureFormat::RGBA8);
  window->color_texture = bgfx::createTexture2D(
      fl::KINECT_DEPTH_W, fl::KINECT_DEPTH_H, 1, bgfx::TextureFormat::BGRA8);
}

void cam_window_update(cam_window* window, const kinect_frame* frame) {
  if (frame->depth_data) {
    update_depth_texture(window->depth_texture_data, frame->depth_data,
                         frame->depth_length);
    bgfx::updateTexture2D(
        window->depth_texture, 0, 0, 0, fl::KINECT_DEPTH_W, fl::KINECT_DEPTH_H,
        bgfx::makeRef(window->depth_texture_data, window->depth_texture_len),
        window->texture_pitch);
  }

  if (frame->rgba_data) {
    const bgfx::Memory* color_mem =
        bgfx::makeRef(frame->rgba_data, uint32_t(frame->rgba_length));
    bgfx::updateTexture2D(window->color_texture, 0, 0, 0, frame->rgba_width,
                          frame->rgba_height, color_mem, frame->rgba_width * 4);
  }
}

void cam_window_draw(cam_window* window, NVGcontext* nvg,
                     const follower_ctx* follower) {
  if (!window->open) return;

  bgfx_imgui_texture depth_img;
  depth_img.handle = window->depth_texture;

  bgfx_imgui_texture color_img;
  color_img.handle = window->color_texture;

  ImGui::Begin("cameras", &window->open);
  ImVec2 pos = ImGui::GetCursorScreenPos();
  ImVec2 cs = ImGui::GetContentRegionAvail();
  const float visible_w = ImGui::GetScrollX() + cs.x;
  const float visible_h = ImGui::GetScrollY() + cs.y;

  if (visible_h > 0.f && visible_w > 0.f) {
    nvgStrokeWidth(nvg, 4.f);
    nvgStrokeColor(nvg, nvgRGBAf(1.f, 0.f, 1.f, 1.f));
    nvgScissor(nvg, pos.x, pos.y, visible_w, visible_h);

    for (int i = 0; i < follower->hod.candidates_len; i++) {
      const hod_candidate* candidate = &follower->hod.candidates[i];
      if (candidate->weight > 0.97f) {
        vec2 tl = {float(candidate->depth_position.x),
                   float(candidate->depth_position.y)};
        vec2 size = {float(candidate->depth_position.width),
                     float(candidate->depth_position.height)};

        nvgBeginPath(nvg);
        nvgRect(nvg, pos.x + tl.x, pos.y + tl.y, size.x, size.y);
        nvgStroke(nvg);
      }
    }

    if (follower->has_target) {
      const AABB* position = &follower->target.avg_position;
      vec2 size = aabb_size(position);
      vec2 center = aabb_center(position);
      nvgBeginPath(nvg);
      nvgRect(nvg, pos.x + position->top_left.x, pos.y + position->top_left.y, size.x, size.y);
      nvgStrokeColor(nvg, nvgRGBAf(1.f, 1.f, 0.f, 1.f));
      nvgStroke(nvg);

      nvgBeginPath(nvg);
      nvgCircle(nvg, pos.x + center.x, pos.y + center.y, (follower->target.time_to_live / 1.f) * 10.f);
      nvgFillColor(nvg, nvgRGBAf(1.f, 1.f, 1.f, 1.f));
      nvgFill(nvg);
    }

    nvgResetScissor(nvg);
  }

  ImGui::Image(depth_img.ptr, ImVec2(fl::KINECT_DEPTH_W, fl::KINECT_DEPTH_H));
  ImGui::Image(color_img.ptr, ImVec2(fl::KINECT_DEPTH_W, fl::KINECT_DEPTH_H));
  ImGui::End();
}

void cam_window_destroy(cam_window* window) {
  free(window->depth_texture_data);
  bgfx::destroyTexture(window->depth_texture);
  bgfx::destroyTexture(window->color_texture);
}
