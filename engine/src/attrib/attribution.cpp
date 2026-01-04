#include "attrib/attribution.hpp"
#include "event/injector.hpp"

#include <algorithm>
#include <cmath>

namespace engine::attrib {

using engine::graph::Graph;
using engine::event::Event;
using engine::event::EventInjector;
using engine::scoring::Scorer;

AttributionEngine::AttributionEngine(engine::scoring::ScoringConfig cfg)
  : cfg_(cfg) {}

Attribution AttributionEngine::attribute(const Graph& graph,
                                         const std::vector<Event>& events,
                                         int64_t unix_s) const {
  const size_t N = graph.node_count();
  const size_t E = events.size();

  Attribution out(N);

  EventInjector injector;
  Scorer scorer(cfg_);

  // For each event, inject it alone and score its contribution
  for (size_t ei = 0; ei < E; ++ei) {
    std::vector<Event> single_event = {events[ei]};
    auto signal = injector.build_signal(graph, single_event, unix_s);
    auto scores = scorer.score(graph, signal, unix_s);

    for (size_t n = 0; n < N; ++n) {
      if (scores[n] != 0.0) {
        out[n].push_back(Contribution{
          static_cast<uint32_t>(ei),
          scores[n]
        });
      }
    }
  }

  return out;
}

} // namespace engine::attrib
