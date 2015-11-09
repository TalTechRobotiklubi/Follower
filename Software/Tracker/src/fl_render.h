#ifndef FL_RENDER_H
#define FL_RENDER_H

struct follower_ctx;
struct fl_render_context;
struct fl_ui_layout;
struct NVGcontext;

fl_render_context* fl_renderer_create(NVGcontext* nvg);
void fl_render(fl_render_context* ctx, fl_ui_layout* layout,
               const follower_ctx* follower);

#endif
