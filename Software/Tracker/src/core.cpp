#include "core.h"
#include <fhd.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <lua.hpp>
#include "Clock.h"
#include "Encode.h"
#include "UdpHost.h"
#include "algorithm/algorithm_runner.h"
#include "comm/datalayer.h"
#include "core_opt.h"
#include "fl_constants.h"
#include "kinect_frame.h"
#include "kinect_frame_source.h"
#include "proto/frame_generated.h"
#include "proto/message_generated.h"

void kinect_loop(core* c) {
  while (c->running) {
    c->frame_source->get_frame();
  }
}

void core_start(core* c) {
  c->kinect_frame_thread = std::thread(kinect_loop, c);
}

void core_check(core* c, double dt) {
  if (true || c->timestamp > 1000.0 * 5.0) {
    c->coreState = kFind;
    return;
  }

  c->state.camera.x = 45.f * float(std::sin(c->timestamp / 100.0));
}

const char* const script = R"(
  local ffi = require("ffi")
  ffi.cdef[[
    typedef struct { float x, y; } vec2;
    typedef struct { float x, y, z; } vec3;
    typedef struct {
      vec2 kinectPosition;
      vec3 metricPosition;
      float weight;
    } Detection;
    typedef struct {
      double timestamp;
      int32_t numDetections;
      Detection detections[16];
    } World;
    typedef struct {
      vec2 camera;
    } ControlState;
  ]]

  context = {}
  function user_decide(dt, world, state)
    if context.targets == nil then
      context.targets = {}
    end

    for i, t in ipairs(context.targets) do
      t.timeToLive = t.timeToLive - dt
    end

    for i, t in pairs(context.targets) do
      io.write(i, "\n")
    end

    local closest_detection
    for i = 0, world.numDetections do
      local detection = world.detections[i]
      if detection.weight >= 1.0 then
        if closest_detection == nil then
          closest_detection = detection
        else
          if detection.metricPosition.z < closest_detection.metricPosition.z then
            closest_detection = detection
          end
        end
      end
    end

    if closest_detection ~= nil then
      local p = closest_detection.metricPosition
      local angle = math.atan(-p.x / p.z)
      state.camera.x = math.deg(angle)
    end
    return state
  end

  function decide(dt, world, state)
    world = ffi.cast("World*", world)
    state = ffi.cast("ControlState*", state)
    if user_decide ~= nil then
      local ok, new_state = pcall(user_decide, dt, world, state)
      if ok then
        return new_state
      else
        io.write("error calling user code: ", new_state, "\n")
      end
    end
  end
)";

void core_decide(core* c, double dt) {
  lua_getglobal(c->lua, "decide");
  lua_pushnumber(c->lua, dt);
  lua_pushlightuserdata(c->lua, &c->world);
  lua_pushlightuserdata(c->lua, &c->state);
  if (lua_pcall(c->lua, 3, 1, 0) == 0) {
  } else {
    printf("failed to call decide %s\n", lua_tostring(c->lua, -1));
  }
}

void core_detect(core* c, double timestamp) {
  if (!c->fhd) {
    return;
  }

  fhd_run_pass(c->fhd, c->current_frame.depth_data);

  c->world.timestamp = timestamp;
  c->world.numDetections = 0;

  // Flip the detection horizontally, Kinect 2's images have left-right
  // reversed.
  const float w = float(kDepthWidth);
  size_t numCandidates =
      std::min(size_t(c->fhd->candidates_len),
               sizeof(c->world.detections) / sizeof(Detection));

  for (int i = 0; i < numCandidates; i++) {
    const fhd_candidate* candidate = &c->fhd->candidates[i];
    vec2 kinectPos{w - candidate->kinect_position.x,
                   candidate->kinect_position.y};
    vec3 metricPos{candidate->metric_position.x, candidate->metric_position.y,
                   candidate->metric_position.z};
    c->world.detections[i] = Detection{kinectPos, metricPos, candidate->weight};
    c->world.numDetections++;
  }
}

void core_serial_send(core* c) {
  c->out_data.camera_degrees = c->state.camera;
  c->serial.send(c->out_data);
}

