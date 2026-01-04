#pragma once

#include <vector>
#include <cstdint>

namespace engine::alloc {

struct AllocationConfig {
  double temperature = 1.0;     // softmax temperature
  double max_weight = 0.25;     // per-asset cap
  double min_weight = 0.0;      // floor (optional)
  bool allow_cash = false;      // if true, leftover goes to cash
};

class Allocator {
public:
  explicit Allocator(AllocationConfig cfg = {});

  // Input: raw scores (can be any scale)
  // Output: weights that sum to 1
  std::vector<double> allocate(const std::vector<double>& scores) const;

private:
  AllocationConfig cfg_;
};

} // namespace engine::alloc
