#include <stdio.h>
#include <stdint.h>
#include <sstream>
#include "vec2.h"
#include "msgpack.h"
#include "follower_ctx.h"
#include "kinect_frame_source.h"
#include "kinect_record_source.h"
#include "kinect_live_source.h"
#include "kinect_null_frame_source.h"
#include "kinect_recording.h"
#include <boost/program_options.hpp>
#include <GLFW/glfw3.h>
#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>
#include <bx/timer.h>
#include <bx/fpumath.h>
#include "shaders.h"
#include "nanovg/nanovg.h"
#include "imgui/imgui.h"
#include "imgui/droidsans.ttf.h"
#include "pos_texcoord_vertex.h"
#include "hog_detect.h"
#include "fl_math.h"

#ifdef WIN32
#define snprintf _snprintf
#endif

const float CLIENT_TEXTURE_SCALE = 1.25f;
const float CLIENT_DEPTH_W = KINECT_DEPTH_W * CLIENT_TEXTURE_SCALE;
const float CLIENT_DEPTH_H = KINECT_DEPTH_H * CLIENT_TEXTURE_SCALE;
const float CLIENT_RGB_W = KINECT_DEPTH_W * CLIENT_TEXTURE_SCALE;
const float CLIENT_RGB_H = CLIENT_DEPTH_H;
const float DEPTH_BAR_HEIGHT = 50.f;
const float DEPTH_BAR_WIDTH = CLIENT_DEPTH_W / range_intervals;
const float X_PLOT_HEIGHT = 100;
const float Y_PLOT_HEIGHT = 100;
const float toolbar_width = 200.f;
const float WIN_WIDTHF = CLIENT_DEPTH_W + CLIENT_RGB_W + toolbar_width;
const float WIN_HEIGHTF = CLIENT_DEPTH_H + DEPTH_BAR_HEIGHT + X_PLOT_HEIGHT + Y_PLOT_HEIGHT;
const int win_width = int(WIN_WIDTHF);
const int win_height = int(WIN_HEIGHTF);

static pos_texcoord_vertex quad[4] = {
  { 0.0f, 0.0f, 0.0f, 0.0f },
  { CLIENT_DEPTH_W, 0.0f, 1.0f, 0.0f },
  { CLIENT_DEPTH_W, CLIENT_DEPTH_H, 1.0f, 1.0f },
  { 0.0f, CLIENT_DEPTH_H, 0.0f, 1.0f }
};

static const uint16_t quad_indices[6] = {
  0, 1, 2,
  2, 3, 0
};

namespace po = boost::program_options;

struct mouse_state {
  int32_t x = 0;
  int32_t y = 0;
  uint8_t button = 0;
  int32_t scroll = 0;
};