void core_serialize(core* c) {
  c->builder.Clear();
  auto depth =
      c->builder.CreateVector(c->encoded_depth.data, c->encoded_depth.len);
  std::vector<proto::Detection> detections;
  std::vector<proto::Target> targets;
  detections.reserve(c->world.numDetections);
  targets.reserve(c->tracking.numTargets);

  for (int32_t i = 0; i < c->world.numDetections; i++) {
    const Detection& detection = c->world.detections[i];
    detections.push_back(proto::Detection(
        proto::Vec2(detection.kinectPosition.x, detection.kinectPosition.y),
        proto::Vec3(detection.metricPosition.x, detection.metricPosition.y,
                    detection.metricPosition.z),
        detection.weight));
  }

  for (int32_t i = 0; i < c->tracking.numTargets; i++) {
    const Target& t = c->tracking.targets[i];
    targets.push_back(
        proto::Target(float(t.weight),
                      proto::Vec2(t.kinectPosition.x, t.kinectPosition.y),
                      proto::Vec3(t.metricPosition.x, t.metricPosition.y,
                                  t.metricPosition.z)));
  }

  auto detectionOffsets = c->builder.CreateVectorOfStructs(detections);
  auto targetOffsets = c->builder.CreateVectorOfStructs(targets);

  proto::Vec2 camera(c->state.camera.x, c->state.camera.y);
  proto::FrameBuilder frame_builder(c->builder);

  frame_builder.add_timestamp(c->timestamp);
  frame_builder.add_camera(&camera);
  frame_builder.add_depth(depth);
  frame_builder.add_detections(detectionOffsets);
  frame_builder.add_targets(targetOffsets);

  auto frame = frame_builder.Finish();
  c->builder.Finish(frame);
}

core::~core() { kinect_frame_thread.join(); }

int main(int argc, char** argv) {
  core c;
  c.lua = luaL_newstate();
  luaL_openlibs(c.lua);

  int scriptStatus = luaL_dostring(c.lua, script);
  if (scriptStatus) {
    printf("failed to load wrapper script: %s\n", lua_tostring(c.lua, -1));
  }

  c.current_frame.depth_data = (uint16_t*)calloc(kDepthWidth * kDeptHeight, 2);
  c.current_frame.depth_length = kDepthWidth * kDeptHeight;
  rgba_image_init(&c.rgba_depth, kDepthWidth, kDeptHeight);
  rgba_image_init(&c.prev_rgba_depth, kDepthWidth, kDeptHeight);
  ActiveMapReset(&c.rgba_depth_diff, kDepthWidth, kDeptHeight);

  c.encoder = EncoderCreate(kDepthWidth, kDeptHeight);

  if (!parse_opt(&c, argc, argv)) {
    return 1;
  }

  core_start(&c);

  double current_time = ms_now();
  double prev_time = current_time;
  while (c.running) {
    prev_time = current_time;
    current_time = ms_now();
    const double frame_time = current_time - prev_time;
    const double frameTimeSeconds = frame_time / 1000.0;
    c.timestamp += frame_time;

    c.frame_source->fill_frame(&c.current_frame);
    memcpy(c.prev_rgba_depth.data, c.rgba_depth.data, c.rgba_depth.bytes);

    core_detect(&c, current_time);

    depth_to_rgba(c.current_frame.depth_data, c.current_frame.depth_length,
                  &c.rgba_depth);
    BlockDiff(c.prev_rgba_depth.data, c.rgba_depth.data, kDepthWidth,
              kDeptHeight, &c.rgba_depth_diff);
    c.encoded_depth =
        EncodeImage(c.encoder, c.rgba_depth.data, &c.rgba_depth_diff);

    c.serial.receive(&c.in_data);

    switch (c.coreState) {
      case kCheck:
        core_check(&c, frameTimeSeconds);
        break;
      case kFind: {
        core_decide(&c, frameTimeSeconds);
        break;
      }
      default:
        break;
    }

    DL_task(int16_t(frame_time));
    core_serial_send(&c);
    core_serialize(&c);

    const IoVec* received = UdpHostPoll(c.udp);

    if (received) {
      auto message = proto::GetMessage(received->data);
      if (message) {
        switch (message->payload_type()) {
          case proto::Payload_LuaMainScript: {
            auto scriptMessage = (const proto::LuaMainScript*)message->payload();
            const char* remoteScript = scriptMessage->content()->c_str();
            printf("%s\n", remoteScript);
            int loadStatus = luaL_dostring(c.lua, remoteScript);
            if (loadStatus) {
              printf("failed to load wrapper script: %s\n", lua_tostring(c.lua, -1));
            } else {

            }
            break;
          } 
          default: break;
        }
      }
    }

    UdpHostBroadcast(c.udp, c.builder.GetBufferPointer(), c.builder.GetSize());
  }

  return 0;
}
