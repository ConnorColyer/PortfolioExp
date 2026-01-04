#include "ingest/validator.hpp"

#include <cmath>

namespace engine::ingest {

ValidationResult Validator::validate(const Record& r) const {
  // Hard sanity checks
  if (!std::isfinite(r.value)) return {false, 0.0};
  if (r.confidence <= 0.0)     return {false, 0.0};
  if (r.confidence > 1.0)      return {false, 0.0};

  double adj = r.confidence;

  // Type-specific sanity
  switch (r.type) {
    case RecordType::Price:
      if (r.value <= 0.0) return {false, 0.0};
      break;

    case RecordType::Fundamental:
    case RecordType::Macro:
      // allow negatives, but damp extremes
      if (std::abs(r.value) > 1e6) adj *= 0.5;
      break;

    default:
      break;
  }

  return {true, adj};
}

std::vector<Record> Validator::filter_valid(const std::vector<Record>& records) const {
  std::vector<Record> out;
  out.reserve(records.size());

  for (auto r : records) {
    auto v = validate(r);
    if (!v.ok) continue;
    r.confidence = v.adjusted_confidence;
    out.push_back(std::move(r));
  }
  return out;
}

} // namespace engine::ingest
