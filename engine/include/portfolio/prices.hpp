#pragma once

#include <vector>
#include <cstdint>

namespace engine::portfolio {

// Prices for all nodes at a timestamp (only Asset nodes should be priced).
struct PriceSnapshot {
  int64_t unix_s;
  std::vector<double> price; // indexed by NodeId, NaN or 0 if not applicable
};

} // namespace engine::portfolio
