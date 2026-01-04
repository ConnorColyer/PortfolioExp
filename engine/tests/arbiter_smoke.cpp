#include "ingest/arbiter.hpp"
#include "ingest/validator.hpp"

#include <iostream>
#include <cassert>

using namespace engine::ingest;

int main() {
  Validator v;
  Arbiter a;

  // Imagine 3 sources give slightly different prices, 1 outlier glitches
  std::vector<Record> raw = {
    {RecordType::Price, 0, 100.0, 0.9, 1700000000, "Exchange"},
    {RecordType::Price, 0, 100.2, 0.8, 1700000001, "AltFeed"},
    {RecordType::Price, 0,  99.9, 0.7, 1700000002, "Broker"},
    {RecordType::Price, 0, 250.0, 0.9, 1700000002, "ScrapeGlitch"} // should be rejected as outlier
  };

  auto clean = v.filter_valid(raw);
  auto beliefs = a.arbitrate(clean);

  assert(beliefs.size() == 1);
  const auto& b = beliefs[0];

  std::cout << "Arbiter smoke test passed.\n";
  std::cout << "Value=" << b.value
            << " Conf=" << b.confidence
            << " Count=" << b.count << "\n";

  // Should be near ~100, not 250
  assert(b.value > 99.0 && b.value < 101.0);
  assert(b.count >= 3);

  return 0;
}
