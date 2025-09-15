#include "FileSystem.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include "CatchVer.hpp"
#include "Platform.hpp"

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
    REQUIRE(d.get_children().getSize() == 2); //? '.' & '..'
    REQUIRE(std::string(d) == pwd + Platform::PathSeparator + "test_dir");
  }

  SECTION("MKDIR cannot re-create a directory", "[mkdir]")
  {
    REQUIRE_THROWS_AS(fs.mkdir("test_dir"), std::runtime_error);
  }

  SECTION("CD changes the working directory")
  {
    REQUIRE_NOTHROW(fs.cd(Path("test_dir")));
    REQUIRE((std::string)fs.pwd() == pwd + Platform::PathSeparator + "test_dir");
  }

  SECTION("CD works with '..'")
  {
    REQUIRE_NOTHROW(fs.cd(Path("..")));
    REQUIRE((std::string)fs.pwd() == std::filesystem::current_path().parent_path().lexically_normal().string());
  }

  SECTION("CD cannot go to non-existant directories")
  {
    REQUIRE_THROWS_AS(fs.cd("garbage_dir"), std::runtime_error);
  }

  SECTION("LS lists the contents of a directory", "[ls]")
  {
    auto children = fs.ls();

    REQUIRE(children.getSize() >= 3); // '.', '..', 'test_dir', ...
    REQUIRE(children.any([fs](Path& p) { return p.get() == fs.pwd() + (String)Platform::PathSeparator + "test_dir"; }));
  }

  SECTION("LS lists the contents of any directory", "[ls]")
  {
    REQUIRE(fs.ls(".."));
  }

  SECTION("LS cannot list contents of nonexistant directories", "[ls]")
  {
    REQUIRE_THROWS_AS(fs.ls("garbage_dir"), std::runtime_error);
  }

  SECTION("CREATE creates a new file")
  {
    // Romove dir if already exists
    std::filesystem::remove_all(pwd + "/test_dir/test_file.txt");
    fs.cd("test_dir");
    REQUIRE_NOTHROW(fs.create((Path) "test_file.txt"));
    REQUIRE(
      fs.ls().any([fs](Path& p) { return p.get() == fs.pwd() + (String)Platform::PathSeparator + "test_file.txt"; }));
  }

  SECTION("CD cannot change into files", "[cd][file]")
  {
    REQUIRE_THROWS_AS(fs.cd("test_dir/test_file.txt"), std::runtime_error);
  }

  SECTION("LS cannot list contents of files", "[ls][file]")
  {
    REQUIRE_THROWS_AS(fs.ls("test_dir/test_file.txt"), std::runtime_error);
  }

  SECTION("CREATE cannot re-create a file", "[create]")
  {
    REQUIRE_THROWS_AS(fs.create("test_dir/test_file.txt"), std::runtime_error);
  }

  SECTION("OPEN opens an existing file")
  {
    fs.cd("test_dir");
    REQUIRE_NOTHROW(fs.open((Path) "test_file.txt", IO_MODE::READ_WRITE));
  }

  SECTION("OPEN throws when opening a directory")
  {
    fs.cd("test_dir");
    REQUIRE_THROWS_AS(fs.open("."), std::runtime_error);
  }

  SECTION("OPEN throws when opening nonexistant file", "[open][exist]")
  {
    fs.cd("test_dir");
    REQUIRE_THROWS_AS(fs.open("test_file3.txt"), std::runtime_error);
  }

  SECTION("Cannot concat 2 absolute paths", "[path][concat]")
  {
    Path p1 = "/home";
    Path p2 = "/usr";

    REQUIRE_THROWS_AS(p1 + p2, std::runtime_error);
  }

  SECTION("Path can be converted to CppUtil::String", "[paths][convert]")
  {
    Path p1 = "/home";

    REQUIRE((String) "/home" == (String)p1);
  }

  SECTION("Paths can be evaluated", "[path][eval]")
  {
    Path p1 = "/home";
    Path p2 = "home";

    REQUIRE_NOTHROW(p1.eval("/"));
    REQUIRE_NOTHROW(p2.eval("/"));
    REQUIRE((String)p1 == (String)p2);
  }
}

TEST_CASE("Test File", "[file]")
{
  FileSystem fs((String)pwd);
  fs.cd("test_dir");

  SECTION("Files can be written", "[write]")
  {
    auto f = fs.open("test_file.txt");

    REQUIRE(6 == f.write(Array<uint8_t>{'H', 'e', 'l', 'l', 'o', '!'}).get());
    REQUIRE(6 == f.read().get().getSize());
  }

  SECTION("Files can be read", "[read]")
  {
    auto f = fs.open("test_file.txt", IO_MODE::READ_ONLY);
    REQUIRE(Array<uint8_t>{'H', 'e', 'l', 'l', 'o', '!'} == f.read().get());
  }

  SECTION("Files can be written from a specific index", "[write][idx]")
  {
    auto f = fs.open("test_file.txt", IO_MODE::READ_WRITE);
    REQUIRE(5 == f.write_from(2, Array<uint8_t>{'a', 'v', 'e', 'n', '\n'}).get());
    REQUIRE(Array<uint8_t>{'H', 'e', 'a', 'v', 'e', 'n', '\n'} == f.read().get());
  }

  SECTION("Files can be read from a specific index", "[read][idx]")
  {
    auto f = fs.open("test_file.txt", IO_MODE::READ_ONLY);
    REQUIRE(Array<uint8_t>{'v', 'e', 'n', '\n'} == f.read_from(3).get());
  }

  SECTION("Files can be created in Write only mode", "[create][write-only][write]")
  {
    auto f = fs.open("test_file2.txt", IO_MODE::CREATE_WO);
    REQUIRE(3 == f.write(Array<uint8_t>{'H', 'i', '\n'}).get());
    REQUIRE_THROWS_AS(f.read().get(), std::runtime_error);
  }

  SECTION("Cannot read / write file from offset bigger than file size", "[read][write][offset]")
  {
    auto f = fs.open("test_file.txt", IO_MODE::READ_WRITE);
    REQUIRE_THROWS(f.read_from(20).get());
    REQUIRE_THROWS(f.write_from(10, Array<uint8_t>()).get());
  }

  SECTION("Files cannot be written / read in incompatible mode", "[read][write][mode]")
  {
    auto f = fs.open("test_file.txt", IO_MODE::READ_ONLY);
    REQUIRE_THROWS(f.write(Array<uint8_t>()).get());
    REQUIRE_THROWS(f.write_from(0, Array<uint8_t>()).get());

    auto g = fs.open("test_file.txt", IO_MODE::WRITE_ONLY);
    REQUIRE_THROWS(g.read().get());
    REQUIRE_THROWS(g.read_from(0).get());
  }

  SECTION("Files cannot be opened from relative path directly", "[open][path]")
  {
    REQUIRE_THROWS_AS(File("test_file.txt"), std::runtime_error);
  }

  SECTION("Files cannot be opened using invalid IO mode", "[open][path]")
  {
    REQUIRE_THROWS_AS(fs.open("test_file.txt", IO_MODE::IO_MODE_COUNT), std::invalid_argument);
  }
}