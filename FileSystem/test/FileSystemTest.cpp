#include "FileSystem.hpp"

#include <filesystem>
#include <string>

#include "CatchVer.hpp"

using namespace CppUtil;

std::string pwd = std::filesystem::current_path().lexically_normal().string();

TEST_CASE("Test FileSystem", "[fs]")
{
  FileSystem fs;
  SECTION("PWD returns the current working directory")
  {
    REQUIRE((std::string)fs.pwd() == pwd);
  }

  SECTION("MKDIR creates a new directory")
  {
    auto d = fs.mkdir(Path(fs.pwd(), "test_dir"));
    // REQUIRE(d.is_dir());
    // REQUIRE(d.get_children().getSize() == 0);
    // REQUIRE(std::string(d) == pwd + Platform::PathSeparator + "test_dir");

    // Cleanup
    std::filesystem::remove_all((const char *)d);
  }
}