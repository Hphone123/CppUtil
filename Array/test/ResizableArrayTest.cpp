#include <catch2/catch.hpp>
#include <stdexcept>

#include "Array.hpp"

TEST_CASE("Test ResizableArray", "[resizable_array]")
{
  ResizableArray<int> a = ResizableArray<int>(11);
  a[0]  = 0;
  a[1]  = 1;
  a[10] = 123456;

  SECTION("ResizableArray can be accessed in valid indeces", "[access]")
  {
    REQUIRE_NOTHROW([&]()
    {
      REQUIRE(a[0] == 0);
      REQUIRE(a[1] == 1);
      REQUIRE(a[10] == 123456);
  
      a[10] = -10;
  
      REQUIRE(a[10] == -10);
    }());
  }

  SECTION("ResizableArray cannot be accessed out of bounds", "[out_of_range]")
  {
    REQUIRE_THROWS_AS(a[11] = 1, std::out_of_range);

    int x;
    REQUIRE_THROWS_AS(x = a[11], std::out_of_range);
    REQUIRE_THROWS_AS(a[-1] = 1, std::out_of_range);
  }

  SECTION("ResizableArray can be sized up", "[size_up]")
  {
    REQUIRE_NOTHROW([&]()
    {
      a.resize(12);
      a[11] = 5;
      REQUIRE(a[11] == 5);
      a.resize(13);
      a[12] = 5;
      REQUIRE(a[12] == 5);
      a.resize(14);
      a[13] = 5;
      REQUIRE(a[13] == 5);
      a.resize(14);
      a[13] = 6;
      REQUIRE(a[13] == 6);
      a.resize(15);
      a[14] = 5;
      REQUIRE(a[14] == 5);
    }());
  }

  SECTION("ResizableArray cannot be sized down using `resize()`", "[resize]")
  {
    REQUIRE_THROWS_AS(a.resize(10), std::length_error);
    REQUIRE_THROWS_AS(a.resize(9), std::length_error);
    a.resize(150);
    REQUIRE_THROWS_AS(a.resize(149), std::length_error);
  }

  SECTION("ResizableArray can be sized down using `resizeForce()`", "[resize]")
  {
    a.resizeForce(10);
    REQUIRE_THROWS_AS(a[10] = 1, std::out_of_range);
    a.resizeForce(9);
    REQUIRE_THROWS_AS(a[9] = 1, std::out_of_range);
    a.resizeForce(8);
    REQUIRE_THROWS_AS(a[8] = 1, std::out_of_range);
    a.resizeForce(7);
    REQUIRE_THROWS_AS(a[7] = 1, std::out_of_range);

    a.resize(150);
    a[149] = 187;
    a.resizeForce(1);
    REQUIRE_THROWS_AS(a[149] == 187, std::out_of_range);
  }

  SECTION("ResizableArray cannot be sized to invalid sizes", "[resize]")
  {
    REQUIRE_THROWS_AS(a.resize(-1)     , std::length_error);
    REQUIRE_THROWS_AS(a.resizeForce(-1), std::length_error);
    REQUIRE_THROWS_AS(a.resize(0)     , std::length_error);
    REQUIRE_THROWS_AS(a.resizeForce(0), std::length_error);
  }
}