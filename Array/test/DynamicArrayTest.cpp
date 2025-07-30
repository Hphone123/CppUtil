#include <catch2/catch.hpp>
#include <stdexcept>

#define UNIT_TEST 1

#include "Array.hpp"
#include <iostream>

TEST_CASE("DynamicArray Init", "[dynamic_array][init]")
{

  DynamicArray<int> arr;

  SECTION("DynamicArray can be constructed")
  {
    REQUIRE_NOTHROW(arr = DynamicArray<int>());
  }

  SECTION("DynamicArray can be constructedwith capacity argument")
  {
    REQUIRE_NOTHROW(arr = DynamicArray<int>(5));
    REQUIRE(5 == arr.getCap());
  }

  SECTION("DynamicArray can be constructedwith capacity and resizeFactor arguments")
  {
    REQUIRE_NOTHROW(arr = DynamicArray<int>(5, 3));
    REQUIRE(5 == arr.getCap());
    REQUIRE(3 == arr.getResizeFactor());
  }
}

TEST_CASE("DynamicArray Access", "[dynamic_array][access]")
{
  DynamicArray<int> arr(5);
  arr.add(1);
  arr.add(2);
  arr.add(3);
  arr.add(4);
  arr.add(5);
  
  SECTION("DynamicArray elements can be retrieved")
  {
    REQUIRE_NOTHROW(arr[0] = 10);
    REQUIRE(arr[0] == 10);
  }

  SECTION("DynamicArray cannot be accessed out of bounds")
  {
    REQUIRE_THROWS_AS(arr[5] = 100, out_of_range);
  }
}

TEST_CASE("DynamicArray Add", "[dynamic_array][add]")
{

  DynamicArray<size_t> arr;

  SECTION("Elements can be added to DynamicArray")
  {
    REQUIRE_NOTHROW(arr.add(1));
    REQUIRE_NOTHROW(arr.add(2));

    REQUIRE(arr[0] == 1);
    REQUIRE(arr[1] == 2);
  }

  SECTION("DynamicArray Sizes up when adding elements", "[resize]")
  {
    for (int i = 0; i < 10; i++)
    {
      REQUIRE_NOTHROW(arr.add(i));
    }
    
    REQUIRE(16 == arr.getCap());
  }
  
  SECTION("Elements can be inserted into DynamicArray", "[insert]")
  {
    size_t idx = 500;

    for (size_t i = 0; i < 10'000; i++)
    {
      REQUIRE_NOTHROW(arr.add(i));
    }
    
    REQUIRE_NOTHROW(arr.add(10'000 + 1, idx));

    for (size_t i = 0; i < idx; i++)
    {
      REQUIRE(i == arr[i]);
    }

    REQUIRE(10'000 + 1 == arr[idx]);
    
    for (size_t i = idx; i < 10'000; i++)
    {
      REQUIRE(i == arr[i + 1]);
    }
  }
}

TEST_CASE("DynamicArray Remove", "[dynamic_arary][remove]")
{

  DynamicArray<int> arr(5);
  arr.add(1);
  arr.add(2);
  arr.add(3);
  arr.add(4);
  arr.add(5);

  SECTION("Elements can be removed from the end of DynamicArray")
  {
    REQUIRE_NOTHROW(arr.remove());
    REQUIRE_THROWS_AS(arr[4], out_of_range);
  }
  
  SECTION("Elements can be removed from the middle of the DynaicArray")
  {
    REQUIRE_NOTHROW(arr.remove(2));
    REQUIRE_THROWS_AS(arr[4], out_of_range);
    REQUIRE(arr[0] == 1);
    REQUIRE(arr[1] == 2);
    REQUIRE(arr[2] == 4);
    REQUIRE(arr[3] == 5);
  }
  
  SECTION("Elements cannot be removed from empty DynamicArray")
  {
    REQUIRE_NOTHROW(arr.remove());
    REQUIRE_NOTHROW(arr.remove());
    REQUIRE_NOTHROW(arr.remove());
    REQUIRE_NOTHROW(arr.remove());
    REQUIRE_NOTHROW(arr.remove());

    REQUIRE_THROWS_AS(arr.remove(), length_error);
    REQUIRE_THROWS_AS(arr.remove(0), length_error);
  }
}