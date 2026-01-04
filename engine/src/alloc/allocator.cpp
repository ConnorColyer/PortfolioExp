#include "alloc/allocator.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace engine::alloc {

Allocator::Allocator(AllocationConfig cfg) : cfg_(cfg) {}

std::vector<double> Allocator::allocate(const std::vector<double>& scores) const {
  if (scores.empty()) {
    throw std::invalid_argument("allocate(): empty score vector");
  }

  const size_t N = scores.size();
  std::vector<double> weights(N, 0.0);

  // 1) Stabilised softmax
  const double max_score = *std::max_element(scores.begin(), scores.end());
  double sum = 0.0;

  for (size_t i = 0; i < N; ++i) {
    const double z = (scores[i] - max_score) / cfg_.temperature;
    weights[i] = std::exp(z);
    sum += weights[i];
  }

  if (sum <= 0.0) {
    throw std::runtime_error("softmax sum <= 0");
  }

  for (double& w : weights) {
    w /= sum;
  }

  // 2) Apply caps
  double excess = 0.0;
  for (double& w : weights) {
    if (w > cfg_.max_weight) {
      excess += (w - cfg_.max_weight);
      w = cfg_.max_weight;
    }
  }

  // 3) Redistribute excess proportionally
  if (excess > 0.0) {
    double redistrib_sum = 0.0;
    for (double w : weights) {
      if (w < cfg_.max_weight) redistrib_sum += w;
    }

    if (redistrib_sum > 0.0) {
      for (double& w : weights) {
        if (w < cfg_.max_weight) {
          w += excess * (w / redistrib_sum);
        }
      }
    }
  }

  // 4) Optional floor
  for (double& w : weights) {
    if (w < cfg_.min_weight) w = 0.0;
  }

  // 5) Renormalize
  double final_sum = std::accumulate(weights.begin(), weights.end(), 0.0);
  if (final_sum > 0.0) {
    for (double& w : weights) {
      w /= final_sum;
    }
  }

  return weights;
}

} // namespace engine::alloc
