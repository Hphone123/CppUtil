#include "../src/Fixed.hpp"

#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <sstream>
#include <string>

#include "CatchVer.hpp"

using namespace CppUtil;

TEST_CASE("construction", "[ctor][constructor]")
{
  SECTION("can be constructed from a float", "[float]")
  {
    for (float f = -1000.0f; f < 1000.0f; f += 0.5f)
    {
      REQUIRE(Fixed(f).round<Fixed<>::ROUND_DIR_DOWN>() == (int)f);
    }
  }

  SECTION("can be constructed from a double", "[double]")
  {
    for (double d = -1000.0; d < 1000.0; d += 0.5)
    {
      REQUIRE(Fixed(d).round<Fixed<>::ROUND_DIR_DOWN>() == (int)d);
    }
  }
}
TEST_CASE("cast operator", "[operator][cast]")
{
  SECTION("can parse back into a float")
  {
    for (float f = -255.0f; f < 255.0f; f += 0.5f)
    {
      const float g = (float)(Fixed<8, 8>(f));
      REQUIRE(g == f);
    }
  }

  SECTION("can parse back into a double")
  {
    for (double d = -255.0; d < 255.0; d += 0.5)
    {
      const double g = (double)(Fixed(d));
      REQUIRE(g == d);
    }
  }
}
TEST_CASE("String conversion", "[string]")
{
  SECTION("Converts to 'String' correctly")
  {
    for (double d = -1000.0; d < 1000.0; d += 0.125)
    {
      const String s =
        String((std::ostringstream() << d)
                 .str()); //? std::to_string() always gives 5 decimal digits, we only want the ones that matter!
      REQUIRE(Fixed(d).to_string() == s);
    }
  }
}