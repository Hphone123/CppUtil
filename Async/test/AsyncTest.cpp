#include "../src/Async.hpp"

#include <iostream>
#include <stdexcept>

#include "CatchVer.hpp"

/// @brief Async function returning void
/// @param x The amount of seconds to sleep for
__async__(void, voidfunc, int x)
{
  std::cout << "[voidFunc] Recieved Argument: x = " << std::to_string(x) << " ..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(x));
  std::cout << "[voidFunc] Done!" << std::endl;
  return;
}

/// @brief Async function sleeping for `a + b` seconds
/// @param a Amount 1
/// @param b Amount 2
/// @return 100
__async__(int, func, int a, int b)
{
  std::cout << "[func]  Recieved Arguments: a + b = " << std::to_string(a + b) << " ..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(a + b));
  std::cout << "[func]  Done!" << std::endl;
  return 100;
}

TEST_CASE("Test Async", "[async]")
{
  SECTION("Async task will return a value", "[return]")
  {
    REQUIRE_NOTHROW(
      []()
      {
        auto res = CppUtil::Async::async<int>(
          []
          {
            std::cout << "[lambda 1] Starting up..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::cout << "[lambda 1] Done!" << std::endl;
            return 1;
          });

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "[Main Thread] Doing something else..." << std::endl;
        while (!res.isFinished())
        {
          std::this_thread::sleep_for(std::chrono::seconds(1));
        };
        REQUIRE(1 == res.get());
      }());
  }

  SECTION("Asnc task can access captured variables", "[capture]")
  {
    int var = 0;

    REQUIRE_NOTHROW(
      [&]()
      {
        auto res = CppUtil::Async::async<int>(
          [&]
          {
            std::cout << "[lambda 2] Starting up..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            var = 5;
            std::cout << "[lambda 2] Done!" << std::endl;
            return 1;
          });

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "[Main Thread] Doing something else..." << std::endl;
        while (!res.isFinished())
        {
          std::this_thread::sleep_for(std::chrono::seconds(1));
        };
        REQUIRE(1 == res.get());
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
    auto res = CppUtil::Async::async<int>(
      []
      {
        std::cout << "[lambda 3] Starting up..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "[lambda 3] Done; Throwing exception..." << std::endl;
        throw std::logic_error("Test exception!");
        return 1;
      });

    std::cout << "[Main Thread] Doing something else..." << std::endl;

    REQUIRE_THROWS_MATCHES(res.get(), std::logic_error, ExceptionMessageMatcher{});
  }

  SECTION("__async__ macro creates working async functions", "[macro]")
  {
    auto res = func(1, 2);

    std::cout << "[Main Thread] Doing something else..." << std::endl;

    REQUIRE(100 == res.get());
  }

  SECTION("__async__ void functions will behave normally", "[void]")
  {
    REQUIRE_NOTHROW(voidfunc(3).get());
  }

  SECTION("__async__ functions are awaitable", "[await]")
  {
    REQUIRE(100 == __await__(func, 1, 2));
  }
}
