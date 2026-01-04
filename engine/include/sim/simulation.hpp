#pragma once

#include "graph/graph.hpp"
#include "event/event.hpp"
#include "scoring/scorer.hpp"
#include "alloc/allocator.hpp"

#include <vector>
#include <cstdint>

namespace engine::sim {

struct SimulationConfig {
  int64_t step_seconds = 60;          // time step
  engine::scoring::ScoringConfig scoring_cfg;
  engine::alloc::AllocationConfig alloc_cfg;
};

struct SimulationState {
  int64_t unix_s;
  std::vector<double> signal;
  std::vector<double> scores;
  std::vector<double> weights;
};

class Simulation {
public:
  Simulation(engine::graph::Graph graph,
             SimulationConfig cfg);

  // Add events before running
  void add_event(engine::event::Event e);

  // Run from start to end (inclusive)
  std::vector<SimulationState> run(int64_t start_unix_s,
                                   int64_t end_unix_s);

private:
  engine::graph::Graph graph_;
  SimulationConfig cfg_;
  std::vector<engine::event::Event> events_;
};

} // namespace engine::sim
