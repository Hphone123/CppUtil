#pragma once

#include <stdexcept>
#include <string>

struct not_found : public std::logic_error
{
  not_found(std::string message) : logic_error(message){};
};