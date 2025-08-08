#include <catch2/catch.hpp>
#include <iostream>
#include <stdexcept>

#include "../src/Async.hpp"

TEST_CASE("Test Async", "[async]")
{
  SECTION("Async task will return a value", "[return]")
  {
    REQUIRE_NOTHROW([]()
    {
      auto res = CppUtil::Async::async<int>([]
        {
          std::cout << "[Async Task]  Starting up..." << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(3));
          std::cout << "[Async Task]  Done!" << std::endl;
          return 1;
        });
        
        int i = 0;
        while(!res->isFinished())
        {
          std::this_thread::sleep_for(std::chrono::seconds(1));
          std::cout << "[Main Thread] Waiting " << std::to_string(++i) << "s..." << std::endl;
      };
      REQUIRE(1 == res.get()->get());
    }());
  }
  
  
  SECTION("Asnc task can access captured variables", "[capture]")
  {
    int var = 0;
    
    REQUIRE_NOTHROW([&]()
    {
      auto res = CppUtil::Async::async<int>([&]
        {
          std::cout << "[Async Task]  Starting up..." << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(3));
          var = 5;
          std::cout << "[Async Task]  Done!" << std::endl;
          return 1;
        });
  
      int i = 0;

      while(!res->isFinished())
      {
          std::this_thread::sleep_for(std::chrono::seconds(1));
          std::cout << "[Main Thread] Waiting " << std::to_string(++i) << "s..." << std::endl;
      };
        REQUIRE(1 == res.get()->get());
        REQUIRE(5 == var);
    }());
  }

  struct ExceptionMessageMatcher
  {
    bool match(const std::logic_error& ex) const 
    {
        return std::string(ex.what()) == "Test exception!";
    }

    std::string toString() const 
    {
        return "matches exceptions with message 'Test exception!'.";
    }
  };
  
  SECTION("Async task's exception will throw when getting.", "[throw]")
  {
    REQUIRE_THROWS_MATCHES
    ( 
      []()
      {
        auto res = CppUtil::Async::async<int>([]
        {
          std::cout << "[Async Task]  Starting up..." << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(1));
          std::cout << "[Async Task]  Done!" << std::endl;
          throw std::logic_error("Test exception!");
          return 1;
        });

        res->get();
      }(), 
      std::logic_error, 
      ExceptionMessageMatcher{}
    );
  }
}
