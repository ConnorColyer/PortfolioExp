#pragma once

#include "ingest/record.hpp"

#include <vector>
#include <cstdint>
#include <string>

namespace engine::ingest {

struct Belief {
  RecordType type;
  uint32_t target_node;
  int64_t bucket_unix_s;

  double value;
  double confidence;

  // Audit trail (for explainability)
  std::vector<std::string> sources_used;
  uint32_t count = 0;
};

} // namespace engine::ingest
