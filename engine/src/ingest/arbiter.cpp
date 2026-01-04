#include "ingest/arbiter.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>

namespace engine::ingest {

Arbiter::Arbiter(ArbiterConfig cfg) : cfg_(cfg) {}

int64_t Arbiter::bucket_time(int64_t unix_s, int64_t bucket_s) {
  return (unix_s / bucket_s) * bucket_s;
}

static double median(std::vector<double> v) {
  if (v.empty()) return 0.0;
  std::sort(v.begin(), v.end());
  const size_t n = v.size();
  if (n % 2 == 1) return v[n / 2];
  return 0.5 * (v[n / 2 - 1] + v[n / 2]);
}

static double mad(std::vector<double> v, double med) {
  for (double& x : v) x = std::abs(x - med);
  return median(std::move(v));
}

std::vector<Belief> Arbiter::arbitrate(const std::vector<Record>& records) const {
  // Group by (type, node, bucket)
  struct Key {
    RecordType type;
    uint32_t node;
    int64_t bucket;
    bool operator<(const Key& o) const {
      if (type != o.type) return type < o.type;
      if (node != o.node) return node < o.node;
      return bucket < o.bucket;
    }
  };

  std::map<Key, std::vector<Record>> groups;
  for (const auto& r : records) {
    Key k{r.type, r.target_node, bucket_time(r.unix_s, cfg_.bucket_seconds)};
    groups[k].push_back(r);
  }

  std::vector<Belief> out;
  out.reserve(groups.size());

  for (auto& [k, grp] : groups) {
    std::vector<double> vals;
    vals.reserve(grp.size());
    for (const auto& r : grp) vals.push_back(r.value);

    const double med = median(vals);
    const double m = mad(vals, med);
    const double scale = (m <= 0.0) ? 1.0 : (1.4826 * m); // robust std estimate

    // Filter outliers using robust z-score
    std::vector<Record> kept;
    kept.reserve(grp.size());

    for (const auto& r : grp) {
      const double z = (scale > 0.0) ? std::abs(r.value - med) / scale : 0.0;
      if (z <= cfg_.outlier_z) kept.push_back(r);
    }

    // If we filtered everything (rare), fall back to all
    if (kept.empty()) kept = grp;

    // Aggregate value = weighted median-ish via confidence weights:
    // We'll do weighted mean around median for simplicity & stability.
    double wsum = 0.0;
    double vsum = 0.0;

    std::vector<double> kept_vals;
    kept_vals.reserve(kept.size());
    for (const auto& r : kept) kept_vals.push_back(r.value);

    const double kept_med = median(kept_vals);
    for (const auto& r : kept) {
      const double w = r.confidence;
      wsum += w;
      vsum += w * r.value;
    }

    const double value = (wsum > 0.0) ? (vsum / wsum) : kept_med;

    // Agreement score: tighter clusters => higher confidence
    // Use average robust z-distance from median
    double avg_z = 0.0;
    for (const auto& r : kept) {
      const double z = (scale > 0.0) ? std::abs(r.value - kept_med) / scale : 0.0;
      avg_z += z;
    }
    avg_z = kept.empty() ? 0.0 : (avg_z / kept.size());

    double agreement = 1.0 / (1.0 + avg_z); // [0,1]ish
    double conf = std::clamp(wsum / kept.size(), 0.0, 1.0) * agreement;
    conf = std::max(conf, cfg_.min_confidence);

    Belief b;
    b.type = k.type;
    b.target_node = k.node;
    b.bucket_unix_s = k.bucket;
    b.value = value;
    b.confidence = conf;
    b.count = static_cast<uint32_t>(kept.size());

    for (const auto& r : kept) b.sources_used.push_back(r.source);

    out.push_back(std::move(b));
  }

  return out;
}

} // namespace engine::ingest
