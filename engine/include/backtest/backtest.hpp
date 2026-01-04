#pragma once

#include "sim/simulation.hpp"
#include "portfolio/portfolio.hpp"

#include <vector>
#include <cstdint>

namespace engine::backtest {

struct BacktestMetrics {
  double final_equity = 0.0;
  double total_return = 0.0;
  double max_drawdown = 0.0;
  double total_turnover = 0.0;
};

struct BacktestResult {
  std::vector<int64_t> time;
  std::vector<double> equity;
  BacktestMetrics metrics;
};

class Backtest {
public:
  Backtest(engine::sim::Simulation sim,
           engine::portfolio::Portfolio portfolio);

  BacktestResult run(int64_t start_unix_s,
                     int64_t end_unix_s,
                     const std::vector<engine::portfolio::PriceSnapshot>& prices);

private:
  engine::sim::Simulation sim_;
  engine::portfolio::Portfolio portfolio_;
};

} // namespace engine::backtest
