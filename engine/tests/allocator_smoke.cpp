#include "alloc/allocator.hpp"
#include <iostream>
#include <cassert>

using namespace engine::alloc;

int main() {
  AllocationConfig cfg;
  cfg.temperature = 1.0;
  cfg.max_weight = 0.6;

  Allocator alloc(cfg);

  std::vector<double> scores = {1.0, 5.0, 10.0};
  auto w = alloc.allocate(scores);

  assert(w.size() == 3);

  double sum = 0.0;
  for (double x : w) sum += x;

  assert(std::abs(sum - 1.0) < 1e-6);
  for (double x : w) assert(x <= cfg.max_weight + 1e-6);

  std::cout << "Allocator smoke test passed.\n";
  std::cout << "Weights: ";
  for (double x : w) std::cout << x << " ";
  std::cout << "\n";

  return 0;
}
