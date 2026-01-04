#include "alloc/allocator.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

namespace engine::alloc {

// ✅ Missing definition that was causing the linker error
Allocator::Allocator(AllocationConfig cfg) : cfg_(cfg) {}

static inline double clamp_nonneg(double x) { return (x > 0.0) ? x : 0.0; }

std::vector<double> Allocator::allocate(const std::vector<double>& scores) const {
  const size_t n = scores.size();
  std::vector<double> w(n, 0.0);
  if (n == 0) return w;

  const double cap = cfg_.max_weight;
  if (!(cap > 0.0)) return w; // all cash

  // 1) Softmax-ish (temperature) but only for positive scores (no shorting)
  //    We do stable exp by subtracting max.
  std::vector<double> s(n, 0.0);
  for (size_t i = 0; i < n; ++i) s[i] = clamp_nonneg(scores[i]);

  double smax = 0.0;
  for (double x : s) smax = std::max(smax, x);
  if (smax <= 0.0) return w; // all cash

  const double T = (cfg_.temperature > 1e-12) ? cfg_.temperature : 1.0;

  std::vector<double> expv(n, 0.0);
  double exp_sum = 0.0;
  for (size_t i = 0; i < n; ++i) {
    if (s[i] <= 0.0) continue;
    const double z = (s[i] - smax) / T;
    const double e = std::exp(z);
    expv[i] = e;
    exp_sum += e;
  }
  if (!(exp_sum > 0.0)) return w;

  for (size_t i = 0; i < n; ++i) w[i] = expv[i] / exp_sum; // sums to ~1

  // 2) Enforce per-asset cap using water-filling redistribution.
  std::vector<char> capped(n, 0);

  auto sum_w = [&]() {
    return std::accumulate(w.begin(), w.end(), 0.0);
  };

  for (int iter = 0; iter < static_cast<int>(10 * n + 10); ++iter) {
    bool changed = false;

    // cap offenders
    for (size_t i = 0; i < n; ++i) {
      if (!capped[i] && w[i] > cap) {
        w[i] = cap;
        capped[i] = 1;
        changed = true;
      }
      if (w[i] < 0.0) w[i] = 0.0;
    }

    const double invested = sum_w();
    if (invested >= 1.0 - 1e-12) {
      // numeric drift safety
      if (invested > 1.0 + 1e-12) {
        const double k = 1.0 / invested;
        for (double& x : w) x *= k;
      }
      break;
    }

    const double remaining = 1.0 - invested;

    // redistribute remaining among uncapped names proportional to expv (or scores)
    double denom = 0.0;
    for (size_t i = 0; i < n; ++i) {
      if (!capped[i] && expv[i] > 0.0) denom += expv[i];
    }

    // no one left to receive -> forced cash
    if (!(denom > 0.0)) break;

    for (size_t i = 0; i < n; ++i) {
      if (!capped[i] && expv[i] > 0.0) {
        w[i] += remaining * (expv[i] / denom);
      }
    }

    // if we didn't cap anything this iteration, we might still overflow after redistribution,
    // so we loop again to re-cap if needed.
    (void)changed;
  }

  // 3) Final hard clamp and decide whether to allow cash
  double invested = 0.0;
  double maxw = 0.0;
  for (double& x : w) {
    if (x > cap) x = cap;
    if (x < 0.0) x = 0.0;
    invested += x;
    maxw = std::max(maxw, x);
  }

  // If we must be fully invested but caps prevent it, we *still* keep cash
  // (better than violating max_weight). This also matches your portfolio rebalance rules.
  if (!cfg_.allow_cash) {
    // Try to renormalize only if it won't violate cap.
    if (invested > 1e-15 && invested < 1.0 - 1e-12) {
      const double scale = 1.0 / invested;
      if (maxw * scale <= cap + 1e-12) {
        for (double& x : w) x *= scale;
        invested = 1.0;
      }
    }
  }

  // Safety: never exceed 1 due to drift
  if (invested > 1.0 + 1e-9) {
    const double k = 1.0 / invested;
    for (double& x : w) x *= k;
  }

  // Optional: min_weight floor (only if it doesn't break caps/sum too badly)
  if (cfg_.min_weight > 0.0) {
    for (double& x : w) x = std::max(x, cfg_.min_weight);
    // re-clamp to cap and re-scale down if needed
    double s2 = 0.0;
    for (double& x : w) {
      if (x > cap) x = cap;
      s2 += x;
    }
    if (s2 > 1.0 + 1e-9) {
      const double k = 1.0 / s2;
      for (double& x : w) x *= k;
    }
  }

  return w;
}

} // namespace engine::alloc
