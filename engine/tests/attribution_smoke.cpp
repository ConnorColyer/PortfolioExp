#include "attrib/attribution.hpp"
#include "graph/graph.hpp"
#include "event/event.hpp"

#include <iostream>
#include <cassert>

using namespace engine::graph;
using namespace engine::event;
using namespace engine::attrib;

int main() {
  Graph g;
  const int64_t t0 = 1700000000;

  NodeId A = g.get_or_create_node(NodeType::Asset, "A");
  NodeId B = g.get_or_create_node(NodeType::Asset, "B");

  g.add_edge(A, B, EdgeType::CoMove, 1.0, t0);

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
    -0.4,
    1.0,
    0.0,
    t0,
    "RSI"
  };

  std::vector<Event> events = {e1, e2};

  AttributionEngine engine;
  auto attrib = engine.attribute(g, events, t0);

  assert(attrib.size() == 2);
  assert(!attrib[A].empty());
  assert(!attrib[B].empty());

  std::cout << "Attribution smoke test passed.\n";
  for (size_t n = 0; n < attrib.size(); ++n) {
    std::cout << "Node " << n << ":\n";
    for (const auto& c : attrib[n]) {
      std::cout << "  Event " << c.event_index
                << " -> " << c.value << "\n";
    }
  }

  return 0;
}
