#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "Array.hpp"
#include "Async.hpp"
#include "String.hpp"

namespace CppUtil
{
enum class IO_MODE
{
  READ_ONLY,
  WRITE_ONLY,
  READ_WRITE,
  CREATE_WO,
  CREATE_RW,
  // ...
  IO_MODE_COUNT
};

class Path
{
private:
  String path;

  bool abs;
  bool is_directory;
  bool exists;

  uint16_t flags;

  Array<Path> children;

public:
  /// @brief Create a Path  using '.'
  Path();

  /// @brief Create a Path using the given path
  /// @param path The path to use
  /// @note Only absolute paths will be evaluated here; use `eval(pwd)` to evaluate relative paths later
  Path(const String& path, const bool scan = true);
  Path(const char * path, const bool scan = true);
  Path(const Path& base, const String relative);

  Path(const Path& other);
  Path& operator=(const Path& other);

  /// @brief Re-Evaluate the path
  const void eval();

  /// @brief Evaluate the path from a given present working directory
  /// @param pwd The present working directory
  const void eval(const Path pwd);

  const String       get() const;
  const bool         is_absolute() const;
  const bool         is_existant() const;
  const bool         is_dir() const;
  const Array<Path>& get_children() const;

  operator const String() const;
  operator const char *() const;

  const Path operator+(const Path& other) const;
};

class File
{
private:
  Path    path;
  FILE *  file;
  size_t  size;
  IO_MODE mode;

public:
  File(const Path path, const IO_MODE mode = IO_MODE::READ_WRITE);
  ~File();

  // void reopen(const IO_MODE mode = IO_MODE::READ_WRITE);
  // void close();

  const bool is_readable() const;
  const bool is_writable() const;

  void stat();

  __async_member_decl__(Array<uint8_t>, read, size_t size = 0);
  __async_member_decl__(size_t, write, const Array<uint8_t>& data);

  __async_member_decl__(Array<uint8_t>, read_from, const size_t offset, const size_t size = 0);
  __async_member_decl__(size_t, write_from, const size_t offset, const Array<uint8_t>& data);
};

class FileSystem
{
private:
  Path path;

public:
  FileSystem();
  FileSystem(const Path pwd);

  String pwd() const;

  Array<Path> ls() const;
  Array<Path> ls(const Path path) const;

  const File open(const Path& path, const IO_MODE mode = IO_MODE::READ_WRITE) const;
  const File create(const Path& path) const;

  const Path mkdir(const Path& path) const;

  const Path cd(const Path& path);
};

#define CurrentFileSystem FileSystem(argv[0])
} // namespace CppUtil