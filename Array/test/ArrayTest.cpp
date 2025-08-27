#include "CatchVer.hpp"
#include <stdexcept>

#include "Array.hpp"

TEST_CASE("Array Access", "[array][access]")
{
  Array<int> a(11);
  a[0]  = 0;
  a[1]  = 1;
  a[10] = 123456;

  SECTION("Array elements can be accessed")
  {
    REQUIRE(a[0] == 0);
    REQUIRE(a[1] == 1);
    REQUIRE(a[10] == 123456);

    REQUIRE_NOTHROW(a[10] = -10);

    REQUIRE(a[10] == -10);
  }

  SECTION("Array cannot be accessed out of bounds")
  {
    REQUIRE_THROWS_AS(a[11] = 1    , std::out_of_range);
    REQUIRE_THROWS_AS(a[10] = a[11], std::out_of_range);
    REQUIRE_THROWS_AS(a[-1] = 1    , std::out_of_range);
  }
}

TEST_CASE("Array Init", "[array][init]")
{

  SECTION("Array can be initialized with existing pointer")
  {
    int * ptr = new int[5];
    REQUIRE_NOTHROW([&]() 
    {
      Array<int> arr(ptr, 5);
      delete [] ptr;
    }());
  }

  SECTION("Array size can be a positive number")
  {
    Array<int> arr(5);
    REQUIRE(5 == arr.getSize());
  }

  SECTION("Array size can be zero")
  {
    Array<int> arr;
    REQUIRE_NOTHROW(arr = Array<int>(0));
    REQUIRE(0 == arr.getSize());
  }
  SECTION("Array size cannot be a negative number")
  {
    Array<int> arr;
    REQUIRE_THROWS_AS(arr = Array<int>(-1), length_error);
  }

  SECTION("Array can be initialized with copy constructor")
  {
    Array<int> arr(5);
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;
    arr[4] = 5;
    
    Array<int> arrCopy(arr);

    REQUIRE(arrCopy.getSize() == arr.getSize());
    REQUIRE(arrCopy[0] == arr[0]);
    REQUIRE(arrCopy[1] == arr[1]);
    REQUIRE(arrCopy[2] == arr[2]);
    REQUIRE(arrCopy[3] == arr[3]);
    REQUIRE(arrCopy[4] == arr[4]);
  }
}

TEST_CASE("Array Equality", "[array][equal]")
{
  Array<int> arr1(5);
  arr1[0] = 1;
  arr1[1] = 2;
  arr1[2] = 3;
  arr1[3] = 4;
  arr1[4] = 5;
  
  Array<int> arr2(arr1);
  Array<int> arr3(5);
  arr3[0] = 1;
  arr3[1] = 2;
  arr3[2] = 3;
  arr3[3] = 4;
  arr3[4] = 5;


  SECTION("An array is equal to itself")
  {
    
    REQUIRE(arr1 == arr1);
  }

  SECTION("Two arrays with the same elements are equal")
  {
    REQUIRE(arr1 == arr2);
    REQUIRE(arr1 == arr3);
    REQUIRE(arr1 == arr3);
  }
  
  SECTION("Two arrays with different elements are not equal")
  {
    arr2[0] = 0;
    arr3[4] = 0;
    
    REQUIRE(arr1 != arr2);
    REQUIRE(arr1 != arr3);
    REQUIRE(arr2 != arr3);
  }
}

TEST_CASE("Array can be cast", "[array][cast]")
{
  SECTION("Array pointers can be accessed via cast operator")
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

TEST_CASE("Array Size", "[array][size]")
{
  SECTION("Array size will match constructors")
  {
    Array<int> arr1(187);
    int * tmp = new int[257];
    Array<int> arr2(tmp, 257);
    
    REQUIRE(187 == arr1.getSize());
    REQUIRE(257 == arr2.getSize());
  }
  
  SECTION("Array will be marked as empty when initialized with 0")
  {
    Array<int> arr1(0);
    int * tmp = new int[0];
    Array<int> arr2(tmp, 0);
    
    REQUIRE(arr1.isEmpty());
    REQUIRE(arr2.isEmpty());
  }
}

TEST_CASE("Elements can be searched and found in Array", "[array][find]")
{
  Array<int> arr(5);
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;
  arr[3] = 4;
  arr[4] = 5;
  
  SECTION("Existing elements can be found")
  {
    REQUIRE(arr.find(1).getSize() == 1);
    REQUIRE(arr.find(2).getSize() == 1);
    REQUIRE(arr.find(3).getSize() == 1);
    REQUIRE(arr.find(4).getSize() == 1);
    REQUIRE(arr.find(5).getSize() == 1);
  }
  
  SECTION("Finding existing elements returns the correct index")
  {
    REQUIRE(arr.find(1)[0] == 0);
    REQUIRE(arr.find(2)[0] == 1);
    REQUIRE(arr.find(3)[0] == 2);
    REQUIRE(arr.find(4)[0] == 3);
    REQUIRE(arr.find(5)[0] == 4);
  }

  SECTION("Finding multiple elements returns all indicees")
  {
    arr[4] = 1;

    REQUIRE(arr.find(1).getSize() == 2);

    REQUIRE(arr.find(1)[0] == 0);
    REQUIRE(arr.find(1)[1] == 4);
  }

  SECTION("Searching non-existing elements returns an empty array")
  {
    REQUIRE(arr.find(0).isEmpty());
  }
}

TEST_CASE("Array Loop", "[array][loop]")
{
  Array<int> arr(5);
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;
  arr[3] = 4;
  arr[4] = 5;

  SECTION("Foreach will do something for each element in the array")
  {
    arr.foreach([](int& x)
    {
      x += 1;
    });

    REQUIRE(2 == arr[0]);
    REQUIRE(3 == arr[1]);
    REQUIRE(4 == arr[2]);
    REQUIRE(5 == arr[3]);
    REQUIRE(6 == arr[4]);
  }

  SECTION("Any will return true if the function is true for any element")
  {
    REQUIRE(arr.any([](int& x){return x == 5;}));
  }

  SECTION("Any will return false if the function is false for all elements")
  {
    REQUIRE(!arr.any([](int& x){return x == 6;}));
  }

  SECTION("All will return true if the function is true for all elements")
  {
    REQUIRE(arr.all([](int& x){return x > 0;}));
  }

  SECTION("All will return false if the function is false for any element")
  {
    REQUIRE(!arr.all([](int& x){return x < 5;}));
  }
}