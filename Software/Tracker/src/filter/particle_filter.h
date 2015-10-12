#ifndef PARTICLE_FILTER_H
#define PARTICLE_FILTER_H

#include "vec2.h"
#include <vector>
#include <random>

struct particle_filter {

  std::vector<vec2> particles;
  std::vector<float> weights;
  float measure_noise = 10.0f;
  float max_w = 0.0f;

  std::mt19937 rng_eng = std::mt19937(std::random_device()());
};

void pf_update(particle_filter* pf, vec2 position);
vec2 pf_sample(particle_filter* pf);


#endif