void draw_tracked_bodies(NVGcontext* nvg, float x, float y, const follower_ctx* follower) {

  const float border_width = 3.f;
  char tex_buf[32];
	for (const auto& kv : follower->bodies) {
    nvgSave(nvg);
		const body* body = &kv.second;
    const AABB* bbox = &body->kbody.body_box;
    const vec2 tl = vec2_mul(bbox->top_left, CLIENT_TEXTURE_SCALE);
    const vec2 br = vec2_mul(bbox->bot_right, CLIENT_TEXTURE_SCALE);
    const float w = br.x - tl.x;
    const float h = br.y - tl.y;

    // AABB
    nvgBeginPath(nvg);
    nvgRect(nvg, x + tl.x, y + tl.y, w, h);
    nvgStrokeWidth(nvg, border_width);
    nvgStrokeColor(nvg, nvgRGBA(255, 20, 0, 200));
    nvgStroke(nvg);

    // Textbox
    nvgBeginPath(nvg);
    nvgRect(nvg, x + tl.x, y + tl.y, 65.f, 40);
    nvgFillColor(nvg, nvgRGBA(0, 0, 0, 255));
    nvgFill(nvg);

    // Text
		snprintf(tex_buf, 32, "TTL: %.2f", body->time_to_live);
    nvgFillColor(nvg, nvgRGBA(225, 225, 225, 255));
    nvgTextAlign(nvg, NVG_ALIGN_TOP);
		nvgText(nvg, x + tl.x + border_width, y + tl.y + border_width, tex_buf, nullptr);
		snprintf(tex_buf, 32, "ID: %u", uint32_t(body->kbody.identifier));
		nvgText(nvg, x + tl.x + border_width, y + tl.y + border_width + 20, tex_buf, nullptr);


    // Kalman prediction
    nvgBeginPath(nvg);
    nvgCircle(nvg, x + body->filter.state(0) * CLIENT_TEXTURE_SCALE, y + body->filter.state(1) * CLIENT_TEXTURE_SCALE, 5);
    nvgFillColor(nvg, nvgRGBA(255, 255, 0, 220));
    nvgFill(nvg);

    // x axis plot
    const float spacing = 5.f;
    float x_pos = spacing;
    const size_t num_possible_points = body->prev_positions.size();
    size_t beginIdx = 0;

    const float render_width = float(num_possible_points) * spacing;
    if (render_width > CLIENT_DEPTH_W) {
      const size_t num_fitted_points = size_t(std::floor(CLIENT_DEPTH_W / spacing));
      beginIdx = num_possible_points - num_fitted_points;
    }

    const float x_axis_plot_begin_y = CLIENT_DEPTH_H + DEPTH_BAR_HEIGHT;
    const float y_axis_plot_begin_y = CLIENT_DEPTH_H + DEPTH_BAR_HEIGHT + X_PLOT_HEIGHT;
    for (size_t i = beginIdx; i < num_possible_points; i++) {
      const vec2& pos = body->prev_positions[i].top_left;
      const float x_y_pos = fl_map_range(pos.x, 0, 512, X_PLOT_HEIGHT, 0);
      const float y_y_pos = fl_map_range(pos.y, 0, 424, Y_PLOT_HEIGHT, 0);
      nvgFillColor(nvg, nvgRGBA(255, 255, 255, 220));

      nvgBeginPath(nvg);
      nvgCircle(nvg, x + x_pos, y + x_axis_plot_begin_y + x_y_pos, 2);
      nvgFill(nvg);
      nvgBeginPath(nvg);
      nvgCircle(nvg, x + x_pos, y + y_axis_plot_begin_y + y_y_pos, 2);
      nvgFill(nvg);

      const vec2& kpos = body->prev_kalmans[i];
      const float kx_y_pos = fl_map_range(kpos.x, 0, 512, X_PLOT_HEIGHT, 0);
      const float ky_y_pos = fl_map_range(kpos.y, 0, 424, Y_PLOT_HEIGHT, 0);

      nvgFillColor(nvg, nvgRGBA(255, 255, 0, 220));

      nvgBeginPath(nvg);
      nvgCircle(nvg, x + x_pos, y + x_axis_plot_begin_y + kx_y_pos, 2);
      nvgFill(nvg);
      nvgBeginPath(nvg);
      nvgCircle(nvg, x + x_pos, y + y_axis_plot_begin_y + ky_y_pos, 2);
      nvgFill(nvg);

      x_pos += spacing;
    }

    nvgRestore(nvg); 
	}

  for (const AABB& aabb : follower->hog_boxes) {
    const float w = aabb.bot_right.x - aabb.top_left.x;
    const float h = aabb.bot_right.y - aabb.top_left.y;
    nvgSave(nvg);
    nvgBeginPath(nvg);
    nvgRect(nvg, x + aabb.top_left.x, y + aabb.top_left.y, w, h);
    nvgStrokeWidth(nvg, 2.f);
    nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 200));
    nvgStroke(nvg);
    nvgRestore(nvg);
  }
}

void draw_depth_summary(NVGcontext* nvg, float x, float y, const follower_ctx* follower) {

  const float max_dist = float(MAX_RELIABLE_DIST);
  const float min_dist = float(MIN_RELIABLE_DIST);
  const float max_height = float(DEPTH_BAR_HEIGHT);
	const depth_window* window = &follower->depth_map;

  nvgBeginPath(nvg);
  nvgRect(nvg, x, y, CLIENT_DEPTH_W, DEPTH_BAR_HEIGHT);
  nvgFillColor(nvg, nvgRGBA(0, 179, 106, 255));
  nvgFill(nvg);

	for (size_t i = 0; i < window->size(); i++) {

    const float v = float(window->at(i));
    const float clamped = std::min(std::max(v, min_dist), max_dist);

    assert(clamped >= min_dist && clamped <= max_dist);

    const float height = max_height * (1.f - (clamped - min_dist) / (max_dist - min_dist));

    nvgBeginPath(nvg);
    nvgRect(nvg, x + i * DEPTH_BAR_WIDTH, y, DEPTH_BAR_WIDTH, height);
    nvgFillColor(nvg, nvgRGBA(30, 30, 30, 255));
    nvgFill(nvg);
	}
}

