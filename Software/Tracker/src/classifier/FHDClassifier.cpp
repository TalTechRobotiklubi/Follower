#include "FHDClassifier.h"
#include <fhd_classifier.h>

FHDClassifier::FHDClassifier(const char* file) : fileName(file) {}

FHDClassifier::~FHDClassifier() {
  if (classifier) {
    fhd_classifier_destroy(classifier);
  }
}

bool FHDClassifier::Init() {
  classifier = fhd_classifier_create(fileName.c_str());
  if (!classifier) {
    printf("FHDClassifier: could not open classifier: %s\n", fileName.c_str());
    return false;
  }

  printf("FHDClassifier: using classifier - %s\n", fileName.c_str());
  return true;
}

bool FHDClassifier::Classify(const fhd_candidate* candidate) {
  const float weight = fhd_classify(classifier, candidate);
  return weight >= 1.f;
}
