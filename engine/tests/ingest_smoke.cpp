#include "ingest/validator.hpp"

#include <cassert>
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
    -100.0,
    1.0,
    1700000000,
    "Garbage"
  };

  auto vg = v.validate(good);
  auto vb = v.validate(bad);

  assert(vg.ok);
  assert(!vb.ok);

  std::cout << "Ingest smoke test passed.\n";
  std::cout << "Adjusted confidence: " << vg.adjusted_confidence << "\n";

  return 0;
}
