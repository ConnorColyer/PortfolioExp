#pragma once

#include <cstdint>
#include <string>

namespace engine::event {

enum class EventType {
  Earnings,
  Guidance,
  Macro,
  News,
  Technical,
  Flow,
  Custom
};

struct Event {
  EventType type;
  uint32_t target_node;      // NodeId, kept uint32_t to avoid include coupling
  double magnitude;          // signed impact (+/-)
  double confidence;         // [0,1]
  double half_life_s;        // decay
  int64_t unix_s;            // timestamp
  std::string source;        // human-readable (for attribution/logging)
};

} // namespace engine::event
