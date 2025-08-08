#include <catch2/catch.hpp>
#include <stdexcept>

#include "Array.hpp"

TEST_CASE("ResizableArray Resize", "[resizable_array][resize]")
{
  ResizableArray<int> arr(5);
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;
  arr[3] = 4;
  arr[4] = 5;

  SECTION("ResizableArray can be sized up")
  {
    REQUIRE_NOTHROW(arr.resize(10));
    REQUIRE_NOTHROW(arr[5] = 6);
    REQUIRE_NOTHROW(arr[6] = 7);
    REQUIRE_NOTHROW(arr[7] = 8);
    REQUIRE_NOTHROW(arr[8] = 9);
    REQUIRE_NOTHROW(arr[9] = 10);
  }
  
  SECTION("ResizableArray can be sized to the same size")
  {
    REQUIRE_NOTHROW(arr.resize(arr.getSize()));
  }

  SECTION("ResizableArray cannot be sized beyond ARRAY_MAX_SIZE")
  {
    REQUIRE_THROWS_AS(arr.resize(ARRAY_MAX_SIZE + 1), length_error);
  }

  SECTION("ResizableArray cannot be sized down normally")
  {
    REQUIRE_THROWS_AS(arr.resize(4), length_error);
  }

  SECTION("ResizableArray can be forced to size down")
  {
    REQUIRE_NOTHROW(arr.resizeForce(4));
  }

  SECTION("ResizableArray can be forced to size down to 0")
  {
    REQUIRE_NOTHROW(arr.resizeForce(0));
  }
}