#pragma once

#include <initializer_list>
#include <utility> // for std::pair

#include "Array.hpp"
#include "Exception.hpp"

/**
 * Maps item U to unique key T
 */
template<typename T, typename U> class Map
{
private:
  DynamicArray<T> t;
  DynamicArray<U> u;

public:
  Map() : t(), u() {}

  Map(std::initializer_list<std::pair<const T, U>> list) {
    for (const auto& item : list) {
      t.add(item.first);
      u.add(item.second);
    }
  } 

  /**
   * Get item U correspinding to `key`
   * 
   * Adds `key` and a new item U if key is not found
   */
  U& operator [](const T& key)
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