#pragma once

struct fhd_candidate;

struct Classifier {
  virtual bool Init() = 0;
  virtual bool Classify(const fhd_candidate* candidate) = 0;
  virtual ~Classifier() {}
};
