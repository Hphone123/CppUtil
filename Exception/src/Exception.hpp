#pragma once

#include <stdexcept>
#include <string>

namespace CppUtil
{
struct not_found : public std::logic_error
{
  not_found(std::string message) : logic_error(message){};
};
} // namespace CppUtil