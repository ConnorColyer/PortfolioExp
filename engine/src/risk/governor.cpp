#include "risk/governor.hpp"

#include <algorithm>
#include <numeric>
#include <cmath>

namespace engine::risk {

static void apply_hard_constraints(std::vector<double>& w,
                                  double per_asset_cap,
                                  double max_invested) {
  // clamp negatives + per-asset cap
  for (double& x : w) {
    if (x < 0.0) x = 0.0;
    if (x > per_asset_cap) x = per_asset_cap;
  }

  // scale down to max invested (leave remainder as cash)
  double invested = std::accumulate(w.begin(), w.end(), 0.0);
  if (invested > max_invested && invested > 0.0) {
    const double s = max_invested / invested;
    for (double& x : w) x *= s;
  }
}

static void apply_turnover_cap(std::vector<double>& w,
                               const std::vector<double>& prev,
                               double max_turnover) {
  if (prev.empty()) return;
  if (w.size() != prev.size()) return;

  double t = 0.0;
  for (size_t i = 0; i < w.size(); ++i) {
    t += std::abs(w[i] - prev[i]);
  }

  if (t <= max_turnover || t <= 0.0) return;

  const double scale = max_turnover / t;
  for (size_t i = 0; i < w.size(); ++i) {
    w[i] = prev[i] + scale * (w[i] - prev[i]);
  }
}

RiskGovernor::RiskGovernor(RiskConfig cfg) : cfg_(cfg) {}

RiskDecision RiskGovernor::apply(const std::vector<double>& target,
                                 const std::vector<double>& prev,
                                 double global_confidence) const {
  RiskDecision out;
  out.adjusted_weights = target;

  const size_t N = target.size();
  if (!prev.empty() && prev.size() != N) {
    // If caller messes up, fail safe: return target projected to constraints.
    // (No exception here to avoid crashing live loop later.)
  }

  // 1) Exposure cap from confidence
  double exposure_cap = 1.0;
  if (global_confidence < cfg_.min_global_confidence) {
    exposure_cap = std::clamp(
      global_confidence / cfg_.min_global_confidence,
      0.0, 1.0
    );
  }
  out.exposure_scale = exposure_cap;

  // cash floor is also a hard cap on invested exposure
  const double max_invested = std::min(exposure_cap, 1.0 - cfg_.cash_floor);

  // 2) Build a "safe previous" reference under the same hard constraints
  std::vector<double> prev_safe;
  if (!prev.empty()) {
    prev_safe = prev;
    apply_hard_constraints(prev_safe, cfg_.max_weight, max_invested);
  }

  // 3) Apply hard constraints to candidate
  apply_hard_constraints(out.adjusted_weights, cfg_.max_weight, max_invested);

  // 4) Turnover cap relative to safe previous (so de-risking isn’t blocked)
  if (!prev_safe.empty()) {
    apply_turnover_cap(out.adjusted_weights, prev_safe, cfg_.max_turnover);
  }

  // 5) Ensure hard constraints still hold (they should, but belt + braces)
  apply_hard_constraints(out.adjusted_weights, cfg_.max_weight, max_invested);

  return out;
}

} // namespace engine::risk
