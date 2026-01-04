#pragma once

#include "graph/graph.hpp"
#include "event/event.hpp"
#include "scoring/scorer.hpp"

#include <vector>
#include <cstdint>

namespace engine::attrib {

// Contribution of one event to one node
struct Contribution {
  uint32_t event_index; // index into original event list
  double value;         // signed contribution
};

using Attribution = std::vector<std::vector<Contribution>>;
// Attribution[node] = list of event contributions

class AttributionEngine {
public:
  AttributionEngine(engine::scoring::ScoringConfig cfg = {});

  Attribution attribute(const engine::graph::Graph& graph,
                        const std::vector<engine::event::Event>& events,
                        int64_t unix_s) const;

private:
  engine::scoring::ScoringConfig cfg_;
};

} // namespace engine::attrib
