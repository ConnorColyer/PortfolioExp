#include "graph/graph.hpp"

#include <iostream>
#include <cassert>

using namespace engine::graph;

int main() {
  Graph g;

  const int64_t t0 = 1700000000;

  NodeId A = g.get_or_create_node(NodeType::Asset, "A");
  NodeId B = g.get_or_create_node(NodeType::Asset, "B");
  NodeId C = g.get_or_create_node(NodeType::Asset, "C");

  assert(g.node_count() == 3);

  g.add_edge(A, B, EdgeType::CoMove, 0.8, t0);
  g.add_edge(B, C, EdgeType::NewsImpact, 0.4, t0, 1.0, 1.0, 3600.0);

  assert(g.edge_count() == 2);

  auto trips = g.adjacency_triplets(t0 + 1800);
  assert(!trips.empty());

  std::cout << "Graph smoke test passed.\n";
  std::cout << "Nodes: " << g.node_count()
            << " Edges: " << g.edge_count()
            << " Triplets: " << trips.size() << "\n";

  return 0;
}
