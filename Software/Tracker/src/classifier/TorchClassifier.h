#pragma once
#include <fhd_image.h>
#include <string>
#include <vector>
#include "Classifier.h"

struct lua_State;

struct TorchClassifier : public Classifier {
  TorchClassifier(const char* file);
  ~TorchClassifier();
  bool Init() override;
  bool Classify(const fhd_candidate* candidate) override;

  lua_State* L = nullptr;
  int classifierFn = 0;
  std::string fileName;
  std::vector<float> depthBuffer;
  fhd_image bufferImg;
};
