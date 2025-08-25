#if defined(CATCH_VERSION_MAJOR) && CATCH_VERSION_MAJOR >= 3
  // Catch2 v3 headers
  #include <catch2/catch_all.hpp>
#else
  // Assume v2
  #include <catch2/catch.hpp>
#endif

#include "String.hpp"

TEST_CASE("String Init", "[string][init]")
{
  String s;

  SECTION("String can be constructed")
  {
    REQUIRE_NOTHROW(s = String());
  }

  SECTION("String can be constructed from string constant")
  {
    REQUIRE_NOTHROW(s = String("AMOGUS"));
    REQUIRE(s == "AMOGUS");
  }

  SECTION("String can be constructed from std::string")
  {
    std::string str = "AMOG-SUS";
    REQUIRE_NOTHROW(s = String(str));
    REQUIRE(s == "AMOG-SUS");
  }
}

TEST_CASE("String concat", "[string][concat]")
{
  String s1 = "Hello";
  String s2 = "World";

  SECTION("Two strings can be concatenated")
  {
    REQUIRE((s1 + s2) == "HelloWorld");
    REQUIRE((s2 + s1) == "WorldHello");
    REQUIRE((s1 + s1) == "HelloHello");
    REQUIRE((s2 + s2) == "WorldWorld");
  }

  SECTION("Two strings can be concatenated with string constants")
  {
    REQUIRE((s1 + " " + s2 + "!") == "Hello World");
  }
  
  SECTION("A String can be concatenated with an empty string constant")
  {
    REQUIRE((s1 + "") == "Hello");

    REQUIRE((s1 + "") == s1);
  }
}

TEST_CASE("String equality")
{
  String s1 = "Hello", s2 = "World", s3 = "Hello World";
  
  SECTION("A String is equal to itself")
  {
    REQUIRE(s1 == s1);
    REQUIRE(s2 == s2);
    REQUIRE(s3 == s3);
  }
  
  SECTION("A String is equal to a Matching string constant")
  {
    REQUIRE(s1 == "Hello");
    REQUIRE(s2 == "World");
    REQUIRE(s3 == "Hello World");
  }
  
  SECTION("A String is equal to another string with the same content")
  {
    String s4 = "Hello", s5 = "World", s6 = "Hello World";
    
    REQUIRE(s1 == s4);
    REQUIRE(s2 == s5);
    REQUIRE(s3 == s6);
  }
  
  SECTION("A String is not unequal to  itself")
  {
    REQUIRE_FALSE(s1 != s1);
    REQUIRE_FALSE(s2 != s2);
    REQUIRE_FALSE(s3 != s3);
  }
  
  SECTION("A String is unequal to another different String")
  {
    REQUIRE(s1 != s2);
    REQUIRE(s1 != s3);
    REQUIRE(s2 != s3);
  }
  
  SECTION("A String is not unequal to another String with the same content")
  {
    String s4 = "Hello", s5 = "World", s6 = "Hello World";
    
    REQUIRE_FALSE(s1 != s4);
    REQUIRE_FALSE(s2 != s5);
    REQUIRE_FALSE(s3 != s6);
  }
}

TEST_CASE("String length", "[string][length]")
{
  String s1 = "Hello", s2 = "World", s3 = "Hello World";
  
  REQUIRE(s1.length() == 5);
  REQUIRE(s2.length() == 5);
  REQUIRE(s3.length() == 11);
}

TEST_CASE("String Remove Element", "[string][remove]")
{
  String s1 = "Hello", s2 = "World", s3 = "Hello World";

  SECTION("Elements can be removed from a String")
  {
    REQUIRE_NOTHROW(s1.remove(1, 2));
    REQUIRE(s1 == "Hlo");

    REQUIRE_NOTHROW(s3.remove(5, 6));
    REQUIRE(s3 == "Hello");
  }
}