#include <catch2/catch.hpp>

#include "../src/Array.hpp"

TEST_CASE("Test Array", "[array]")
{
  Array<int> a = Array<int>(11);
  a[0]  = 0;
  a[1]  = 1;
  a[10] = 123456;

  SECTION("Array can be accessed in valid indeces", "[access]")
  {
    REQUIRE(a[0] == 0);
    REQUIRE(a[1] == 1);
    REQUIRE(a[10] == 123456);

    a[10] = -10;

    REQUIRE(a[10] == -10);
  }

  SECTION("Array cannot be accessed out of bounds", "[out_of_range]")
  {
    REQUIRE_THROWS_AS(a[11] = 1, std::out_of_range);

    int x;
    REQUIRE_THROWS_AS(x = a[11], std::out_of_range);
    REQUIRE_THROWS_AS(a[-1] = 1, std::out_of_range);
  }

  SECTION("Array can be initialized with existing pointer", "[init]")
  {
    #define FÜNNÜF 5
    int * ptr = new int[FÜNNÜF];
    REQUIRE_NOTHROW([&]() 
    {
      auto arr = Array<int>(ptr, FÜNNÜF);
      delete [] ptr;
    }());
  }

  SECTION("Array pointers can be accessed via cast operator", "[cast]")
  {
    auto arr = Array<int>(20);
    REQUIRE_NOTHROW([&]()
    {
      int * ptr = arr;
      ptr[19] = 100;
      REQUIRE(arr[19] == 100);
    }());
  }
}

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

