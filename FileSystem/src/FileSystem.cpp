#include "FileSystem.hpp"

#include <cstdint>
#include <cstdio>

#include "Array.hpp"
#include "Async.hpp"
#include "Platform.hpp"
#if defined(PLATFORM_UNIX_LIKE)
#include <dirent.h>
#include <sys/stat.h>
#elif defined(PLATFORM_WINDOWS)
// ...
#endif

using namespace CppUtil;

Path::Path() : Path(".") {}

Path::Path(const String path, const bool scan) :
    path(path), abs(false), is_directory(false), exists(false), flags(0), children()
{
#if defined(PLATFORM_UNIX_LIKE)
  this->path.replace("\\", Platform::PathSeparator);
#elif defined(PLATFORM_WINDOWS)
  this->path.replace("/", Platform::PathSeparator);
#endif

  if (this->path[this->path.last()] == Platform::PathSeparator && this->path.length() > 1)
  {
    this->path.remove(this->path.last(), 1);
  }

#if defined(PLATFORM_UNIX_LIKE)
  if (this->path[0] == Platform::PathSeparator)
  {
    this->abs = true;

    if (auto d = opendir(this->path); d == nullptr)
    {
      this->is_directory = false;
      if (auto f = fopen(this->path, "r"); f == nullptr)
      {
        this->exists = false;
      }
      else
      {
        this->exists = true;
        fclose(f);
      }
    }
    else
    {
      this->is_directory = true;

      if (scan)
      {
        DynamicArray<Path> d_entries;
        while (auto entry = readdir(d))
        {
          d_entries.add(Path((String)this->path + (String)Platform::PathSeparator + String(entry->d_name), false));
        }
        this->children = d_entries.toArray();
      }

      closedir(d);
    }
  }
#elif defined(PLATFORM_WINDOWS)
  throw not_implemented_exception("Path parsing not implemented for Windows yet!");
#endif
}

Path::Path(const Path& base, const String relative) : Path(base + (Path)relative) {}

Path::Path(const Path& other) :
    path(other.path), abs(other.abs), is_directory(other.is_directory), flags(other.flags), children(other.children)
{
}

Path& Path::operator=(const Path& other)
{
  if (this != &other)
  {
    this->path         = other.path;
    this->abs          = other.abs;
    this->is_directory = other.is_directory;
    this->flags        = other.flags;
    this->children     = other.children;
  }
  return *this;
}

const void Path::eval(const Path pwd)
{
  if (this->abs)
  {
    return;
  }
  else
  {
    *this = Path(pwd + *this);
  }
}

const String Path::get() const
{
  return this->path;
}

const bool Path::is_existant() const
{
  return this->exists;
}

const bool Path::is_dir() const
{
  return this->is_directory;
}

const Array<Path>& Path::get_children() const
{
  return this->children;
}

Path::operator const String() const
{
  return this->path;
}

Path::operator const char *() const
{
  return this->path;
}

const Path Path::operator+(const Path& other) const
{
  if (other.abs)
  {
    throw std::runtime_error("Cannot add absolte path '" + (std::string)other.path + "' to path '" +
                             (std::string)this->path + "'!"); // ToDo: Custom Exception
  }
  else
  {
    String newpath = this->path;
    if (newpath[newpath.last()] != Platform::PathSeparator)
    {
      newpath += (String)Platform::PathSeparator;
    }
    newpath += other.path;
    return Path(newpath);
  }
}

File::File(const Path path, const IO_MODE mode) : file(nullptr), size(0), mode(mode)
{
  String mode_str;
  switch (mode)
  {
    case IO_MODE::READ_ONLY:
      mode_str = "r";
      break;
    case IO_MODE::WRITE_ONLY:
      mode_str = "w";
      break;
    case IO_MODE::READ_WRITE:
      mode_str = "r+w";
      break;
    default:
      throw std::invalid_argument("Invalid IO_MODE given!");
  }

  if (!path.is_existant())
  {
    throw std::runtime_error("File '" + (std::string)path.get() + "' does not exist!");
  }

  if (path.is_dir())
  {
    throw std::runtime_error("Path '" + (std::string)path.get() + "' is a directory, not a file!");
  }

  if (path.is_absolute())
  {
    throw std::invalid_argument("Path '" + (std::string)path.get() + "' is not absolute!");
  }

  this->file = fopen(path.get(), mode_str);
  if (this->file == nullptr)
  {
    throw std::runtime_error("Could not open file '" + (std::string)path.get() + "'!");
  }

  fseek(this->file, 0, SEEK_END);
  this->size = ftell(this->file);
  fseek(this->file, 0, SEEK_SET);
}

File::~File()
{
  if (this->file != nullptr)
  {
    fclose(this->file);
    this->file = nullptr;
  }
}

