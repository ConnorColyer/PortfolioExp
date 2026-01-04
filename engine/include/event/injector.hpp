#pragma once

#include "event/event.hpp"
#include "graph/graph.hpp"

#include <vector>
#include <cstdint>

namespace engine::event {

class EventInjector {
public:
  // Build a signal vector from events
  std::vector<double> build_signal(const engine::graph::Graph& graph,
                                   const std::vector<Event>& events,
                                   int64_t now_unix_s) const;

private:
  static double apply_decay(double magnitude,
                            double half_life_s,
                            int64_t event_s,
                            int64_t now_s);
};

} // namespace engine::event
