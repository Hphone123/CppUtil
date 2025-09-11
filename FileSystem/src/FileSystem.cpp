#include "FileSystem.hpp"

#include <filesystem>

#include "Platform.hpp"

using namespace CppUtil;

namespace fs = std::filesystem;

Path::Path() : Path(".") {}

Path::Path(const String path) :
    path(fs::absolute(fs::path(path)).lexically_normal().string()), is_directory(false), children()
{
  fs::path p((const char *)this->path);

  if (!fs::exists(p))
    return;

  if (fs::is_regular_file(p))
  {
    is_directory = false;
  }
  else
  {
    is_directory = true;

    if (this->path[this->path.last()] == Platform::PathSeparator)
    {
      if (this->path.length() > 1)
        this->path = this->path.substring(0, this->path.last());
    }

    DynamicArray<String> children_tmp;
    for (const auto& entry : fs::directory_iterator(p))
    {
      children_tmp.add(entry.path().string());
    }
    children = children_tmp.toArray();
  }
}

Path::Path(const Path& base, const String relative) : Path(base.path + (const String)Platform::PathSeparator + relative)
{
}

Path::Path(const Path& other) : path(other.path), is_directory(other.is_directory), children(other.children) {}

const char * Path::get() const
{
  return path;
}

const bool Path::exists() const
{
  return fs::exists((const char *)this->path);
}

const bool Path::is_dir() const
{
  return is_directory;
}

const Array<String>& Path::get_children() const
{
  return children;
}

Path::operator const String() const
{
  return path;
}

Path::operator const char *() const
{
  return path;
}

File::File(const Path path, const IO_MODE mode) : size(0)
{
  const char * mode_str = nullptr;
  switch (mode)
  {
    case IO_MODE::READ_ONLY:
      mode_str = "rb";
      break;
    case IO_MODE::WRITE_ONLY:
      mode_str = "wb";
      break;
    case IO_MODE::READ_WRITE:
      mode_str = "r+b";
      break;
    default:
      throw std::runtime_error("Invalid IO_MODE"); // ToDo: better error handling
  }

  file = fopen(path, mode_str);
  if (file == nullptr)
    throw std::runtime_error("Failed to open file"); // ToDo: better error handling

  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);

  this->mode = mode;
}

File::~File()
{
  if (file)
    fclose(file);
}

__async_member_impl__(const Array<uint8_t>, File, read, const size_t size)
{
  if (file == nullptr)
    throw std::runtime_error("File not opened"); // ToDo: better error handling

  if (mode == IO_MODE::WRITE_ONLY)
    throw std::runtime_error("File not opened in read mode"); // ToDo: better error handling

  const size_t read_size = (size == 0 || size > this->size) ? this->size : size;

  Array<uint8_t> buffer(read_size);
  fread(buffer, sizeof(uint8_t), this->size, file);

  return buffer;
}

__async_member_impl__(const size_t, File, write, const Array<uint8_t>& data)
{
  if (file == nullptr)
    throw std::runtime_error("File not opened"); // ToDo: better error handling

  if (mode == IO_MODE::READ_ONLY)
    throw std::runtime_error("File not opened in write mode"); // ToDo: better error handling

  const size_t written = fwrite(data, sizeof(uint8_t), data.getSize(), file);
  fflush(file);

  return written;
}

__async_member_impl__(const Array<uint8_t>, File, read_from, const size_t offset, const size_t size)
{
  if (file == nullptr)
    throw std::runtime_error("File not opened"); // ToDo: better error handling

  if (mode == IO_MODE::WRITE_ONLY)
    throw std::runtime_error("File not opened in read mode"); // ToDo: better error handling

  if (offset >= this->size)
    throw std::out_of_range("Offset is beyond file size"); // ToDo: better error handling

  const size_t read_size = (size == 0 || offset + size > this->size) ? (this->size - offset) : size;

  fseek(file, offset, SEEK_SET);

  Array<uint8_t> buffer(read_size);
  fread(buffer, sizeof(uint8_t), read_size, file);

  return buffer;
}

__async_member_impl__(const size_t, File, write_from, const size_t offset, const Array<uint8_t>& data)
{
  if (file == nullptr)
    throw std::runtime_error("File not opened"); // ToDo: better error handling

  if (mode == IO_MODE::READ_ONLY)
    throw std::runtime_error("File not opened in write mode"); // ToDo: better error handling

  if (offset >= this->size)
    throw std::out_of_range("Offset is beyond file size"); // ToDo: better error handling

  fseek(file, offset, SEEK_SET);

  const size_t written = fwrite(data, sizeof(uint8_t), data.getSize(), file);
  fflush(file);

  return written;
}

FileSystem::FileSystem() : path(".") {}

FileSystem::FileSystem(const Path pwd) : path(pwd) {}

String FileSystem::pwd() const
{
  return path;
}

const File FileSystem::open(const Path path, const IO_MODE mode) const
{
  return File(path, mode);
}

const File FileSystem::create(const Path path) const
{
  return File(path, IO_MODE::READ_WRITE);
}

const Path FileSystem::mkdir(const Path path) const
{
  fs::path p((std::string)path);

  if (fs::exists(p))
    throw std::invalid_argument("Directory '" + (std::string)((String)path) +
                                "' already exists!"); // ToDo: better error handling

  if (!fs::create_directories(p))
    throw std::runtime_error("Failed to create directory!"); // ToDo: better error handling

  return path;
}

const Path FileSystem::cd(const Path relative)
{
  Path new_path = Path(this->path, relative);
  this->path    = new_path;
  return new_path;
}