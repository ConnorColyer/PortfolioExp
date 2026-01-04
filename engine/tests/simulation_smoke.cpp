#include "sim/simulation.hpp"
#include "graph/graph.hpp"
#include "event/event.hpp"

#include <cassert>
#include <iostream>

using namespace engine::graph;
using namespace engine::event;
using namespace engine::sim;

int main() {
  Graph g;
  const int64_t t0 = 1700000000;
  const int64_t t1 = t0 + 300;

  NodeId A = g.get_or_create_node(NodeType::Asset, "A");
  NodeId B = g.get_or_create_node(NodeType::Asset, "B");

  g.add_edge(A, B, EdgeType::CoMove, 1.0, t0);

  SimulationConfig cfg;
  cfg.step_seconds = 60;

  Simulation sim(g, cfg);

  sim.add_event(Event{
    EventType::News,
    A,
    1.0,
    1.0,
    300.0,
    t0,
    "Reuters"
  });

  auto states = sim.run(t0, t1);

  assert(!states.empty());
  assert(states.back().weights.size() == 2);

  std::cout << "Simulation smoke test passed.\n";
  for (const auto& s : states) {
    std::cout << "t=" << s.unix_s
              << " wA=" << s.weights[A]
              << " wB=" << s.weights[B]
              << "\n";
  }

  return 0;
}
