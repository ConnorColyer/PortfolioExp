#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace engine {
namespace graph {

using NodeId = uint32_t;
using EdgeId = uint32_t;

inline constexpr NodeId kInvalidNode = 0xFFFFFFFFu;
inline constexpr EdgeId kInvalidEdge = 0xFFFFFFFFu;

enum class NodeType : uint8_t {
  Asset,
  Company,
  Sector,
  Macro,
  Partner,
  Event,
  Signal,
  Source,
  Unknown
};

enum class EdgeType : uint8_t {
  Ownership,
  Subsidiary,
  Partnership,
  SupplyChain,
  SectorLink,
  CoMove,
  NewsImpact,
  SignalImpact,
  Contradicts,
  Unknown
};

struct Node {
  NodeId id;
  NodeType type;
  uint32_t meta_idx;
  std::string key; // boundary/debug key; not for hot loops
};

struct Edge {
  EdgeId id;
  NodeId from;
  NodeId to;

  EdgeType type;

  double weight;
  double confidence;
  double source_repute;

  double half_life_s;          // <=0 means no decay
  int64_t last_update_unix_s;  // seconds since epoch
};

class Graph {
public:
  Graph();

  // Nodes
  NodeId add_node(NodeType type, std::string key = {}, uint32_t meta_idx = 0);
  NodeId get_or_create_node(NodeType type, const std::string& key);
  const Node& node(NodeId id) const;

  size_t node_count() const;
  size_t edge_count() const;

  // Edges
  EdgeId add_edge(NodeId from,
                  NodeId to,
                  EdgeType type,
                  double weight,
                  int64_t unix_s,
                  double confidence = 1.0,
                  double source_repute = 1.0,
                  double half_life_s = 0.0);

  const Edge& edge(EdgeId id) const;
  Edge& edge_mut(EdgeId id);

  void update_edge_weight(EdgeId id, double new_weight, int64_t unix_s, bool decay_before = true);
  void scale_edge_weight(EdgeId id, double factor, int64_t unix_s, bool decay_before = true);

  void apply_decay_all(int64_t unix_s);
  double effective_weight(EdgeId id, int64_t unix_s) const;

  // Adjacency
  const std::vector<EdgeId>& outgoing(NodeId from) const;
  const std::vector<EdgeId>& incoming(NodeId to) const;

  // Sparse view helper
  struct Triplet { NodeId row; NodeId col; double value; };
  std::vector<Triplet> adjacency_triplets(int64_t unix_s,
                                         bool use_effective = true,
                                         bool include_self_loops = false) const;

  // Boundary lookup (not for hot loops)
  NodeId find_node_by_key(const std::string& key) const;

private:
  static double apply_decay_scalar(double weight,
                                   double half_life_s,
                                   int64_t last_unix_s,
                                   int64_t now_unix_s);

  std::vector<Node> nodes_;
  std::vector<Edge> edges_;
  std::vector<std::vector<EdgeId>> out_;
  std::vector<std::vector<EdgeId>> in_;
  std::unordered_map<std::string, NodeId> key_to_node_;
};

} // namespace graph
} // namespace engine
