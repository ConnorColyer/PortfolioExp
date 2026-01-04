#include "alloc/allocator.hpp"

#include <catch2/catch_test_macros.hpp>
#include <random>
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>

using namespace engine::alloc;

static bool finite_vec(const std::vector<double>& v) {
  for (double x : v) if (!std::isfinite(x)) return false;
  return true;
}

TEST_CASE("Allocator properties: bounds, sum, finiteness", "[props][alloc]") {
  AllocationConfig cfg;
  cfg.max_weight = 0.25;
  cfg.allow_cash = true;
  cfg.temperature = 1.0;
  cfg.min_weight = 0.0;

  Allocator a(cfg);

  std::mt19937_64 rng(0xC0FFEEULL);
  std::uniform_int_distribution<int> n_dist(1, 128);
  std::normal_distribution<double> score_dist(0.0, 5.0);

  for (int trial = 0; trial < 2000; ++trial) {
    const size_t n = static_cast<size_t>(n_dist(rng));

    std::vector<double> scores;
    scores.reserve(n);
    for (size_t i = 0; i < n; ++i) {
      double s = score_dist(rng);
      // Occasionally inject extreme values
      if ((trial % 97) == 0 && i == 0) s = 1e6;
      if ((trial % 113) == 0 && i == 1) s = -1e6;
      scores.push_back(s);
    }

    auto w = a.allocate(scores);

    REQUIRE(w.size() == scores.size());
    REQUIRE(finite_vec(w));

    double sum = 0.0;
    for (double wi : w) {
      REQUIRE(wi >= -1e-12);
      REQUIRE(wi <= cfg.max_weight + 1e-9);
      sum += wi;
    }

    REQUIRE(sum <= 1.0 + 1e-9);
    REQUIRE(sum >= -1e-9);
  }
}

TEST_CASE("Allocator property: permutation invariance", "[props][alloc]") {
  AllocationConfig cfg;
  cfg.max_weight = 0.3;
  cfg.allow_cash = true;
  cfg.temperature = 1.0;

  Allocator a(cfg);

  std::vector<double> scores = {3.0, 1.0, 2.0, 5.0, 0.0, -4.0};
  auto w1 = a.allocate(scores);

  // permute
  std::vector<size_t> idx = {3, 0, 5, 2, 1, 4};
  std::vector<double> scores2(scores.size());
  for (size_t i = 0; i < idx.size(); ++i) scores2[i] = scores[idx[i]];

  auto w2 = a.allocate(scores2);

  // unpermute w2 back to original order
  std::vector<double> w2_un(scores.size(), 0.0);
  for (size_t i = 0; i < idx.size(); ++i) {
    w2_un[idx[i]] = w2[i];
  }

  REQUIRE(w1.size() == w2_un.size());
  for (size_t i = 0; i < w1.size(); ++i) {
    REQUIRE(std::abs(w1[i] - w2_un[i]) <= 1e-9);
  }
}
