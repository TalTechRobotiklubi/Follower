#include "hod_classifier.h"
#include "hod_candidate.h"
#include <fann.h>
#include <stdio.h>

void hod_classifier_init(hod_classifier* classifier, const char* nn_file) {
  classifier->nn = fann_create_from_file(nn_file);
}

float hod_classify(hod_classifier* classifier, const hod_candidate* candidate) {
  float* output = fann_run(classifier->nn, candidate->features);
  return output[0];
}

void hod_classifier_destroy(hod_classifier* classifier) {
  if (classifier->nn) {
    fann_destroy(classifier->nn);
  }
}
