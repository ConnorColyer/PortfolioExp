#include "ingest/record.hpp"
#include "ingest/validator.hpp"

#include <iostream>

using namespace engine::ingest;

int main() {
  Validator v;

  Record good {
    RecordType::Price,
    0,
    100.0,
    0.9,
    1700000000,
    "Exchange"
  };

  Record bad {
    RecordType::Price,
    0,
    -100.0,     // invalid price
    1.2,        // invalid confidence
    1700000000,
    "BadFeed"
  };

  auto vg = v.validate(good);
  if (!vg.ok) {
    std::cerr << "Good record failed validation\n";
    return 1;
  }

  auto vb = v.validate(bad);
  if (vb.ok) {
    std::cerr << "Bad record unexpectedly passed validation\n";
    return 1;
  }

  std::cout << "Ingest smoke test passed.\n";
  std::cout << "Adjusted confidence (good): " << vg.adjusted_confidence << "\n";
  std::cout << "Bad record rejected (as expected). "
            << "value=" << bad.value
            << " conf=" << bad.confidence
            << " source=" << bad.source << "\n";

  return 0;
}
