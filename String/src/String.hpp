#pragma once

#include <iostream>
#include <istream>
#include <stdexcept>
#include <string.h>

#include "Array.hpp"
#include "Exception.hpp"

namespace CppUtil
{
class String : public ResizableArray<char>
{
private:
  using ResizableArray<char>::resize;
  using ResizableArray<char>::resizeForce;

protected:
  String(size_t size) : ResizableArray<char>(size){};

public:
  String() : ResizableArray<char>(1){};

  String(const char * str) : ResizableArray<char>(str, strlen(str) + 1){};

  String(const char ch) : ResizableArray<char>(2)
  {
    this->operator[]((size_t)0) = ch;
    this->operator[](1)         = '\0';
  };

  String(std::string str) : ResizableArray<char>(str.c_str(), strlen(str.c_str()) + 1){};

  String operator+(const String& other) const
  {
    String res(this->size + other.size - 1);
    for (size_t i = 0; i < this->size - 1; i++)
    {
      res[i] = this->operator[](i);
    }
    for (size_t i = 0; i < other.size; i++)
    {
      res[i + this->size - 1] = other[i];
    }
    res[res.size - 1] = '\0';
    return res;
  }

  String& operator+=(const String& other)
  {
    size_t old_size = this->size;
    this->resize(this->size + other.size - 1);
    for (size_t i = 0; i < other.size; i++)
    {
      this->operator[](i + old_size - 1) = other[i];
    }
    this->operator[](this->size - 1) = '\0';
    return *this;
  }

  operator std::string() const
  {
    return std::string(this->arr);
  }

  operator const char *() const
  {
    return this->arr;
  }

  friend std::ostream& operator<<(std::ostream& os, const String& str)
  {
    os << static_cast<const char *>(str);
    return os;
  }

  const size_t last() const
  {
    return this->size - 2;
  }

  bool operator==(const String& other) const
  {
    if (this->size != other.getSize())
      return false;
    for (size_t i = 0; i < this->size - 1; i++)
    {
      if (this->operator[](i) != other[i])
        return false;
    }
    return true;
  }

  bool operator!=(const String& other) const
  {
    return !(this->operator==(other));
  }

  bool operator==(const char * other) const
  {
    if (strlen(other) + 1 != this->size)
      return false;

    for (size_t i = 0; i < this->size; i++)
    {
      if (this->operator[](i) != other[i])
        return false;
    }
    return true;
  }

  bool operator!=(const char * other) const
  {
    return !(this->operator==(other));
  }

  size_t length() const
  {
    return this->size - 1;
  }

  //ToDo: UnitTest
  void remove(size_t idx, size_t len)
  {
    if (idx < 0 || idx >= this->size)
      throw std::invalid_argument("Index must be within string bounds!");

    if (idx + len > this->length())
      throw std::invalid_argument("Removed element must not exceed string bounds!");

    if (len == 0)
      return;

    if (len < 0)
      throw std::invalid_argument("Length must be a positive number!");

    char * tmp    = new char[this->size - len];
    size_t offset = 0;
    for (size_t i = 0; i < this->size; i++)
    {
      if (i >= idx && i < idx + len)
      {
        offset++;
        continue;
      }

      tmp[i - offset] = this->operator[](i);
    }
    delete[] this->arr;
    this->arr  = tmp;
    this->size = this->size - len;
  }

  void insert(String obj, size_t idx)
  {
    size_t len    = obj.length();
    char * tmp    = new char[this->size + len];
    size_t offset = 0;

    for (size_t i = 0; i < this->size; i++)
    {
      while (i >= idx && offset < len)
      {
        tmp[i + offset] = obj[offset];
        offset++;
      }

      tmp[i + offset] = this->operator[](i);
    }

    delete[] this->arr;
    this->arr  = tmp;
    this->size = this->size + len;
  }

  Array<size_t> find(const String str)
  {
    DynamicArray<size_t> res;
    for (size_t i = 0; i < this->size; i++)
    {
      size_t j = 0;
      while (this->operator[](i + j) == str[j])
      {
        if (++j == str.size - 1)
        {
          res.add(i);
          break;
        }
      }
    }

    return res.toArray();
  }

  /** 
    * Replaces all occurences of `rem` with `rep`
    */
  void replace(String rem, String rep)
  {
    this->find(rem);
    auto idx = this->find(rem);

    for (size_t i = 0; i < idx.getSize(); i++)
    {
      this->remove(idx[i], rem.size - 1);
      this->insert(rep, idx[i]);
    }
  }

  String substring(size_t idx, size_t len) const
  {
    String res(len + 1);
    for (size_t i = 0; i < len; i++)
    {
      res[i] = this->operator[](i + idx);
    }
    return res;
  }

  String substring(size_t idx) const
  {
    return this->substring(idx, this->length() - idx);
  }

  void trim(char character)
  {
    for (size_t i = 0; i < this->length(); i++)
    {
      if (this->operator[](i) != character)
      {
        if (i > 0)
          this->remove(0, i);
        break;
      }
    }

    for (size_t i = this->length(); i >= 0; i--)
    {
      if (this->operator[](i) != character)
      {
        if (i < this->length())
          this->remove(i + 1, this->length() - i);
        break;
      }
    }
  }

  // ToDo: UnitTest
  DynamicArray<String> splitAt(char character) const
  {
    DynamicArray<String> res;
    for (size_t i = 0; i < this->length(); i++)
    {
      if (this->operator[](i) == character)
        continue;

      size_t startIdx = i;

      while (1)
      {
        if (i >= this->length())
        {
          res.add(this->substring(startIdx, i - startIdx));
          break;
        }

        if (this->operator[](i) == character)
        {
          res.add(this->substring(startIdx, i - startIdx));
          break;
        }
        i++;
      }
    }
    return res;
  }

  static bool charIsAlpha(char c)
  {
    if (c >= 0x30 && c <= 0x39)
      return true;
    if (c >= 0x41 && c <= 0x5A)
      return true;
    if (c >= 0x61 && c <= 0x7A)
      return true;

    return false;
  }

  bool isAlpha()
  {
    for (size_t i = 0; i < this->length(); i++)
    {
      if (!charIsAlpha(this->operator[](i)))
        return false;
    }
    return true;
  }

  bool isAlphaOr(String characters)
  {
    for (size_t i = 0; i < this->length(); i++)
    {
      char c = this->operator[](i);
      if (!charIsAlpha(c))
      {
        bool res = false;
        for (size_t j = 0; j < characters.length(); j++)
        {
          if (c == characters[j])
            res = true;
        }
        if (!res)
          return false;
      }
    }
    return true;
  }

  template <typename func> void foreach (size_t startIdx, func && f)
  {
    for (size_t i = startIdx; i < this->length(); i++)
    {
      if constexpr (std::is_invocable_v<func, char>)
        f(this->operator[](i));
      else if constexpr (std::is_invocable_v<func, char, size_t>)
        f(this->operator[](i), i);
      else
        static_assert(std::is_invocable_v<func, char> || std::is_invocable_v<func, char, size_t>,
                      "Function must have signature 'void(char)' or 'void(char, size_t)'!");
    }
  }
};
} // namespace CppUtil