#pragma once

#include <vector>
#include <cstdint>

namespace engine::risk {

struct RiskConfig {
  double min_global_confidence = 0.4;   // below this → reduce exposure
  double max_turnover = 0.25;           // per step cap
  double max_weight = 0.30;             // absolute per-asset cap
  double cash_floor = 0.10;             // minimum cash allocation
};

struct RiskDecision {
  std::vector<double> adjusted_weights;
  bool frozen = false;
  double exposure_scale = 1.0;
};

class RiskGovernor {
public:
  explicit RiskGovernor(RiskConfig cfg = {});

  RiskDecision apply(const std::vector<double>& target_weights,
                     const std::vector<double>& prev_weights,
                     double global_confidence) const;

private:
  RiskConfig cfg_;
};

} // namespace engine::risk
