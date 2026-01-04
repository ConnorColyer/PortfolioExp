#include "event/injector.hpp"

#include <cmath>
#include <stdexcept>

namespace engine::event {

std::vector<double> EventInjector::build_signal(
    const engine::graph::Graph& graph,
    const std::vector<Event>& events,
    int64_t now_unix_s) const {

  const size_t N = graph.node_count();
  std::vector<double> signal(N, 0.0);

  for (const auto& e : events) {
    if (e.target_node >= N) {
      throw std::out_of_range("Event target_node out of range");
    }

    const double decayed =
        apply_decay(e.magnitude, e.half_life_s, e.unix_s, now_unix_s);

    signal[e.target_node] += decayed * e.confidence;
  }

  return signal;
}

double EventInjector::apply_decay(double magnitude,
                                  double half_life_s,
                                  int64_t event_s,
                                  int64_t now_s) {
  if (half_life_s <= 0.0) return magnitude;

  const double dt = static_cast<double>(now_s - event_s);
  if (dt <= 0.0) return magnitude;

  const double factor = std::pow(0.5, dt / half_life_s);
  return magnitude * factor;
}

} // namespace engine::event
