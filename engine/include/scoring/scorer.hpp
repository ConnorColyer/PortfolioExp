#pragma once

#include "graph/graph.hpp"

#include <vector>
#include <cstdint>

namespace engine::scoring {

struct ScoringConfig {
  double alpha = 0.85;        // propagation strength
  double tolerance = 1e-6;    // convergence tolerance
  uint32_t max_iters = 100;   // hard stop
  bool include_self_loops = true;
};

class Scorer {
public:
  explicit Scorer(ScoringConfig cfg = {});

  // Compute steady-state scores.
  // - graph: relationship graph
  // - signal: initial shock per node (size == node_count)
  // - unix_s: time for decay evaluation
  std::vector<double> score(const engine::graph::Graph& graph,
                            const std::vector<double>& signal,
                            int64_t unix_s) const;

private:
  ScoringConfig cfg_;
};

} // namespace engine::scoring
