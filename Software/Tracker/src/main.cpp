#include <stdio.h>
#include <stdint.h>
#include "msgpack.h"
#include "follower_ctx.h"
#include "kinect_frame_source.h"
#include "fl_kinect_stream_source.h"
#include "kinect_live_source.h"
#include "kinect_null_frame_source.h"
#include <GLFW/glfw3.h>
#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>
#include <bx/timer.h>
#include <bx/commandline.h>
#include "imgui/imgui.h"
#include "imgui/droidsans.ttf.h"
#include "pos_texcoord_vertex.h"
#include "hog_detect.h"
#include "fl_math.h"
#include "fl_render.h"
#include "nanovg/nanovg.h"
#include "fl_ui_layout.h"
#include "fl_constants.h"
#include "fl_stream_writer.h"
#include <math.h>

struct mouse_state {
  int32_t x = 0;
  int32_t y = 0;
  uint8_t button = 0;
  int32_t scroll = 0;
};

struct window_info {
  mouse_state mouse;
  fl_ui_layout layout;
};

void update_depth_texture(uint8_t* texture_data, const kinect_frame* frame) {
  uint8_t* pixelData = (uint8_t*)texture_data;
  for (uint32_t i = 0; i < frame->depth_length; i++) {
    uint16_t reading = frame->depth_data[i];
    uint8_t normalized =
        fl_depth_to_byte(reading, MIN_RELIABLE_DIST, MAX_RELIABLE_DIST);

    uint32_t idx = 4 * i;

    if (reading >= MIN_RELIABLE_DIST && reading <= MAX_RELIABLE_DIST) {
      // depth grey
      pixelData[idx] = normalized;
      pixelData[idx + 1] = normalized;
      pixelData[idx + 2] = normalized;
    } else {
      // blue
      pixelData[idx] = 0;
      pixelData[idx + 1] = 0;
      pixelData[idx + 2] = 255;
    }
    pixelData[idx + 3] = 255;
  }
}

void update_ir_texture(uint8_t* texture_data, const kinect_frame* frame) {
  uint8_t* pixelData = (uint8_t*)texture_data;
  for (uint32_t i = 0; i < frame->infrared_length; i++) {
    const float reading = float(frame->infrared_data[i]);
    const uint8_t gamma_corrected =
        uint16_t(powf(reading / 65535.f, 1.f / 2.2f) * 65535.f) >> 8;
    const uint32_t idx = 4 * i;

    pixelData[idx] = gamma_corrected;
    pixelData[idx + 1] = gamma_corrected;
    pixelData[idx + 2] = gamma_corrected;
    pixelData[idx + 3] = 255;
  }
}

void glfwErrorCallback(int error, const char* description) {
  fprintf(stderr, "GLFW error %i: %s\n", error, description);
}

void on_win_resize(GLFWwindow* window, int w, int h) {
  window_info* win_info = (window_info*)glfwGetWindowUserPointer(window);
  fl_ui_layout_calculate(&win_info->layout, w, h);
  bgfx::reset(w, h, BGFX_RESET_VSYNC);
}

void on_key(GLFWwindow* window, int key, int, int action, int) {
  switch (key) {
    case GLFW_KEY_ESCAPE:
      if (action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
      break;
    default:
      break;
  }
}

void on_mouse_move(GLFWwindow* window, double x, double y) {
  window_info* win_info = (window_info*)glfwGetWindowUserPointer(window);
  win_info->mouse.x = int32_t(x);
  win_info->mouse.y = int32_t(y);
}

void on_mouse_button(GLFWwindow* window, int button, int action, int) {
  window_info* win_info = (window_info*)glfwGetWindowUserPointer(window);

  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      win_info->mouse.button ^=
          (-action ^ win_info->mouse.button) & IMGUI_MBUT_LEFT;
      break;
    default:
      break;
  }
}

