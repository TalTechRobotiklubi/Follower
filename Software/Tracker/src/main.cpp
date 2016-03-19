// Copyright 2016 TUT Robotics Club

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include <bx/timer.h>
#include <bx/commandline.h>

#include "msgpack.h"
#include "follower_ctx.h"
#include "kinect_frame_source.h"
#include "fl_sqlite_source.h"
#include "kinect_live_source.h"
#include "kinect_null_frame_source.h"
#include "imgui/imgui.h"
#include "imgui/droidsans.ttf.h"
#include "fl_math.h"
#include "nanovg/nanovg.h"
#include "ui/main_window.h"
#include "fl_constants.h"
#include "algorithm/algorithm_runner.h"
#include "ui/cam_window.h"
#include "comm/datahandler.h"
#include "hod/candidate_db.h"
#include "hod/hod_classifier.h"
#include "fl_sqlite_writer.h"

int main(int argc, char* argv[]) {
  Fl::window_info win_info;
  GLFWwindow* window = create_main_window(&win_info);

  bx::CommandLine cmd_line(argc, argv);

  fl_sqlite_writer* writer = NULL;
  const char* record_file = cmd_line.findOption('r');
  if (record_file) {
    writer = fl_sqlite_writer_create(record_file);
  }

  follower_ctx follower;
  win_info.user_data = &follower;

  candidate_db* candidates_database = NULL;
  if (const char* train_db = cmd_line.findOption('t')) {
    candidates_database = (candidate_db*)calloc(1, sizeof(candidate_db));
    candidate_db_init(candidates_database, train_db);
  }

  if (const char* database = cmd_line.findOption('d')) {
    printf("frame source: sqlite\n");
    fl_sqlite_source* source = new fl_sqlite_source(database);
    follower.frame_source.reset(source);
    if (candidates_database) source->manual_advance = true;
  } else {
#ifdef FL_KINECT_ENABLED
    printf("frame source: kinect\n");
    follower.frame_source.reset(new kinect_live_source());
#else
    printf("frame source: null\n");
    follower.frame_source.reset(new kinect_null_frame_source());
#endif
  }

  const char* serial_port = cmd_line.findOption('s');
  if (serial_port) follower.serial.start(serial_port, 115200);


  hod_debug hod_debugger;
  hod_debug_init(&hod_debugger, &follower.hod, candidates_database);
  follower.hod_debugger = &hod_debugger;

  hod_classifier classifier;
  if (const char* classifier_file = cmd_line.findOption('c')) {
    hod_classifier_init(&classifier, classifier_file);
    follower.hod.classifier = &classifier;
  } else {
    follower.hod.classifier = NULL;
  }

  imguiCreate();

  const uint8_t UI_VIEW_ID = 192;
  NVGcontext* nvg = nvgCreate(1, UI_VIEW_ID);
  nvgCreateFontMem(nvg, "default", (unsigned char*)s_droidSansTtf, INT32_MAX,
                   0);
  nvgFontSize(nvg, 12);
  nvgFontFace(nvg, "default");

  int64_t current_time = bx::getHPCounter();
  int64_t prev_time = current_time;

  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xE4F1FEFF, 1.0f,
                     0);

  double total_frame_time = 0.0;
  double smooth_frame_time = 0;

  AlgorithmRunner::initialize();

  cam_window cam_window;
  cam_window_init(&cam_window);

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

    const kinect_frame* frame = follower.frame_source->get_frame();

    if (writer && frame) {
      fl_sqlite_writer_add_frame(writer, frame);
    }

    if (frame) {
      if (frame->depth_data) {
        int64_t grid_sample_start = bx::getHPCounter();
        if (hod_debugger.update_enabled) {
          hod_grid_sample(&follower.hod, frame->depth_data);
        }
        int64_t grid_sample_end = bx::getHPCounter();
        hod_debugger.processing_time =
            double(grid_sample_end - grid_sample_start) * to_ms;
        hod_debug_update(&hod_debugger);

        follower.depth_map =
            calculate_range_map(frame->depth_data, fl::KINECT_DEPTH_W,
                                fl::KINECT_DEPTH_H, follower.depth_cutoff);

        follower_update_candidates(&follower, float(dt_seconds));
      }

      cam_window_update(&cam_window, frame);
    }

    follower.serial.receive(&follower.in_data);
    AlgorithmRunner::run(0, follower.in_data, &follower.out_data);
    DataHandler_TASK(20);
    follower.serial.send(follower.out_data);

    follower_update(&follower, float(dt_seconds));

    bgfx::setViewRect(0, 0, 0, win_info.win_width, win_info.win_height);
    bgfx::touch(0);

    nvgBeginFrame(nvg, win_info.win_width, win_info.win_height, 1.f);
    imguiBeginFrame(win_info.mouse.x, win_info.mouse.y, win_info.mouse.button,
                    win_info.mouse.scroll, win_info.win_width,
                    win_info.win_height, char(0), UI_VIEW_ID);
    nvgViewId(nvg, UI_VIEW_ID);

    bgfx::setViewName(UI_VIEW_ID, "UI");
    bgfx::setViewSeq(UI_VIEW_ID, true);

    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));

    bool w_open = true;
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::Begin("follower", &w_open, ImVec2(200.f, win_info.win_width), -1.f,
                 flags);
    ImGui::Text("frame time: %.1f ms", smooth_frame_time);
    ImGui::Text("camera %.1f %.1f", follower.out_data.camera_degrees.x,
                follower.out_data.camera_degrees.y);

    ImGui::Checkbox("HOD debug", &hod_debugger.window_open);
    ImGui::Checkbox("cameras", &cam_window.open);

    if (ImGui::Button("start algorithm")) {
      AlgorithmRunner::start(0, follower.in_data);
      follower.out_data.left_speed = 0;
      follower.out_data.right_speed = 0;
    }

    if (ImGui::Button("stop algorithm")) {
      AlgorithmRunner::stop(0);
      follower.out_data.left_speed = 0;
      follower.out_data.right_speed = 0;
    }

    ImGui::SliderInt("turn", &follower.in_data.degreesToTurn, -180, 180);

    cam_window_draw(&cam_window, nvg, &follower);
    hod_debug_draw(&hod_debugger, nvg);

    ImGui::End();

    imguiEndFrame();
    nvgEndFrame(nvg);

    bgfx::frame();
  }

  if (writer) {
    fl_sqlite_writer_destroy(writer);
  }

  cam_window_destroy(&cam_window);

  nvgDelete(nvg);
  imguiDestroy();
  bgfx::shutdown();
  glfwTerminate();

  candidate_db_close(candidates_database);

  return 0;
}
