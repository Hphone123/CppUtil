#include "FileSystem.hpp"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <iostream>
#include <string>

#include "CatchVer.hpp"

using namespace CppUtil;

std::string pwd = std::filesystem::current_path().lexically_normal().string();

TEST_CASE("Test FileSystem", "[fs]")
{
  FileSystem fs((Path)pwd);
  SECTION("PWD returns the current working directory")
  {
    REQUIRE((std::string)fs.pwd() == pwd);
  }

  SECTION("MKDIR creates a new directory")
  {
    // Romove dir if already exists
    std::filesystem::remove_all(pwd + "/test_dir");

    auto d = fs.mkdir((Path) "test_dir");
    REQUIRE(d.is_dir());
    REQUIRE(d.get_children().getSize() == 0);
    REQUIRE(std::string(d) == pwd + Platform::PathSeparator + "test_dir");
  }

  // SECTION("CD changes the working directory")
  // {
  //   REQUIRE_NOTHROW(fs.cd(Path("test_dir")));
  //   REQUIRE((std::string)fs.pwd() == pwd + Platform::PathSeparator + "test_dir");
  // }
}