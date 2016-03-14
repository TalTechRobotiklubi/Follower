#pragma once

struct hod_candidate;
struct fann;

struct hod_classifier {
  fann* nn;
};

void hod_classifier_init(hod_classifier* classifier, const char* nn_file);
float hod_classify(hod_classifier* classifier, const hod_candidate* candidate);
void hod_classifier_destroy(hod_classifier* classifier);