void update_depth_texture(uint8_t* texture_data, const kinect_frame* frame) {

	uint8_t* pixelData = (uint8_t*)texture_data;
	for (uint32_t i = 0; i < frame->depth_length; i++) {
		uint16_t reading = frame->depth_data[i];
		uint8_t normalized = fl_depth_to_byte(reading, MIN_RELIABLE_DIST, MAX_RELIABLE_DIST);

		uint32_t idx = 4 * i;

		if (reading >= MIN_RELIABLE_DIST && reading <= MAX_RELIABLE_DIST) {
			// depth grey
			pixelData[idx] = normalized;
			pixelData[idx + 1] = normalized;
			pixelData[idx + 2] = normalized;
		}
		else {
			// blue
			pixelData[idx] = 0;
			pixelData[idx + 1] = 0;
			pixelData[idx + 2] = 255;
			pixelData[idx + 2] = 0;
		}
		pixelData[idx + 3] = 255;

	}
}

po::variables_map get_command_line_args(int argc, char** argv) {
	po::options_description desc;
	desc.add_options()
    ("help,h", "print help message")
		("record,r", po::value<std::string>(), "record kinect frames to a file")
		("frames,f", po::value<uint32_t>(), "number of frames to record")
		("playback,p", po::value<std::string>(), "playback file")
		("serial,s", po::value<std::string>(), "serial port (COM0, /dev/ttyS0)")
    ("speedup,x", po::value<float>(), "record playback speedup")
    ("cascade,c", po::value<std::string>(), "hog cascade");
	po::variables_map vm;

	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (std::exception& e) {
		printf("args error: %s\n", e.what());
	}

  if (vm.count("help")) {
    std::stringstream msg;
    desc.print(msg);
    printf("%s\n", msg.str().c_str());
    exit(0);
  }

	return vm;
}

void glfwErrorCallback(int error, const char* description) {
  fprintf(stderr, "GLFW error %i: %s\n", error, description);
}

void on_key(GLFWwindow* window, int key, int, int action, int) {
  switch (key) {
    case GLFW_KEY_ESCAPE:
      if (action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
      break;
    default: break;
  }
}

void on_mouse_move(GLFWwindow* window, double x, double y) {
  mouse_state* mouse = (mouse_state*)glfwGetWindowUserPointer(window);
  mouse->x = int32_t(x);
  mouse->y = int32_t(y);
}

void on_mouse_button(GLFWwindow* window, int button, int action, int) {
  mouse_state* mouse = (mouse_state*)glfwGetWindowUserPointer(window);
  
  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      mouse->button ^= (-action ^ mouse->button) & IMGUI_MBUT_LEFT;
      break;
    default:
      break;
  }

}

