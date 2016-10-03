#pragma once

#include <GL/gl3w.h>
#include <stdint.h>

struct Texture {
  GLuint handle = 0;
  int width = 0;
  int height = 0;

  int Pitch() { return width * 4; }
  int Length() { return Pitch() * height; }
  bool Valid() const { return handle != 0; }
  void* PtrHandle() { return (void*)(intptr_t(handle)); }
};

Texture TextureAllocate(int width, int height);
void TextureUpdate(Texture* texture, const uint8_t* data, int width, int height);
void TextureDestroy(Texture* texture);
