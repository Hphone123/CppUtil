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
  // ...
  IO_MODE_COUNT
};

class Path
{
private:
  String path;
  bool   is_directory;

  Array<String> children;

public:
  Path();
  Path(const String path);
  Path(const Path& base, const String relative);

  Path(const Path& other);

  const char *         get() const;
  const bool           exists() const;
  const bool           is_dir() const;
  const Array<String>& get_children() const;

  operator const String() const;
  operator const char *() const;
};

class File
{
private:
  FILE *  file;
  size_t  size;
  IO_MODE mode;

public:
  File(const Path path, const IO_MODE mode = IO_MODE::READ_WRITE);
  ~File();

  __async_member_decl__(const Array<uint8_t>, read, const size_t size = 0)
    __async_member_decl__(const size_t, write, const Array<uint8_t>& data)

      __async_member_decl__(const Array<uint8_t>, read_from, const size_t offset, const size_t size = 0)
        __async_member_decl__(const size_t, write_from, const size_t offset, const Array<uint8_t>& data)
};

class FileSystem
{
private:
  Path path;

public:
  FileSystem();
  FileSystem(const Path pwd);

  String pwd() const;

  const File open(const Path path, const IO_MODE mode = IO_MODE::READ_WRITE) const;
  const File create(const Path path) const;

  const Path mkdir(const Path path) const;

  const Path cd(const Path relative);
};
} // namespace CppUtil