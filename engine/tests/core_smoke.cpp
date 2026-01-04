#include <catch2/catch_test_macros.hpp>

TEST_CASE("Core smoke: arithmetic sanity", "[core]") {
  REQUIRE(1 + 1 == 2);
}
