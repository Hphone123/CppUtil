#include "../src/Fixed.hpp"

#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <sstream>
#include <string>

#include "CatchVer.hpp"

using namespace CppUtil;

TEST_CASE("Fixed construction", "[ctor][constructor]")
{
  SECTION("can be constructed from a float", "[float]")
  {
    for (float f = -1000.0f; f <= 1000.0f; f += 0.5f)
    {
      REQUIRE(Fixed(f).round<Fixed<>::ROUND_DIR_DOWN>() == (int)f);
    }
  }

  SECTION("can be constructed from a double", "[double]")
  {
    for (double d = -1000.0; d <= 1000.0; d += 0.5)
    {
      REQUIRE(Fixed(d).round<Fixed<>::ROUND_DIR_DOWN>() == (int)d);
    }
  }
};

TEST_CASE("cast operator", "[operator][cast]")
{
  SECTION("can parse back into a float")
  {
    for (float f = -255.0f; f <= 255.0f; f += 0.5f)
    {
      const float g = (float)(Fixed<8, 8>(f));
      REQUIRE(g == f);
    }
  }

  SECTION("can parse back into a double")
  {
    for (double d = -2000.0; d <= 2000.0; d += 0.5)
    {
      const double g = (double)(Fixed(d));
      REQUIRE(g == d);
    }
  }
};

TEST_CASE("Fixed string-conversion", "[string]")
{
  SECTION("converts from 'const char *' correctly")
  {
    for (double d = -2000.0; d <= 2000.0; d += 0.25)
    {
      //? std::to_string() always gives 5 decimal digits, we only want the ones that matter!
      const String s = String((std::ostringstream() << d).str());
      REQUIRE(operator""_s_fp64(s).to_string() == s);
    }

    REQUIRE((255.99609375_u_fp16).value() == 0xFFFF); // Biggest possible value 0xffffffff10310140
    REQUIRE((0.00390625_u_fp16).value() == 0x0001); // Smallest possible value

    REQUIRE((65535.9999847412109375_u_fp32).value() == 0xFFFFFFFF); // Biggest possible value
    REQUIRE((0.0000152587890625_u_fp32).value() == 0x00000001); // Smallest possible value

#if defined(__SIZEOF_INT128__)
    REQUIRE((4'294'967'295.99999999976716935634613037109375_u_fp64).value() ==
            0xFFFFFFFFFFFFFFFF); // Biggest possible value
    REQUIRE((0.00000000023283064365386962890625_u_fp64).value() == 0x0000000000000001); // Smallest possible value
#endif
  };

  SECTION("converts to 'String' correctly")
  {
    for (double d = -2000.0; d <= 2000.0; d += 0.25)
    {
      //? std::to_string() always gives 5 decimal digits, we only want the ones that matter!
      const String s = String((std::ostringstream() << d).str());
      REQUIRE(Fixed(d).to_string() == s);
    }
  }
};

TEST_CASE("Fixed Arithmetics", "[arithmetic]")
{
  SECTION("Addition", "[addition][add]")
  {
    SECTION("addition of 2 fixed numbers")
    {
      for (double d = -200; d <= 200; d += 5)
      {
        for (double dd = -200; dd <= 200; dd += 5)
        {
          const s_fp64 a = d;
          const s_fp64 b = dd;
          REQUIRE((double)(a + b) == d + dd);
        }
      }
    }

    SECTION("addition overflow", "[overflow]")
    {
      const u_fp16 a = 255.0_u_fp16;
      for (u_fp16 b = 1.0_u_fp16; b < 255.0_u_fp16; b += 0.5_u_fp16)
      {
        REQUIRE(a + b == b - 1.0_u_fp16);
      }
      REQUIRE((255.99609375_u_fp16 /*0xFF.FF*/ + 0.00390625_u_fp16 /*0x00.01*/ == 0.0_u_fp16));
    }
  }

  SECTION("Subtraction", "[subtraction][subtract]")
  {
    SECTION("Can subtract 2 positive and negative numbers")
    {
      for (double d = -200; d <= 200; d += 5)
      {
        for (double dd = -200; dd <= 200; dd += 5)
        {
          const s_fp64 a = d;
          const s_fp64 b = dd;
          REQUIRE((double)(a - b) == d - dd);
        }
      }
    }
  }
};