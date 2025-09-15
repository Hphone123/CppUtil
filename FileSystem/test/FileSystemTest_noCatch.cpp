#include <filesystem>
#include <string>

#include "FileSystem.hpp"

using namespace CppUtil;

std::string pwd = std::filesystem::current_path().lexically_normal().string();

int main()
{
  try
  {
    std::filesystem::remove_all("./test_dir");
  }
  catch (...)
  {
  };

  FileSystem fs((Path)pwd);

  std::cout << (std::string)fs.pwd() << std::endl;

  auto d = fs.mkdir(Path("test_dir"));

  std::cout << (std::string)d << std::endl;
  std::cout << d.is_dir() << std::endl;
  std::cout << d.get_children().getSize() << std::endl;

  // Cleanup
}