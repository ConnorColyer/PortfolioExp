#include "risk/governor.hpp"

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>

using namespace engine::risk;

static double l1_turnover(const std::vector<double>& a,
                          const std::vector<double>& b) {
  const size_t n = std::min(a.size(), b.size());
  double t = 0.0;
  for (size_t i = 0; i < n; ++i) t += std::abs(a[i] - b[i]);
  return t;
}

int main() {
  RiskConfig cfg;
  RiskGovernor gov(cfg);

  // IMPORTANT: prev must already obey hard constraints,
  // otherwise "turnover <= max_turnover" is not achievable.
  //
  // With default cfg: max_weight=0.30, cash_floor=0.10
  // We'll sit at invested=0.5 (also matches low-confidence exposure cap later).
  std::vector<double> prev   = {0.30, 0.20};  // invested 0.50, each <= 0.30

  // A nasty target that would imply a big move (and will be capped/clamped anyway)
  std::vector<double> target = {0.00, 0.90};

  const double conf = 0.20; // exposure_cap = 0.20/0.40 = 0.50
  auto d = gov.apply(target, prev, conf);

  // 1) exposure scale rule
  const double expected_scale = conf / cfg.min_global_confidence; // 0.5
  if (std::abs(d.exposure_scale - expected_scale) > 1e-12) {
    std::cerr << "Exposure scale mismatch: got " << d.exposure_scale
              << " expected " << expected_scale << "\n";
    return 1;
  }

  // 2) per-asset cap
  const double max_w =
      *std::max_element(d.adjusted_weights.begin(), d.adjusted_weights.end());
  if (max_w > cfg.max_weight + 1e-9) {
    std::cerr << "Max weight violated: " << max_w
              << " > " << cfg.max_weight << "\n";
    return 1;
  }

  // 3) invested cap: min(exposure_cap, 1 - cash_floor) = min(0.5, 0.9) = 0.5
  const double invested =
      std::accumulate(d.adjusted_weights.begin(), d.adjusted_weights.end(), 0.0);
  if (invested > 0.5 + 1e-9) {
    std::cerr << "Invested violated: " << invested << " > 0.5\n";
    return 1;
  }

  // 4) turnover cap relative to SAFE prev
  const double t = l1_turnover(d.adjusted_weights, prev);
  if (t > cfg.max_turnover + 1e-9) {
    std::cerr << "Turnover violated: " << t
              << " > " << cfg.max_turnover << "\n";
    return 1;
  }

  std::cout << "Risk governor smoke test passed.\n";
  std::cout << "Exposure scale: " << d.exposure_scale << "\n";
  std::cout << "Turnover: " << t << "\n";
  std::cout << "Invested: " << invested << " (cash=" << (1.0 - invested) << ")\n";
  std::cout << "Adjusted weights: ";
  for (double w : d.adjusted_weights) std::cout << w << " ";
  std::cout << "\n";

  return 0;
}