__async_member_impl__(const Array<uint8_t>, File, read, size_t size)
{
  if (this->mode == IO_MODE::WRITE_ONLY)
  {
    throw std::runtime_error("File not opened in read mode!");
  }

  if (size == 0 || size > this->size)
  {
    size = this->size;
  }

  Array<uint8_t> buffer(size);
  size_t         read_bytes = fread((void *)buffer, 1, size, this->file);
  if (read_bytes != size)
  {
    throw std::runtime_error("Could not read " + std::to_string(size) + " bytes from file!");
  }

  return buffer;
}

__async_member_impl__(const size_t, File, write, const Array<uint8_t>& data)
{
  if (this->mode == IO_MODE::READ_ONLY)
  {
    throw std::runtime_error("File not opened in write mode!");
  }

  size_t written_bytes = fwrite((const void *)data, 1, data.getSize(), this->file);
  if (written_bytes != data.getSize())
  {
    throw std::runtime_error("Could not write " + std::to_string(data.getSize()) + " bytes to file!");
  }

  return written_bytes;
}

__async_member_impl__(const Array<uint8_t>, File, read_from, const size_t offset, const size_t size)
{
  if (this->mode == IO_MODE::WRITE_ONLY)
  {
    throw std::runtime_error("File not opened in read mode!");
  }

  if (offset >= this->size)
  {
    throw std::out_of_range("Offset " + std::to_string(offset) + " out of bounds for file of size " +
                            std::to_string(this->size) + "!");
  }

  size_t read_size = size;
  if (size == 0 || offset + size > this->size)
  {
    read_size = this->size - offset;
  }

  fseek(this->file, offset, SEEK_SET);

  Array<uint8_t> buffer(read_size);
  size_t         read_bytes = fread((void *)buffer, 1, read_size, this->file);
  if (read_bytes != read_size)
  {
    throw std::runtime_error("Could not read " + std::to_string(read_size) + " bytes from file!");
  }

  return buffer;
}

__async_member_impl__(const size_t, File, write_from, const size_t offset, const Array<uint8_t>& data)
{
  if (this->mode == IO_MODE::READ_ONLY)
  {
    throw std::runtime_error("File not opened in write mode!");
  }

  if (offset > this->size)
  {
    throw std::out_of_range("Offset " + std::to_string(offset) + " out of bounds for file of size " +
                            std::to_string(this->size) + "!");
  }

  fseek(this->file, offset, SEEK_SET);

  size_t written_bytes = fwrite((const void *)data, 1, data.getSize(), this->file);
  if (written_bytes != data.getSize())
  {
    throw std::runtime_error("Could not write " + std::to_string(data.getSize()) + " bytes to file!");
  }

  return written_bytes;
}

FileSystem::FileSystem() : path() {}

FileSystem::FileSystem(const Path pwd) : path(pwd) {}

String FileSystem::pwd() const
{
  return this->path.get();
}

Array<Path> FileSystem::ls() const
{
  return this->ls(this->pwd());
}

Array<Path> FileSystem::ls(const Path path) const
{
  Path p = path;
  if (!p.is_absolute())
  {
    p.eval(this->pwd());
  }

  if (!p.is_existant())
  {
    throw std::runtime_error("Path '" + (std::string)p.get() + "' does not exist!");
  }

  if (!p.is_dir())
  {
    throw std::runtime_error("Path '" + (std::string)p.get() + "' is not a directory!");
  }

  return p.get_children();
}

const File FileSystem::open(const Path path, const IO_MODE mode) const
{
  Path p = path;
  if (!p.is_absolute())
  {
    p.eval(this->pwd());
  }

  return File(p, mode);
}

const File FileSystem::create(const Path path) const
{
  Path p = path;
  if (!p.is_absolute())
  {
    p.eval(this->pwd());
  }

  if (p.is_existant())
  {
    throw std::runtime_error("File '" + (std::string)p.get() + "' already exists!");
  }

  return File(p, IO_MODE::READ_WRITE);
}

const Path FileSystem::mkdir(const Path path) const
{
  Path p = path;
  if (!p.is_absolute())
  {
    p.eval(this->pwd());
  }

  if (p.is_existant())
  {
    throw std::runtime_error("Directory '" + (std::string)p.get() + "' already exists!");
  }

#if defined(PLATFORM_UNIX_LIKE)
  if (::mkdir(p.get(), 0755) != 0)
  {
    throw std::runtime_error("Could not create directory '" + (std::string)p.get() + "'!");
  }
#elif defined(PLATFORM_WINDOWS)
  throw not_implemented_exception("Directory creation not implemented for Windows yet!");
#endif

  return p;
}

const Path FileSystem::cd(const Path path)
{
  Path p = path;
  if (!p.is_absolute())
  {
    p.eval(this->pwd());
  }

  if (!p.is_existant())
  {
    throw std::runtime_error("Path '" + (std::string)p.get() + "' does not exist!");
  }

  throw std::runtime_error("Path '" + (std::string)p.get() + "' is not a directory!");

  this->path = p;
  return this->path;
}