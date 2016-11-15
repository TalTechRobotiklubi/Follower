#pragma once
#include <string>
#include "Classifier.h"

struct fhd_classifier;

struct FHDClassifier : public Classifier {
  FHDClassifier(const char* file);
  ~FHDClassifier();
  bool Init() override;
  bool Classify(const fhd_candidate* candidate) override;

  std::string fileName;
  fhd_classifier* classifier = nullptr;
};
