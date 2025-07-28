#include <catch2/catch.hpp>
#include <stdexcept>

#include "Array.hpp"

TEST_CASE("Test DynamicArray", "[array]")
{
  DynamicArray<std::string> a = DynamicArray<std::string>();
  a.add("Test 1");
  a.add("Test 2");
  a.add("Test 3");
  
  SECTION("Elements can be apended to DynamicArray", "[add]")
  {
    DynamicArray<std::string> b = DynamicArray<std::string>();
    REQUIRE_NOTHROW([&](){
      b.add("String 1");
      b.add("String 2");
      b.add("String 3");
      
      REQUIRE(b[0] == "String 1");
      REQUIRE(b[1] == "String 2");
      REQUIRE(b[2] == "String 3");
    }());
  }
  

  SECTION("Elements can be added into DynamicArray", "[add]")
  {
    a.add("String 1", 0);
    a.add("String 2", 2);
    a.add("String 3", 4);

    REQUIRE(a[0] == "String 1");
    REQUIRE(a[1] == "Test 1");
    REQUIRE(a[2] == "String 2");
    REQUIRE(a[3] == "Test 2");
    REQUIRE(a[4] == "String 3");
    REQUIRE(a[5] == "Test 3");
  }

  SECTION("Elements can be removed from the end of DynamicArray until empty" "[remove]")
  {
    SECTION("Elements can be removed from DynamicArray", "[remove]")
    {
      SECTION("Remove 1 Elements", "[remove]")
      {
        REQUIRE_NOTHROW  (a.remove());
        REQUIRE          (a[0] == "Test 1");
        REQUIRE          (a[1] == "Test 2");
        REQUIRE_THROWS_AS(a[2] == "Test 3", std::out_of_range);
      }
      
      SECTION("Remove 2 Elements", "[remove]")
      {
        REQUIRE_NOTHROW([&](){
          REQUIRE_NOTHROW  (a.remove());
          REQUIRE_NOTHROW  (a.remove());
          REQUIRE          (a[0] == "Test 1");
          REQUIRE_THROWS_AS(a[1] == "Test 2", std::out_of_range);
          REQUIRE_THROWS_AS(a[2] == "Test 3", std::out_of_range);
        }());
      }
      
      SECTION("Remove 3 Elements", "[remove]")
      {
        REQUIRE_NOTHROW  (a.remove());
        REQUIRE_NOTHROW  (a.remove());
        REQUIRE_NOTHROW  (a.remove());
        REQUIRE_THROWS_AS(a[0] == "Test 1", std::out_of_range);
        REQUIRE_THROWS_AS(a[1] == "Test 2", std::out_of_range);
        REQUIRE_THROWS_AS(a[2] == "Test 3", std::out_of_range);
      }
    }
    
    SECTION("Elements cannot be removed from empty DynamicArray", "[remove]")
    {
      REQUIRE_NOTHROW  (a.remove());
      REQUIRE_NOTHROW  (a.remove());
      REQUIRE_NOTHROW  (a.remove());
      REQUIRE_THROWS_AS(a.remove(), std::length_error);
    }
  }

  SECTION("Elements can be removed from the middle of DynamicArray", "[remove]")
  {
    REQUIRE_NOTHROW  (a.remove(1));
    REQUIRE          (a[0] == "Test 1");
    REQUIRE_THROWS_AS(a[2] == "Test 2", std::out_of_range);
    REQUIRE          (a[1] == "Test 3");
    
    REQUIRE_NOTHROW  (a.remove(0));
    REQUIRE_THROWS_AS(a[1] == "Test 1", std::out_of_range);
    REQUIRE_THROWS_AS(a[2] == "Test 2", std::out_of_range);
    REQUIRE          (a[0] == "Test 3");
    
    REQUIRE_NOTHROW  (a.remove(0));
    REQUIRE_THROWS_AS(a[0] == "Test 1", std::out_of_range);
    REQUIRE_THROWS_AS(a[1] == "Test 2", std::out_of_range);
    REQUIRE_THROWS_AS(a[2] == "Test 3", std::out_of_range);
  }
}