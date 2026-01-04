#include "portfolio/portfolio.hpp"

#include <stdexcept>
#include <numeric>
#include <cmath>

namespace engine::portfolio {

using engine::graph::NodeType;

static bool is_finite_positive(double x) {
  return std::isfinite(x) && x > 0.0;
}

Portfolio::Portfolio(engine::graph::Graph graph, PortfolioConfig cfg)
  : graph_(std::move(graph)), cfg_(cfg) {}

PortfolioState Portfolio::init(int64_t unix_s) {
  PortfolioState s;
  s.unix_s = unix_s;
  s.cash = cfg_.initial_cash;
  s.units.assign(graph_.node_count(), 0.0);
  s.equity = s.cash;
  return s;
}

PortfolioState Portfolio::mark_to_market(const PortfolioState& prev,
                                        const PriceSnapshot& px) const {
  if (px.price.size() != graph_.node_count()) {
    throw std::invalid_argument("PriceSnapshot.price size must equal node_count");
  }

  PortfolioState s = prev;
  s.unix_s = px.unix_s;

  double eq = s.cash;
  for (size_t i = 0; i < s.units.size(); ++i) {
    if (s.units[i] == 0.0) continue;
    const double p = px.price[i];
    if (!is_finite_positive(p)) continue;
    eq += s.units[i] * p;
  }
  s.equity = eq;
  return s;
}

PortfolioState Portfolio::rebalance(const PortfolioState& prev,
                                   const PriceSnapshot& px,
                                   const std::vector<double>& target_weights) const {
  if (px.price.size() != graph_.node_count() || target_weights.size() != graph_.node_count()) {
    throw std::invalid_argument("rebalance: vector size mismatch with node_count");
  }

  // mark-to-market first
  PortfolioState s = mark_to_market(prev, px);

  // sanity: weights sum
  const double wsum = std::accumulate(target_weights.begin(), target_weights.end(), 0.0);
  if (std::abs(wsum - 1.0) > 1e-6) {
    throw std::invalid_argument("rebalance: target weights must sum to 1");
  }

  // desired dollar allocation per node
  const double total = s.equity;
  std::vector<double> desired_value(graph_.node_count(), 0.0);

  for (size_t i = 0; i < graph_.node_count(); ++i) {
    if (graph_.node(static_cast<uint32_t>(i)).type != NodeType::Asset) continue;
    const double p = px.price[i];
    if (!is_finite_positive(p)) continue;
    desired_value[i] = total * target_weights[i];
  }

  // current value
  std::vector<double> current_value(graph_.node_count(), 0.0);
  for (size_t i = 0; i < graph_.node_count(); ++i) {
    const double p = px.price[i];
    if (!is_finite_positive(p)) continue;
    current_value[i] = s.units[i] * p;
  }

  // trades = desired - current, executed immediately at price
  for (size_t i = 0; i < graph_.node_count(); ++i) {
    if (graph_.node(static_cast<uint32_t>(i)).type != NodeType::Asset) continue;

    const double p = px.price[i];
    if (!is_finite_positive(p)) continue;

    const double dv = desired_value[i] - current_value[i];
    if (std::abs(dv) < 1e-9) continue;

    const double du = dv / p; // units to buy (+) or sell (-)

    // update cash and holdings
    s.cash -= dv;
    s.units[i] += du;
  }

  // final equity re-check
  s = mark_to_market(s, px);
  return s;
}

} // namespace engine::portfolio
