#pragma once

#include <initializer_list>
#include <utility> // for std::pair

#include "Array.hpp"
#include "Exception.hpp"

namespace CppUtil
{

template <typename T, typename U> struct Pair
{
  T t;
  U u;

  Pair(T t, U u) : t(t), u(u){};
};

/**
  * Maps item U to unique key T
  */
template <typename T, typename U> class Map
{
private:
  DynamicArray<T> t;
  DynamicArray<U> u;

public:
  Map() : t(), u() {}

  Map(std::initializer_list<std::pair<const T, U>> list)
  {
    for (const auto& item : list)
    {
      t.add(item.first);
      u.add(item.second);
    }
  }

  const size_t size() const
  {
    return t.getCount();
  }

  const Pair<T, U> idx(const size_t idx) const
  {
    if (idx > this->size())
      throw "Cannot acces map out of bounds!";
    return Pair(this->t[idx], this->u[idx]);
  }

  const bool has(const T& key) const
  {
    for (int i = 0; i < t.getCount(); i++)
      if (t[i] == key)
        return true;
    return false;
  }

  /**
    * Get item U correspinding to `key`
    * 
    * Adds `key` and a new item U if key is not found
    */
  U& operator[](const T& key)
  {
    for (size_t i = 0; i < t.getCount(); i++)
    {
      if (key == t[i])
      {
        return u[i];
      }
    }

    t.add(key);
    u.add(U());
    return u[u.getCount() - 1];
  }

  /**
    * Get item U corresponding to `key`
    * 
    * @throws not_found
    */
  U& tryGetItem(const T& key)
  {
    for (size_t i = 0; i < t.getCount(); i++)
    {
      if (key == t[i])
      {
        return u[i];
      }
    }
    throw not_found("Cannot get item of nonexistant key '" + key + "'!");
  }

  /**
    * Set item U correspinding to `key` to `item`
    * 
    * @throws not_found
    */
  void trySetItem(const T& key, const U& item)
  {
    for (size_t i = 0; i < t.getCount(); i++)
    {
      if (key == t[i])
      {
        u[i] = item;
        return;
      }
    }
    throw not_found("Cannot set item of nonexistant key '" + key + "'!");
  }

  /**
    * Remove item U correspinding to `key`
    * 
    * @throws not_found
    */
  void tryRemoveItem(const T& key)
  {
    for (size_t i = 0; i < t.getCount(); i++)
    {
      if (key == t[i])
      {
        t.remove(i);
        u.remove(i);
        return;
      }
    }
    throw not_found("Cannot remove item of nonexistant key '" + key + "'!");
  }
};

template <typename T, typename U> const bool operator==(const Map<T, U> a, const Map<T, U> b)
{
  if (a.size() != b.size())
    return false;
  for (int i = 0; i < a.size(); i++)
  {
    if (a.idx(i).t != b.idx(i).t || a.idx(i).u != b.idx(i).u)
      return false;
  }
  return true;
}

template <typename T, typename U> const bool operator!=(const Map<T, U> a, const Map<T, U> b)
{
  return !(a == b);
}

} // namespace CppUtil