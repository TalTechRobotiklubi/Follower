#ifndef POS_TEXCOORD_VERTEX
#define POS_TEXCOORD_VERTEX

#include <bgfx/bgfx.h>

struct pos_texcoord_vertex {
  float x;
  float y;
  float u;
  float v;

  static void init() {
    ms_decl.begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
  }

  static bgfx::VertexDecl ms_decl;
};

#endif
