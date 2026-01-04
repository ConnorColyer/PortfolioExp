#include "graph/graph.hpp"

#include <cmath>
#include <stdexcept>
#include <limits>

namespace engine {
namespace graph {

Graph::Graph() = default;

// ---- Nodes ----
NodeId Graph::add_node(NodeType type, std::string key, uint32_t meta_idx) {
  NodeId id = static_cast<NodeId>(nodes_.size());
  nodes_.push_back(Node{ id, type, meta_idx, std::move(key) });
  out_.emplace_back();
  in_.emplace_back();
  return id;
}

NodeId Graph::get_or_create_node(NodeType type, const std::string& key) {
  auto it = key_to_node_.find(key);
  if (it != key_to_node_.end()) return it->second;
  NodeId id = add_node(type, key);
  key_to_node_.emplace(key, id);
  return id;
}

const Node& Graph::node(NodeId id) const {
  if (id >= nodes_.size()) throw std::out_of_range("node id out of range");
  return nodes_[id];
}

size_t Graph::node_count() const { return nodes_.size(); }
size_t Graph::edge_count() const { return edges_.size(); }

// ---- Edges ----
EdgeId Graph::add_edge(NodeId from,
                       NodeId to,
                       EdgeType type,
                       double weight,
                       int64_t unix_s,
                       double confidence,
                       double source_repute,
                       double half_life_s) {
  if (from >= nodes_.size() || to >= nodes_.size()) {
    throw std::out_of_range("add_edge: node id out of range");
  }

  EdgeId id = static_cast<EdgeId>(edges_.size());
  edges_.push_back(Edge{
    id, from, to, type,
    weight, confidence, source_repute,
    half_life_s, unix_s
  });

  out_[from].push_back(id);
  in_[to].push_back(id);
  return id;
}

const Edge& Graph::edge(EdgeId id) const {
  if (id >= edges_.size()) throw std::out_of_range("edge id out of range");
  return edges_[id];
}

Edge& Graph::edge_mut(EdgeId id) {
  if (id >= edges_.size()) throw std::out_of_range("edge id out of range");
  return edges_[id];
}

void Graph::update_edge_weight(EdgeId id, double new_weight, int64_t unix_s, bool decay_before) {
  Edge& e = edge_mut(id);
  if (decay_before) {
    e.weight = apply_decay_scalar(e.weight, e.half_life_s, e.last_update_unix_s, unix_s);
  }
  e.weight = new_weight;
  e.last_update_unix_s = unix_s;
}

void Graph::scale_edge_weight(EdgeId id, double factor, int64_t unix_s, bool decay_before) {
  Edge& e = edge_mut(id);
  if (decay_before) {
    e.weight = apply_decay_scalar(e.weight, e.half_life_s, e.last_update_unix_s, unix_s);
  }
  e.weight *= factor;
  e.last_update_unix_s = unix_s;
}

void Graph::apply_decay_all(int64_t unix_s) {
  for (auto& e : edges_) {
    if (e.half_life_s > 0.0) {
      e.weight = apply_decay_scalar(e.weight, e.half_life_s, e.last_update_unix_s, unix_s);
      e.last_update_unix_s = unix_s;
    }
  }
}

double Graph::effective_weight(EdgeId id, int64_t unix_s) const {
  const Edge& e = edge(id);
  if (e.half_life_s <= 0.0) return e.weight;
  return apply_decay_scalar(e.weight, e.half_life_s, e.last_update_unix_s, unix_s);
}

// ---- Adjacency ----
const std::vector<EdgeId>& Graph::outgoing(NodeId from) const {
  if (from >= out_.size()) throw std::out_of_range("outgoing: node id out of range");
  return out_[from];
}

const std::vector<EdgeId>& Graph::incoming(NodeId to) const {
  if (to >= in_.size()) throw std::out_of_range("incoming: node id out of range");
  return in_[to];
}

std::vector<Graph::Triplet> Graph::adjacency_triplets(int64_t unix_s,
                                                      bool use_effective,
                                                      bool include_self_loops) const {
  std::vector<Triplet> t;
  t.reserve(edges_.size() + (include_self_loops ? nodes_.size() : 0));

  if (include_self_loops) {
    for (NodeId i = 0; i < nodes_.size(); ++i) {
      t.push_back(Triplet{ i, i, 1.0 });
    }
  }

  for (const auto& e : edges_) {
    double w = use_effective ? effective_weight(e.id, unix_s) : e.weight;
    w *= (e.confidence * e.source_repute);
    if (w == 0.0) continue;

    // Convention: col=from, row=to so A*x pushes from -> to
    t.push_back(Triplet{ e.to, e.from, w });
  }

  return t;
}

// ---- Boundary lookup ----
NodeId Graph::find_node_by_key(const std::string& key) const {
  auto it = key_to_node_.find(key);
  return it == key_to_node_.end() ? kInvalidNode : it->second;
}

// ---- Helpers ----
double Graph::apply_decay_scalar(double weight,
                                 double half_life_s,
                                 int64_t last_unix_s,
                                 int64_t now_unix_s) {
  if (half_life_s <= 0.0) return weight;
  const double dt = static_cast<double>(now_unix_s - last_unix_s);
  if (dt <= 0.0) return weight;
  const double factor = std::pow(0.5, dt / half_life_s);
  return weight * factor;
}

} // namespace graph
} // namespace engine
