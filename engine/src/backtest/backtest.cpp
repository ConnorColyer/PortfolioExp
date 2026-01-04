#include "backtest/backtest.hpp"

#include <algorithm>
#include <cmath>

namespace engine::backtest {

using engine::portfolio::PortfolioState;

static double compute_drawdown(const std::vector<double>& equity) {
  double peak = equity.front();
  double max_dd = 0.0;

  for (double v : equity) {
    if (v > peak) peak = v;
    double dd = (peak - v) / peak;
    max_dd = std::max(max_dd, dd);
  }
  return max_dd;
}

Backtest::Backtest(engine::sim::Simulation sim,
                   engine::portfolio::Portfolio portfolio)
  : sim_(std::move(sim)), portfolio_(std::move(portfolio)) {}

BacktestResult Backtest::run(int64_t start_unix_s,
                             int64_t end_unix_s,
                             const std::vector<engine::portfolio::PriceSnapshot>& prices) {
  BacktestResult out;

  auto sim_states = sim_.run(start_unix_s, end_unix_s);

  PortfolioState pstate = portfolio_.init(start_unix_s);

  for (size_t i = 0; i < sim_states.size(); ++i) {
    const auto& s = sim_states[i];
    const auto& px = prices[i];

    // rebalance
    auto prev_units = pstate.units;
    pstate = portfolio_.rebalance(pstate, px, s.safe_weights);

    // turnover
    double step_turnover = 0.0;
    for (size_t j = 0; j < pstate.units.size(); ++j) {
      step_turnover += std::abs(pstate.units[j] - prev_units[j]);
    }
    out.metrics.total_turnover += step_turnover;

    out.time.push_back(s.unix_s);
    out.equity.push_back(pstate.equity);
  }

  out.metrics.final_equity = out.equity.back();
  out.metrics.total_return =
      (out.metrics.final_equity / out.equity.front()) - 1.0;
  out.metrics.max_drawdown = compute_drawdown(out.equity);

  return out;
}

} // namespace engine::backtest
