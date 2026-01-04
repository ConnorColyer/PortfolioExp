#include "scoring/scorer.hpp"
#include "graph/graph.hpp"

#include <cassert>
#include <iostream>

using namespace engine::graph;
using namespace engine::scoring;

int main() {
  Graph g;
  const int64_t t0 = 1700000000;

  NodeId A = g.get_or_create_node(NodeType::Asset, "A");
  NodeId B = g.get_or_create_node(NodeType::Asset, "B");
  NodeId C = g.get_or_create_node(NodeType::Asset, "C");

  g.add_edge(A, B, EdgeType::CoMove, 1.0, t0);
  g.add_edge(B, C, EdgeType::CoMove, 1.0, t0);

  std::vector<double> signal = {1.0, 0.0, 0.0};

  Scorer scorer;
  auto scores = scorer.score(g, signal, t0);

  assert(scores.size() == 3);
  assert(scores[B] > 0.0);
  assert(scores[C] > 0.0);

  std::cout << "Scoring smoke test passed.\n";
  std::cout << "Scores: A=" << scores[A]
            << " B=" << scores[B]
            << " C=" << scores[C] << "\n";

  return 0;
}
