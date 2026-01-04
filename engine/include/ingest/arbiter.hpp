#pragma once

#include "ingest/record.hpp"
#include "ingest/belief.hpp"

#include <vector>
#include <cstdint>

namespace engine::ingest {

struct ArbiterConfig {
  int64_t bucket_seconds = 60;      // group close-timestamp records
  double outlier_z = 3.5;           // MAD-based outlier cutoff
  double min_confidence = 0.05;     // floor after arbitration
};

class Arbiter {
public:
  explicit Arbiter(ArbiterConfig cfg = {});

  std::vector<Belief> arbitrate(const std::vector<Record>& records) const;

private:
  ArbiterConfig cfg_;

  static int64_t bucket_time(int64_t unix_s, int64_t bucket_s);
};

} // namespace engine::ingest
