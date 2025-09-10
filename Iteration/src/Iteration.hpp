//  Iteration:
//  Required functions:
//  it<T> it::operator++, T it::operator*, it::operator!=(const it&)

#pragma once

#include <type_traits>
#include <utility>

// Common alias for the result of ++t
template <typename T> using preinc_result_t = decltype(++std::declval<T&>());

// has_valid_preincrement: ++t is valid and returns T&
template <typename, typename = void> struct has_valid_preincrement : std::false_type
{
};

template <typename T>
struct has_valid_preincrement<T, std::void_t<preinc_result_t<T>>>
    : std::bool_constant<std::is_same_v<preinc_result_t<T>, T&>>
{
};

template <typename T> inline constexpr bool has_valid_preincrement_v = has_valid_preincrement<T>::value;

// has_valid_pointer_preincrement: T is a pointer AND ++t is valid and returns T&
template <typename, typename = void> struct has_valid_pointer_preincrement : std::false_type
{
};

template <typename T>
struct has_valid_pointer_preincrement<T, std::void_t<preinc_result_t<T>>>
    : std::bool_constant<std::is_pointer_v<T> && std::is_same_v<preinc_result_t<T>, T&>>
{
};

template <typename T> inline constexpr bool has_valid_pointer_preincrement_v = has_valid_pointer_preincrement<T>::value;
template <typename T> class Iterator
{
private:
  T val;

public:
  Iterator(T obj) : val(obj) {}

  T operator*() const
  {
    return this->val;
  }

  Iterator& operator++()
  {
    if constexpr (std::is_pointer_v<T>)
    {
      static_assert(has_valid_pointer_preincrement_v<decltype(this->val)>,
                    "Type T must support operator++ and must result in T*!");
      this->val = ++(*this->val);
    }
    else
    {
      static_assert(has_valid_preincrement_v<decltype(this->val)>,
                    "Type T must support operator++ and must result in T!");
      ++this->val;
    }
    return *this;
  }

  bool operator!=(const Iterator& other) const
  {
    return this->val != other.val;
  }
};

template <typename T> class Iterable
{
public:
  virtual Iterator<T> begin() const = 0;
  virtual Iterator<T> end() const   = 0;
};

template <typename T> class IterationElement
{
public:
  virtual T operator++() = 0;
};