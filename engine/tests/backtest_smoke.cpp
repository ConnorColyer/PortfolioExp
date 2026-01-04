#include "backtest/backtest.hpp"
#include "graph/graph.hpp"
#include "event/event.hpp"

#include <iostream>
#include <cassert>

using namespace engine::graph;
using namespace engine::event;
using namespace engine::sim;
using namespace engine::portfolio;
using namespace engine::backtest;

int main() {
  Graph g;
  const int64_t t0 = 1700000000;
  const int64_t t1 = t0 + 300;

  NodeId A = g.get_or_create_node(NodeType::Asset, "A");
  NodeId B = g.get_or_create_node(NodeType::Asset, "B");

  g.add_edge(A, B, EdgeType::CoMove, 1.0, t0);

  SimulationConfig scfg;
  scfg.step_seconds = 60;

  Simulation sim(g, scfg);
  sim.add_event(Event{
    EventType::News, A, 1.0, 1.0, 300.0, t0, "Reuters"
  });

  Portfolio port(g, PortfolioConfig{10'000.0});

  std::vector<PriceSnapshot> prices;
  for (int64_t t = t0; t <= t1; t += 60) {
    PriceSnapshot px{t, std::vector<double>(g.node_count(), 0.0)};
    px.price[A] = 100.0 + (t - t0) * 0.05; // A up
    px.price[B] = 100.0;                  // B flat
    prices.push_back(px);
  }

  Backtest bt(sim, port);
  auto res = bt.run(t0, t1, prices);

  assert(!res.equity.empty());
  assert(res.metrics.final_equity > 10'000.0);

  std::cout << "Backtest smoke test passed.\n";
  std::cout << "Final equity: " << res.metrics.final_equity << "\n";
  std::cout << "Total return: " << res.metrics.total_return * 100.0 << "%\n";
  std::cout << "Max drawdown: " << res.metrics.max_drawdown * 100.0 << "%\n";
  std::cout << "Turnover: " << res.metrics.total_turnover << "\n";

  return 0;
}
