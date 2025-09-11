#if defined(BUILD_TESTS)
#if defined __has_include
#if __has_include(<catch2/catch_all.hpp>)
// #pragma message ("Using catch2 v3...")
#include <catch2/catch_all.hpp>
#elif __has_include(<catch2/catch.hpp>)
// #pragma message("Using catch2 v2...")
#include <catch2/catch.hpp>
#else
#error No suitable catch2 header available!
#endif
#else
#warning Cannot check catch2 availability, using catch2 v2...
#include <catch2/catch.hpp>
#endif
#else
#pragma message("Building without tests, Catch2 disabled.")
#endif