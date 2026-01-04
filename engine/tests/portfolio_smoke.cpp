#include "portfolio/portfolio.hpp"
#include "graph/graph.hpp"

#include <iostream>
#include <cassert>

using namespace engine::graph;
using namespace engine::portfolio;

int main() {
  Graph g;
  const int64_t t0 = 1700000000;
  NodeId A = g.get_or_create_node(NodeType::Asset, "A");
  NodeId B = g.get_or_create_node(NodeType::Asset, "B");

  Portfolio p(g, PortfolioConfig{10'000.0});
  auto s = p.init(t0);

  PriceSnapshot px0{t0, std::vector<double>(g.node_count(), 0.0)};
  px0.price[A] = 100.0;
  px0.price[B] = 100.0;

  // target: 50/50
  std::vector<double> w(g.node_count(), 0.0);
  w[A] = 0.5;
  w[B] = 0.5;

  s = p.rebalance(s, px0, w);
  assert(std::abs(s.equity - 10'000.0) < 1e-6);

  // price move: A doubles, B flat
  PriceSnapshot px1{t0 + 60, std::vector<double>(g.node_count(), 0.0)};
  px1.price[A] = 200.0;
  px1.price[B] = 100.0;

  auto s1 = p.mark_to_market(s, px1);

  std::cout << "Portfolio smoke test passed.\n";
  std::cout << "Equity0=" << s.equity << " Equity1=" << s1.equity << "\n";

  // With 50/50 at 100/100, you hold 50 units of A and 50 of B.
  // New equity = 50*200 + 50*100 = 15000
  assert(std::abs(s1.equity - 15000.0) < 1e-3);

  return 0;
}
