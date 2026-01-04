#pragma once

#include "ingest/record.hpp"

#include <vector>

namespace engine::ingest {

struct ValidationResult {
  bool ok;
  double adjusted_confidence;
};

class Validator {
public:
  ValidationResult validate(const Record& r) const;

  std::vector<Record> filter_valid(const std::vector<Record>& records) const;
};

} // namespace engine::ingest