int main(int argc, char *argv[]) {

  glfwSetErrorCallback(glfwErrorCallback);
  glfwInit();

  mouse_state mouse;

  GLFWwindow* window = glfwCreateWindow(
    win_width,
    win_height,
    "Follower",
    nullptr,
    nullptr
  );

  glfwSetWindowUserPointer(window, &mouse);
  glfwSetKeyCallback(window, on_key);
  glfwSetCursorPosCallback(window, on_mouse_move);
  glfwSetMouseButtonCallback(window, on_mouse_button);

  bgfx::glfwSetWindow(window);
	bgfx::init(bgfx::RendererType::OpenGL);
  bgfx::reset(win_width, win_height, BGFX_RESET_VSYNC);
  bgfx::setDebug(BGFX_DEBUG_TEXT);

	po::variables_map options = get_command_line_args(argc, argv);

	recording_ctx recording_context;
	if (options.count("record") && options.count("frames")) {
		const std::string file = options["record"].as<std::string>();
		const uint32_t frames = options["frames"].as<uint32_t>();
		recording_begin(&recording_context, file.c_str(), frames);
	}

	follower_ctx follower;

	if (options.count("playback")) {
		const std::string file = options["playback"].as<std::string>();
		printf("frame source: file [%s]\n", file.c_str());

		FILE* record_file = fopen(file.c_str(), "rb");
		fseek(record_file, 0, SEEK_END);
		size_t record_size = ftell(record_file);
		fseek(record_file, 0, SEEK_SET);
		auto record_data = std::unique_ptr<uint8_t[]>(new uint8_t[record_size]);
		size_t bytes_read = fread(record_data.get(), sizeof(uint8_t), record_size, record_file);

		printf("record file size: %.2f MB\n", (float)bytes_read / 1000000.f);

		fclose(record_file);

		follower.frame_source.reset(new kinect_record_source(std::move(record_data)));
	}
	else {
		printf("frame source: kinect\n");
#if 0 //def WIN32
		follower.frame_source.reset(new kinect_live_source());
#else
    follower.frame_source.reset(new kinect_null_frame_source());
#endif
	}

	if (options.count("serial")) {
		const std::string port = options["serial"].as<std::string>();
    follower_begin_serial(&follower, port, 115200);
	}

  float record_speedup = 1.f;
  if (options.count("speedup")) {
    record_speedup = options["speedup"].as<float>();
  }

  if (options.count("cascade")) {
    hog_detect* hog = hog_alloc();
    hog_init(hog);
    hog_load_cascade(hog, options["cascade"].as<std::string>().c_str());
    follower.hog = hog;
  }

  bgfx::TextureHandle depth_texture = bgfx::createTexture2D(
    KINECT_DEPTH_W,
    KINECT_DEPTH_H,
    1, 
    bgfx::TextureFormat::RGBA8
  );

	bgfx::TextureHandle color_texture = bgfx::createTexture2D(
		KINECT_DEPTH_W,
		KINECT_DEPTH_H,
		1,
		bgfx::TextureFormat::RGBA8
	);

  // rgba
  const size_t texture_pitch = KINECT_DEPTH_W * 4;
  const size_t texture_bytes = texture_pitch * KINECT_DEPTH_H;
  uint8_t* texture_data = (uint8_t*)malloc(texture_bytes);

  const bgfx::Memory* mem = bgfx::makeRef(texture_data, texture_bytes);

  bgfx::updateTexture2D(depth_texture, 0, 0, 0, KINECT_DEPTH_W, KINECT_DEPTH_H, mem, texture_pitch);

  int64_t current_time = bx::getHPCounter();
  int64_t prev_time = current_time;
	double total_frame_time = 0.0;

  bgfx::setViewClear(
    0,
    BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
    0,
    1.0f,
    0
  );

  imguiCreate();

  pos_texcoord_vertex::init();

  bgfx::UniformHandle unif_tex_color = bgfx::createUniform(
    "u_texColor",
    bgfx::UniformType::Int1
  );

  bgfx::ShaderHandle vsh = bgfx::createShader(
    bgfx::makeRef(vs_pos_tex_color, sizeof(vs_pos_tex_color))
  );

  bgfx::ShaderHandle fsh = bgfx::createShader(
    bgfx::makeRef(fs_pos_tex_color, sizeof(fs_pos_tex_color))
  );

  bgfx::ProgramHandle tex_program = bgfx::createProgram(vsh, fsh, true);

  bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
    bgfx::makeRef(quad, sizeof(quad)),
    pos_texcoord_vertex::ms_decl
  );
  bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
    bgfx::makeRef(quad_indices, sizeof(quad_indices))
  );

  NVGcontext* nvg = nvgCreate(1, 0);
 	nvgCreateFontMem(nvg, "default", (unsigned char*)s_droidSansTtf, INT32_MAX, 0);
 	nvgFontSize(nvg, 12);
 	nvgFontFace(nvg, "default");
  bgfx::setViewSeq(0, true);

  double smooth_frame_time = 0;
	while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

		prev_time = current_time;
		current_time = bx::getHPCounter();
    const int64_t frame_time = current_time - prev_time;
    const double freq = double(bx::getHPFrequency());
    const double to_ms = 1000.0 / freq;
    const double dt = double(frame_time) * to_ms; // ms
    const double dt_seconds = dt / 1000.0f;
    smooth_frame_time = 0.95 * smooth_frame_time + 0.05 * dt;
		total_frame_time += dt_seconds;

		const kinect_frame* frame = follower.frame_source->get_frame(float(dt_seconds) * record_speedup);
		
		if (frame) {
			if (recording_context.recording()) {
				recording_add_frame(&recording_context, frame, float(total_frame_time));
			}

      update_depth_texture(texture_data, frame);
      const bgfx::Memory* m = bgfx::makeRef(texture_data, texture_bytes);
      bgfx::updateTexture2D(depth_texture, 0, 0, 0, KINECT_DEPTH_W, KINECT_DEPTH_H, m, texture_pitch);

			// update color texture
			const bgfx::Memory* color_mem = bgfx::makeRef(frame->rgb_data, uint32_t(frame->rgb_length));
			bgfx::updateTexture2D(color_texture, 0, 0, 0, frame->rgb_width, frame->rgb_height, color_mem, frame->rgb_width * 4);

			follower.depth_map = calculate_range_map(
        frame->depth_data,
        KINECT_DEPTH_W,
        KINECT_DEPTH_H,
        follower.depth_cutoff
      );

			if (frame->num_bodies > 0) {
        follower_kinect_update(&follower, frame);
			}

      if (follower.hog) {
        hog_result results = hog_do_detect(follower.hog, frame->rgb_data, frame->rgb_width, frame->rgb_height);
        follower.hog_boxes.clear();
				const float scale_x = float(KINECT_DEPTH_W) / float(frame->rgb_width);
				const float scale_y = float(KINECT_DEPTH_H) / float(frame->rgb_height);
        for (size_t i = 0; i < results.len; i++) {
					AABB aabb = results.boxes[i];
					aabb.top_left.x *= scale_x;
					aabb.top_left.y *= scale_y;

					aabb.bot_right.x *= scale_x;
					aabb.bot_right.y *= scale_y;
          follower.hog_boxes.push_back(aabb);
        }
      }
      
      if (follower_has_serial(&follower)) {
        uint8_t depth_field_msg[DEPTH_FIELD_LENGTH];
        fill_depth_field_message(
          follower.depth_map.data(),
          follower.depth_map.size(),
          depth_field_msg
        );
        send_serial_message(&follower, depth_field_msg, DEPTH_FIELD_LENGTH);

        for (size_t i = 0; i < frame->num_bodies; i++) {
          const AABB* bbox = &frame->bodies[i].body_box;
          uint8_t bodypos_msg[BODYPOS_MSG_LENGTH];
          fill_bodypos_message(
            bbox->top_left.x,
            bbox->top_left.y,
            bbox->bot_right.x,
            bbox->bot_right.y,
            bodypos_msg
          );
          send_serial_message(&follower, bodypos_msg, BODYPOS_MSG_LENGTH);
        }
      }
		}

		// Disable rendering when recording to avoid skipped frames
		if (recording_context.recording()) continue;

    follower_update(&follower, float(dt_seconds) * record_speedup);

    bgfx::touch(0);

    float ortho[16];
		bx::mtxOrtho(ortho, 0, WIN_WIDTHF, WIN_HEIGHTF, 0, -1.0f, 1.0f);
    bgfx::setViewTransform(0, 0, ortho);

    float transform[16];
    bx::mtxTranslate(transform, toolbar_width, 0, 0);
    bgfx::setTransform(transform);

    bgfx::setVertexBuffer(vbh);
    bgfx::setIndexBuffer(ibh);
    bgfx::setTexture(0, unif_tex_color, depth_texture);
    bgfx::submit(0, tex_program);

		bx::mtxTranslate(transform, toolbar_width + CLIENT_DEPTH_W, 0, 0);
		bgfx::setTransform(transform);

		bgfx::setVertexBuffer(vbh);
		bgfx::setIndexBuffer(ibh);
		bgfx::setTexture(0, unif_tex_color, color_texture);
		bgfx::submit(0, tex_program);


    nvgBeginFrame(nvg, win_width, win_height, 1.f);
		draw_depth_summary(nvg, toolbar_width, CLIENT_DEPTH_H, &follower);
		draw_tracked_bodies(nvg, toolbar_width, 0, &follower);
    nvgEndFrame(nvg);


    static int32_t right_scroll_area = 0;
    imguiBeginFrame(mouse.x, mouse.y, mouse.button, 0, win_width, win_height);
		imguiLabel("frame time: %.1f ms", smooth_frame_time);
    imguiBeginScrollArea("Settings", 0, 20, int(toolbar_width), win_height, &right_scroll_area);
    imguiSlider("playback rate", record_speedup, 0.0f, 10.0f, 0.1f);
    imguiSlider("max body ttl", follower.body_time_to_live, 0.0f, 5.0f, 0.1f);
    imguiEndScrollArea();
    imguiEndFrame();

    bgfx::frame();
	}

	if (recording_context.recording()) {
		recording_end(&recording_context);
	}

  bgfx::shutdown();
  glfwTerminate();
	
	return 0;
}
