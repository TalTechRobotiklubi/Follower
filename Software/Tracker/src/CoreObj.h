#pragma once
#include <stdint.h>
#include "Image.h"
#include "comm/comm_input.h"
#include "vec3.h"

struct Detection {
  vec2i depthTopLeft;
  vec2i depthBotRight;
  vec3 metricPosition;
  float weight;
  float histogram[768];
  RgbaImage color;
};

struct World {
  double timestamp = 0.0;
  int32_t numDetections = 0;
  Detection detections[16];
  uint8_t distance_sensors[NUM_OF_DISTANCE_SENSORS];
};

struct Target {
  float weight = 0.f;
  vec2 kinect;
  vec3 position;
  int32_t detectionIndex = -1;

  Target() = default;
  Target(float weight, vec2 kinect, vec3 position)
      : weight(weight), kinect(kinect), position(position) {}
};

enum Activity {
  // meant for Hw side, if packet not received with valid data or smt.
  Error = 0,
  // no activity, kinect down, sleeping emoticon on tft screen
  Resting = 1,
  // no target, moving and searches, emoticon with eyes looking around
  Searching = 2,
  // follows target, happy emoticon
  Following = 3
};

struct TrackingState {
  int32_t activeTarget = -1;
  int32_t numTargets = 0;
  Target targets[16];
  Activity activity;
};

struct ControlState {
  vec2 camera;
  float rotationSpeed = 0.f;
  float speed = 0.f;
};
