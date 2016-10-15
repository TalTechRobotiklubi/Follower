#include "Texture.h"
#include <stdio.h>
#include <stdlib.h>

Texture TextureAllocate(int width, int height) {
  Texture t;
  t.width = width;
  t.height = height;

  glGenTextures(1, &t.handle);
  glBindTexture(GL_TEXTURE_2D, t.handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA,
               GL_UNSIGNED_BYTE, NULL);

  return t;
}

void TextureUpdate(Texture* texture, const uint8_t* data, int width,
                   int height) {
  if (!texture->Valid()) {
    glGenTextures(1, &texture->handle);
    glBindTexture(GL_TEXTURE_2D, texture->handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    glBindTexture(GL_TEXTURE_2D, texture->handle);
  }

  if (texture->width != width || texture->height != height) {
    texture->width = width;
    texture->height = height;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, NULL);

  } else {
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA,
                    GL_UNSIGNED_BYTE, data);
  }
}

void TextureDestroy(Texture* texture) { glDeleteTextures(1, &texture->handle); }
