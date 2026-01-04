#pragma once

#include "graph/graph.hpp"
#include "portfolio/prices.hpp"

#include <vector>
#include <cstdint>

namespace engine::portfolio {

struct PortfolioConfig {
  double initial_cash = 10'000.0;
};

struct PortfolioState {
  int64_t unix_s = 0;
  double cash = 0.0;
  std::vector<double> units; // shares/units held per NodeId
  double equity = 0.0;       // cash + sum(units * price)
};

class Portfolio {
public:
  Portfolio(engine::graph::Graph graph, PortfolioConfig cfg);

  PortfolioState init(int64_t unix_s);
  PortfolioState mark_to_market(const PortfolioState& prev,
                                const PriceSnapshot& px) const;

  // Rebalance to target weights using current prices.
  // Assumes all weights sum to 1.
  PortfolioState rebalance(const PortfolioState& prev,
                           const PriceSnapshot& px,
                           const std::vector<double>& target_weights) const;

private:
  engine::graph::Graph graph_;
  PortfolioConfig cfg_;
};

} // namespace engine::portfolio
