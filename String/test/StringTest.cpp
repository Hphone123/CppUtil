#include "String.hpp"

#include "CatchVer.hpp"

using namespace CppUtil;

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
    REQUIRE(s == "AMOGUS"_str);
  }

  SECTION("String can be constructed from std::string")
  {
    std::string str = "AMOG-SUS"_str;
    REQUIRE_NOTHROW(s = String(str));
    REQUIRE(s == "AMOG-SUS"_str);
  }
}

TEST_CASE("String concat", "[string][concat]")
{
  String s1 = "Hello"_str;
  String s2 = "World"_str;

  SECTION("Two strings can be concatenated")
  {
    REQUIRE((s1 + s2) == "HelloWorld"_str);
    REQUIRE((s2 + s1) == "WorldHello"_str);
    REQUIRE((s1 + s1) == "HelloHello"_str);
    REQUIRE((s2 + s2) == "WorldWorld"_str);
  }

  SECTION("Two strings can be concatenated with string constants")
  {
    REQUIRE((s1 + " "_str + s2 + "!"_str) == "Hello World!"_str);
  }

  SECTION("A String can be concatenated with an empty string constant")
  {
    REQUIRE((s1 + "") == "Hello"_str);

    REQUIRE((s1 + "") == s1);
  }
}

TEST_CASE("String equality")
{
  String s1 = "Hello"_str, s2 = "World"_str, s3 = "Hello World"_str;

  SECTION("A String is equal to itself")
  {
    REQUIRE(s1 == s1);
    REQUIRE(s2 == s2);
    REQUIRE(s3 == s3);
  }

  SECTION("A String is equal to a Matching string constant")
  {
    REQUIRE(s1 == "Hello"_str);
    REQUIRE(s2 == "World"_str);
    REQUIRE(s3 == "Hello World"_str);
  }

  SECTION("A String is equal to another string with the same content")
  {
    String s4 = "Hello"_str, s5 = "World"_str, s6 = "Hello World"_str;

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
    String s4 = "Hello"_str, s5 = "World"_str, s6 = "Hello World"_str;

    REQUIRE_FALSE(s1 != s4);
    REQUIRE_FALSE(s2 != s5);
    REQUIRE_FALSE(s3 != s6);
  }
}

TEST_CASE("String length", "[string][length]")
{
  String s1 = "Hello"_str, s2 = "World"_str, s3 = "Hello World"_str;

  REQUIRE(s1.length() == 5);
  REQUIRE(s2.length() == 5);
  REQUIRE(s3.length() == 11);
}

TEST_CASE("String Remove Element", "[string][remove]")
{
  String s1 = "Hello"_str, s2 = "World"_str, s3 = "Hello World"_str;

  SECTION("Elements can be removed from a String")
  {
    REQUIRE_NOTHROW(s1.remove(1, 2));
    REQUIRE(s1 == "Hlo"_str);

    REQUIRE_NOTHROW(s3.remove(5, 6));
    REQUIRE(s3 == "Hello"_str);
  }
}

TEST_CASE("String Insert Element", "[string][insert]")
{
  String s1 = "Hell", s2 = "o World", s3 = "Hellorld";

  SECTION("Strings can be appended")
  {
    s1.insert("o", 4);
    REQUIRE(s1 == "Hello"_str);
  }

  SECTION("Strings can be inserted")
  {
    s3.insert("o W", 4);
    REQUIRE(s3 == "Hello World"_str);
  }
}