int main(int argc, char* argv[]) {
  glfwSetErrorCallback(glfwErrorCallback);
  glfwInit();

  window_info win_info;
  fl_ui_layout_calculate(&win_info.layout, 1400, 800);

  GLFWwindow* window =
      glfwCreateWindow(win_info.layout.win_width, win_info.layout.win_height,
                       "Follower", nullptr, nullptr);

  glfwSetWindowUserPointer(window, &win_info);
  glfwSetWindowSizeCallback(window, on_win_resize);
  glfwSetKeyCallback(window, on_key);
  glfwSetCursorPosCallback(window, on_mouse_move);
  glfwSetMouseButtonCallback(window, on_mouse_button);

  bgfx::glfwSetWindow(window);
  bgfx::init(bgfx::RendererType::OpenGL);
  bgfx::reset(win_info.layout.win_width, win_info.layout.win_height,
              BGFX_RESET_VSYNC);
  bgfx::setDebug(BGFX_DEBUG_TEXT);

  bx::CommandLine cmd_line(argc, argv);

  fl_stream_writer* stream_writer = nullptr;
  const char* record_file = cmd_line.findOption('r');
  if (record_file) {
    stream_writer = fl_stream_writer_create(record_file);
  }

  follower_ctx follower;

  const char* playback_file = cmd_line.findOption('p');
  if (playback_file) {
    follower.frame_source.reset(new fl_kinect_stream_source(playback_file));
  } else {
    printf("frame source: kinect\n");
#ifdef WIN32
    follower.frame_source.reset(new kinect_live_source());
#else
    follower.frame_source.reset(new kinect_null_frame_source());
#endif
  }

  const char* serial_port = cmd_line.findOption('s');
  if (serial_port) {
    follower_begin_serial(&follower, serial_port, 115200);
  }

  float record_speedup = 1.f;
  cmd_line.hasArg(record_speedup, 'x', "speedup");

  const char* cascade_file = cmd_line.findOption('c');
  if (cascade_file) {
    hog_detect* hog = hog_alloc();
    hog_init(hog);
    hog_load_cascade(hog, cascade_file);
    follower.hog = hog;
  }

  pos_texcoord_vertex::init();

  follower.depth_texture = bgfx::createTexture2D(
      fl::KINECT_DEPTH_W, fl::KINECT_DEPTH_H, 1, bgfx::TextureFormat::RGBA8);

  follower.color_texture = bgfx::createTexture2D(
      fl::KINECT_DEPTH_W, fl::KINECT_DEPTH_H, 1, bgfx::TextureFormat::BGRA8);

  follower.infrared_texture = bgfx::createTexture2D(
      fl::KINECT_IR_W, fl::KINECT_IR_H, 1, bgfx::TextureFormat::RGBA8);

  imguiCreate();

  NVGcontext* nvg = nvgCreate(1, 0);
  nvgCreateFontMem(nvg, "default", (unsigned char*)s_droidSansTtf, INT32_MAX,
                   0);
  nvgFontSize(nvg, 12);
  nvgFontFace(nvg, "default");
  bgfx::setViewSeq(0, true);

  fl_render_context* renderer = fl_renderer_create(nvg);

  // rgba
  const size_t texture_pitch = fl::KINECT_DEPTH_W * 4;
  const size_t texture_bytes = texture_pitch * fl::KINECT_DEPTH_H;
  uint8_t* depth_texture_data = (uint8_t*)malloc(texture_bytes);
  uint8_t* ir_texture_data = (uint8_t*)malloc(texture_bytes);

  int64_t current_time = bx::getHPCounter();
  int64_t prev_time = current_time;
  double total_frame_time = 0.0;

  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.0f, 0);

  double smooth_frame_time = 0;

  bool ir_processing_enabled = true;
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    prev_time = current_time;
    current_time = bx::getHPCounter();
    const int64_t frame_time = current_time - prev_time;
    const double freq = double(bx::getHPFrequency());
    const double to_ms = 1000.0 / freq;
    const double dt = double(frame_time) * to_ms;  // ms
    const double dt_seconds = dt / 1000.0f;
    smooth_frame_time = 0.95 * smooth_frame_time + 0.05 * dt;
    total_frame_time += dt_seconds;

    const kinect_frame* frame =
        follower.frame_source->get_frame(float(dt_seconds) * record_speedup);

    if (stream_writer && frame) {
      fl_stream_add_kframe(stream_writer, frame, float(total_frame_time));
      continue;
    }

    if (frame) {
      if (frame->depth_data) {
        update_depth_texture(depth_texture_data, frame);
        const bgfx::Memory* m =
            bgfx::makeRef(depth_texture_data, texture_bytes);
        bgfx::updateTexture2D(follower.depth_texture, 0, 0, 0,
                              fl::KINECT_DEPTH_W, fl::KINECT_DEPTH_H, m,
                              texture_pitch);

        follower.depth_map =
            calculate_range_map(frame->depth_data, fl::KINECT_DEPTH_W,
                                fl::KINECT_DEPTH_H, follower.depth_cutoff);
      }

      if (frame->rgba_data) {
        const bgfx::Memory* color_mem =
            bgfx::makeRef(frame->rgba_data, uint32_t(frame->rgba_length));
        bgfx::updateTexture2D(follower.color_texture, 0, 0, 0,
                              frame->rgba_width, frame->rgba_height, color_mem,
                              frame->rgba_width * 4);
      }

      if (ir_processing_enabled && frame->infrared_data) {
        update_ir_texture(ir_texture_data, frame);
        const bgfx::Memory* ir_mem =
            bgfx::makeRef(ir_texture_data, texture_bytes);
        bgfx::updateTexture2D(follower.infrared_texture, 0, 0, 0,
                              fl::KINECT_IR_W, fl::KINECT_IR_H, ir_mem,
                              fl::KINECT_IR_W * 4);
      }

      if (frame->num_bodies > 0) {
        follower_kinect_update(&follower, frame);
      }

      if (follower.hog) {
        hog_result results =
            hog_do_detect(follower.hog, frame->rgba_data, frame->rgba_width,
                          frame->rgba_height);
        follower.hog_boxes.clear();
        const float scale_x = float(win_info.layout.kinect_image_width) /
                              float(frame->rgba_width);
        const float scale_y = float(win_info.layout.kinect_image_height) /
                              float(frame->rgba_height);
        for (size_t i = 0; i < results.len; i++) {
          AABB aabb = results.boxes[i];
          aabb.top_left.x *= scale_x;
          aabb.top_left.y *= scale_y;

          aabb.bot_right.x *= scale_x;
          aabb.bot_right.y *= scale_y;
          follower.hog_boxes.push_back(aabb);
        }

        follower_update_possible_position(&follower, follower.hog_boxes.data(),
                                          follower.hog_boxes.size());
      }

      if (follower_has_serial(&follower)) {
        uint8_t depth_field_msg[DEPTH_FIELD_LENGTH];
        fill_depth_field_message(follower.depth_map.data(),
                                 follower.depth_map.size(), depth_field_msg);
        send_serial_message(&follower, depth_field_msg, DEPTH_FIELD_LENGTH);

        if (follower.has_target) {
          uint8_t bodypos_msg[BODYPOS_MSG_LENGTH];
          const AABB* target = &follower.possible_target;
          fill_bodypos_message(target->top_left.x, target->top_left.y,
                               target->bot_right.x, target->bot_right.y,
                               bodypos_msg);
          send_serial_message(&follower, bodypos_msg, BODYPOS_MSG_LENGTH);
        }
      }
    }

    follower_update(&follower, float(dt_seconds) * record_speedup);

    bgfx::touch(0);

    fl_render(renderer, &win_info.layout, &follower);

    static int32_t right_scroll_area = 0;
    imguiBeginFrame(win_info.mouse.x, win_info.mouse.y, win_info.mouse.button,
                    0, win_info.layout.win_width, win_info.layout.win_height);
    imguiLabel("frame time: %.1f ms", smooth_frame_time);
    imguiBeginScrollArea("Settings", 0, 20, int(win_info.layout.toolbar_width),
                         win_info.layout.win_height, &right_scroll_area);
    imguiSlider("playback rate", record_speedup, 0.0f, 10.0f, 0.1f);
    imguiSlider("max body ttl", follower.body_time_to_live, 0.0f, 5.0f, 0.1f);
    imguiBool("IR processing", ir_processing_enabled);
    imguiEndScrollArea();
    imguiEndFrame();

    bgfx::frame();
  }

  if (stream_writer) {
    fl_stream_writer_destroy(stream_writer);
  }

  bgfx::shutdown();
  glfwTerminate();

  return 0;
}
