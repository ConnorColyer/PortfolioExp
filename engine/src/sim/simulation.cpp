#include "sim/simulation.hpp"
#include "event/injector.hpp"
#include "risk/governor.hpp"

#include <algorithm>

namespace engine::sim {

using engine::event::EventInjector;
using engine::scoring::Scorer;
using engine::alloc::Allocator;
using engine::risk::RiskGovernor;

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
  RiskGovernor governor;

  std::vector<double> prev_safe_weights;

  for (int64_t t = start_unix_s; t <= end_unix_s; t += cfg_.step_seconds) {

    std::vector<engine::event::Event> active_events;
    for (const auto& e : events_) {
      if (e.unix_s <= t) active_events.push_back(e);
    }

    auto signal = injector.build_signal(graph_, active_events, t);
    auto scores = scorer.score(graph_, signal, t);
    auto raw_weights = allocator.allocate(scores);

    double global_conf = 1.0;
    if (!active_events.empty()) {
      double sum = 0.0;
      for (const auto& e : active_events) sum += e.confidence;
      global_conf = sum / active_events.size();
    }

    auto decision = governor.apply(
      raw_weights,
      prev_safe_weights,
      global_conf
    );

    auto safe_weights = decision.adjusted_weights;
    prev_safe_weights = safe_weights;

    timeline.push_back(SimulationState{
      t,
      std::move(signal),
      std::move(scores),
      std::move(raw_weights),
      std::move(safe_weights),
      global_conf
    });
  }

  return timeline;
}

} // namespace engine::sim
