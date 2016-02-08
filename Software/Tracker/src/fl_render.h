#ifndef FL_RENDER_H
#define FL_RENDER_H

struct follower_ctx;
struct NVGcontext;

namespace Fl {
struct fl_render_context;
struct ui_layout;

fl_render_context* fl_renderer_create(NVGcontext* nvg);
void render(fl_render_context* ctx, ui_layout* layout,
  const follower_ctx* follower);

}
#endif
