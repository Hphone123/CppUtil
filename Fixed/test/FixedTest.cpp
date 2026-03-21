#include "../src/Fixed.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>

#include "CatchVer.hpp"

TEST_CASE("FixedTest")
{
  SECTION("can be constructed from a float")
  {
    for (float f = -1000.0f; f < 1000.0f; f += 0.5f)
    {
      REQUIRE(Fixed(f).round<Fixed<>::ROUND_DIR_DOWN>() == (unsigned int)f);
    }
  }

  SECTION("can parse back into a float")
  {
    for (float f = -255.0f; f < 255.0f; f += 0.5f)
    {
      const float g = (float)(Fixed<8, 8>(f));
      REQUIRE(g == f);
    }
  }
}