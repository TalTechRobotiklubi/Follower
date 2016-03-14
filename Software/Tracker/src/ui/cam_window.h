#ifndef CAM_WINDOW_H
#define CAM_WINDOW_H

#include <bgfx/bgfx.h>

struct follower_ctx;
struct NVGcontext;

struct cam_window {
  bool open;

  size_t texture_pitch;
  size_t depth_texture_len;
  uint8_t* depth_texture_data;

  bgfx::TextureHandle depth_texture;
  bgfx::TextureHandle color_texture;
};

void cam_window_init(cam_window* window);
void cam_window_update(cam_window* window, const struct kinect_frame* frame);
void cam_window_draw(cam_window* window, NVGcontext* nvg, const follower_ctx* follower);
void cam_window_destroy(cam_window* window);

#endif
