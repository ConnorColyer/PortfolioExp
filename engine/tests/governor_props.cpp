#include "risk/governor.hpp"

#include <catch2/catch_test_macros.hpp>
#include <random>
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>

using namespace engine::risk;

static double l1_turnover(const std::vector<double>& a,
                          const std::vector<double>& b) {
  const size_t n = std::min(a.size(), b.size());
  double t = 0.0;
  for (size_t i = 0; i < n; ++i) t += std::abs(a[i] - b[i]);
  return t;
}

TEST_CASE("RiskGovernor properties: caps/sum/turnover/idempotence", "[props][risk]") {
  RiskConfig cfg;
  RiskGovernor gov(cfg);

  std::mt19937_64 rng(0xBADC0FFEEULL);
  std::uniform_int_distribution<int> n_dist(1, 64);
  std::uniform_real_distribution<double> w_dist(0.0, 1.0);
  std::uniform_real_distribution<double> conf_dist(0.0, 1.0);

  for (int trial = 0; trial < 2000; ++trial) {
    const size_t n = static_cast<size_t>(n_dist(rng));

    std::vector<double> prev(n, 0.0);
    std::vector<double> target(n, 0.0);

    // Create a "safe-ish" prev (doesn't have to be perfect; governor handles it)
    for (size_t i = 0; i < n; ++i) prev[i] = w_dist(rng);
    double sp = std::accumulate(prev.begin(), prev.end(), 0.0);
    if (sp > 1e-12) for (double& x : prev) x /= sp;

    // Target can be wild
    for (size_t i = 0; i < n; ++i) target[i] = w_dist(rng);
    double st = std::accumulate(target.begin(), target.end(), 0.0);
    if (st > 1e-12) for (double& x : target) x /= st;

    const double conf = conf_dist(rng);

    auto d = gov.apply(target, prev, conf);

    // 1) per-asset cap + non-neg
    for (double wi : d.adjusted_weights) {
      REQUIRE(wi >= -1e-12);
      REQUIRE(wi <= cfg.max_weight + 1e-9);
      REQUIRE(std::isfinite(wi));
    }

    // 2) sum <= min(exposure_scale, 1-cash_floor) + eps
    const double invested = std::accumulate(d.adjusted_weights.begin(),
                                            d.adjusted_weights.end(), 0.0);
    const double max_invested = std::min(d.exposure_scale, 1.0 - cfg.cash_floor);
    REQUIRE(invested <= max_invested + 1e-9);

    // 3) turnover cap (relative to prev)
    const double t = l1_turnover(d.adjusted_weights, prev);
    REQUIRE(t <= cfg.max_turnover + 1e-9);

    // 4) idempotence: applying again should not change (or tiny drift)
    auto d2 = gov.apply(d.adjusted_weights, d.adjusted_weights, conf);
    REQUIRE(d2.adjusted_weights.size() == d.adjusted_weights.size());
    for (size_t i = 0; i < d.adjusted_weights.size(); ++i) {
      REQUIRE(std::abs(d2.adjusted_weights[i] - d.adjusted_weights[i]) <= 1e-12);
    }
  }
}
