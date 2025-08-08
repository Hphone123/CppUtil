#include <catch2/catch.hpp>

#include "Map.hpp"

TEST_CASE("Test Square Brackets", "[square_brackets]")
{
  Map<std::string, int> m;
  
  REQUIRE_NOTHROW([&](){
    m["One"]    = 1;
    m["Two"]    = 2;
    m["Three"]  = 3;

    REQUIRE(m["One"]    == 1);
    REQUIRE(m["Two"]    == 2);
    REQUIRE(m["Three"]  == 3);

    m["Three"] = 4;

    REQUIRE(m["Three"]  == 4);

    REQUIRE(m["Five"] == 0);
  }());
}

TEST_CASE("Get / Set / Remove item", "[non_add_operation]")
{
  Map<std::string, std::string> m;

  REQUIRE_NOTHROW([&](){
    m["1"] = "1";
    m["2"] = "2";
    m["3"] = "3";
    m["4"] = "4";

    REQUIRE(m.tryGetItem("1") == "1");
    REQUIRE(m.tryGetItem("2") == "2");
    REQUIRE(m.tryGetItem("3") == "3");
    REQUIRE(m.tryGetItem("4") == "4");
    
    REQUIRE_THROWS_AS(m.tryGetItem("5") == "5", not_found);
    REQUIRE_THROWS_AS(m.trySetItem("5", "5")  , not_found);
    REQUIRE_THROWS_AS(m.tryGetItem("5") == "5", not_found);
    
    m.tryRemoveItem("2");
    
    REQUIRE_THROWS_AS(m.tryRemoveItem("2")    , not_found);
    REQUIRE_THROWS_AS(m.tryGetItem("2")       , not_found);
    REQUIRE_THROWS_AS(m.trySetItem("2", "2")  , not_found);
    
    REQUIRE(m.tryGetItem("1") == "1");
    REQUIRE(m.tryGetItem("3") == "3");
    REQUIRE(m.tryGetItem("4") == "4");

    m.trySetItem("1", "5");

    REQUIRE(m.tryGetItem("1") == "5");
  }());
}

TEST_CASE("Map can be initialized with initializer list", "[initialize]")
{
  REQUIRE_NOTHROW([&](){

    Map<std::string, double> m 
    {
      {"1,1", 1.1},
      {"1,2", 1.2},
      {"1,3", 1.3}
    };

    REQUIRE(m["1,1"] == 1.1);
    REQUIRE(m["1,2"] == 1.2);
    REQUIRE(m["1,3"] == 1.3);
  }());
};