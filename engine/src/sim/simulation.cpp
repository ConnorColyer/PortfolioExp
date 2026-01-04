#include "sim/simulation.hpp"
#include "event/injector.hpp"

#include <algorithm>

namespace engine::sim {

using engine::event::EventInjector;
using engine::scoring::Scorer;
using engine::alloc::Allocator;

Simulation::Simulation(engine::graph::Graph graph,
                       SimulationConfig cfg)
  : graph_(std::move(graph)), cfg_(cfg) {}

void Simulation::add_event(engine::event::Event e) {
  events_.push_back(std::move(e));
}

std::vector<SimulationState> Simulation::run(int64_t start_unix_s,
                                             int64_t end_unix_s) {
  std::vector<SimulationState> timeline;

  EventInjector injector;
  Scorer scorer(cfg_.scoring_cfg);
  Allocator allocator(cfg_.alloc_cfg);

  for (int64_t t = start_unix_s; t <= end_unix_s; t += cfg_.step_seconds) {

    // collect events up to time t
    std::vector<engine::event::Event> active_events;
    for (const auto& e : events_) {
      if (e.unix_s <= t) active_events.push_back(e);
    }

    // build signal
    auto signal = injector.build_signal(graph_, active_events, t);

    // score
    auto scores = scorer.score(graph_, signal, t);

    // allocate
    auto weights = allocator.allocate(scores);

    timeline.push_back(SimulationState{
      t,
      std::move(signal),
      std::move(scores),
      std::move(weights)
    });
  }

  return timeline;
}

} // namespace engine::sim
