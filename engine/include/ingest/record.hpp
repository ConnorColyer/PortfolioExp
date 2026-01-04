#pragma once

#include <cstdint>
#include <string>

namespace engine::ingest {

enum class RecordType {
  Price,
  Fundamental,
  Macro,
  News,
  Flow,
  Technical,
  Custom
};

struct Record {
  RecordType type;
  uint32_t target_node;   // NodeId
  double value;           // numeric payload
  double confidence;      // [0,1]
  int64_t unix_s;         // timestamp
  std::string source;     // provider identifier
};

} // namespace engine::ingest
