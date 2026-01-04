#include "scoring/scorer.hpp"
#include "core/log.hpp"

#include <cmath>
#include <stdexcept>
#include <algorithm>

namespace engine::scoring {

Scorer::Scorer(ScoringConfig cfg) : cfg_(cfg) {}

std::vector<double> Scorer::score(const engine::graph::Graph& graph,
                                  const std::vector<double>& signal,
                                  int64_t unix_s) const {
  const size_t N = graph.node_count();
  if (signal.size() != N) {
    throw std::invalid_argument("signal vector size must equal node count");
  }

  // Initial state
  std::vector<double> s = signal;
  std::vector<double> s_next(N, 0.0);

  // Precompute adjacency triplets
  const auto trips = graph.adjacency_triplets(unix_s, true, cfg_.include_self_loops);

  for (uint32_t iter = 0; iter < cfg_.max_iters; ++iter) {
    std::fill(s_next.begin(), s_next.end(), 0.0);

    // Propagate
    for (const auto& t : trips) {
      s_next[t.row] += t.value * s[t.col];
    }

    // Damping / injection
    double delta = 0.0;
    for (size_t i = 0; i < N; ++i) {
      s_next[i] = (1.0 - cfg_.alpha) * signal[i] + cfg_.alpha * s_next[i];
      delta += std::abs(s_next[i] - s[i]);
    }

    s.swap(s_next);

    if (delta < cfg_.tolerance) {
      engine::core::log_info("Scorer converged early.");
      break;
    }
  }

  return s;
}

} // namespace engine::scoring
