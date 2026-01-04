#include "alloc/allocator.hpp"

#include <cassert>
#include <iostream>
#include <numeric>

using namespace engine::alloc;

int main() {
  AllocationConfig cfg;
  cfg.max_weight = 0.4;

  Allocator alloc(cfg);

  std::vector<double> scores = {1.0, 2.0, 3.0};
  auto w = alloc.allocate(scores);

  // Size check
  assert(w.size() == scores.size());

  // Hard invariant checks (survive NDEBUG)
  double invested = 0.0;
  for (size_t i = 0; i < w.size(); ++i) {
    const double wi = w[i];
    invested += wi;

    if (!(wi >= 0.0 && wi <= cfg.max_weight + 1e-6)) {
      std::cerr << "Weight out of bounds: w[" << i << "]=" << wi << "\n";
      return 1;
    }
  }

  if (invested > 1.0 + 1e-6) {
    std::cerr << "Total invested > 1: " << invested << "\n";
    return 1;
  }

  std::cout << "Allocator smoke test passed.\n";
  std::cout << "Weights: ";
  for (double wi : w) std::cout << wi << " ";
  std::cout << "(cash=" << (1.0 - invested) << ")\n";

  return 0;
}
