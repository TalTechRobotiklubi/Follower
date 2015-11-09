#include "particle_filter.h"
#define _USE_MATH_DEFINES
#include <math.h>

float gaussian(float mu, float sigma, float x) {
  const float variance = sigma * sigma;
  return std::exp(-std::pow(mu - x, 2) / variance * 0.5f) /
         std::sqrt(2.f * M_PI * variance);
}

void pf_update(particle_filter* pf, vec2 position) {
  const float noise = 10.f;
  std::normal_distribution<float> x_dist(position.x, noise);
  std::normal_distribution<float> y_dist(position.y, noise);
  for (vec2& p : pf->particles) {
    p.x = x_dist(pf->rng_eng);
    p.y = y_dist(pf->rng_eng);
  }

  pf->weights.clear();
  pf->max_w = 0.f;
  for (vec2 p : pf->particles) {
    float prob =
        gaussian(p.x, noise, position.x) * gaussian(p.y, noise, position.y);
    pf->weights.push_back(prob);

    if (prob > pf->max_w) {
      pf->max_w = prob;
    }
  }

  std::vector<vec2> sampled;
  sampled.reserve(pf->particles.size());

  std::uniform_int_distribution<size_t> dist(0, pf->particles.size());
  std::uniform_real_distribution<float> w_dist(0, 2.f * pf->max_w);
  size_t idx = dist(pf->rng_eng);
  float beta = 0.f;

  for (size_t i = 0; i < pf->particles.size(); i++) {
    beta += w_dist(pf->rng_eng);
    while (beta > pf->weights[idx]) {
      beta -= pf->weights[idx];
      idx = (idx + 1) % pf->particles.size();
    }

    sampled.push_back(pf->particles[idx]);
  }

  pf->particles = sampled;
}

vec2 pf_sample(particle_filter* pf) {
  vec2 mean = {0.0f, 0.0f};
  for (const vec2& p : pf->particles) {
    mean.x += p.x;
    mean.y += p.y;
  }
  const float count = float(pf->particles.size());
  mean.x /= count;
  mean.y /= count;
  return mean;
}
