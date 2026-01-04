#include "event/injector.hpp"
#include "graph/graph.hpp"

#include <cassert>
#include <iostream>

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

  Event e2 {
    EventType::Technical,
    A,
    -0.5,
    0.5,
    0.0,
    t0,
    "RSI"
  };

  EventInjector injector;
  auto signal = injector.build_signal(g, {e1, e2}, t1);

  assert(signal.size() == 2);
  assert(signal[A] != 0.0);
  assert(signal[B] == 0.0);

  std::cout << "Event injector smoke test passed.\n";
  std::cout << "Signal[A]=" << signal[A] << "\n";

  return 0;
}
