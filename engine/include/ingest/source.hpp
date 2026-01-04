#pragma once

#include <string>

namespace engine::ingest {

struct SourceMeta {
  std::string name;
  double base_reliability;   // long-run trust score [0,1]
  double latency_s;          // expected delay
  bool audited;              // e.g. exchange vs scraped
};

} // namespace engine::ingest
