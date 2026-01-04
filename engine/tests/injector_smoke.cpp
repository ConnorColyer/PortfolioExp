#include "event/injector.hpp"
#include "graph/graph.hpp"

#include <iostream>
#include <cassert>

using namespace engine::graph;
using namespace engine::event;

int main() {
  Graph g;
  const int64_t t0 = 1700000000;
  const int64_t t1 = t0 + 1800;

  NodeId A = g.get_or_create_node(NodeType::Asset, "A");
  NodeId B = g.get_or_create_node(NodeType::Asset, "B");

  Event e1 {
    EventType::News,
    A,
    1.0,
    1.0,
    3600.0,
    t0,
    "Reuters"
  };

  EventInjector injector;
  auto signal = injector.build_signal(g, {e1}, t1);

  // Runtime checks (not assert-only)
  if (signal.size() != 2) {
    std::cerr << "Signal size mismatch\n";
    return 1;
  }

  if (signal[A] == 0.0) {
    std::cerr << "Signal for A should be non-zero\n";
    return 1;
  }

  if (signal[B] != 0.0) {
    std::cerr << "Signal for B should be zero, got " << signal[B] << "\n";
    return 1;
  }

  std::cout << "Injector smoke test passed.\n";
  std::cout << "Signal[A]=" << signal[A]
            << " Signal[B]=" << signal[B] << "\n";

  return 0;
}
