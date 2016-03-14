#ifndef BGFX_IMGUI_TEXTURE_H
#define BGFX_IMGUI_TEXTURE_H

#include "imgui/imgui.h"

union bgfx_imgui_texture {
  ImTextureID ptr;
  struct {
    uint16_t flags;
    bgfx::TextureHandle handle;
  };
};

#endif
