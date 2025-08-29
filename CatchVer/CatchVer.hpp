#if defined __has_include
#if __has_include(<catch2/catch_all.hpp>)
#include <catch2/catch_all.hpp>
#elif __has_include(<catch2/catch.hpp>)
#include <catch2/catch.hpp>
#else
#error No suitable catch2 header available!
#endif
#else
#warning Cannot check catch2 availability, using catch2 v2...
#include <catch2/catch.hpp>
#endif