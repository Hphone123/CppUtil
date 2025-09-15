#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

#include "Exception.hpp"

// Hope a billion is enough elements you sick people
#define ARRAY_MAX_SIZE 1'000'000'000

namespace CppUtil
{
template <typename T> class Array
{
protected:
  T *    arr;
  size_t size;

public:
  Array<T>() : size(0)
  {
    this->arr = new T[0]();
  }

  Array<T>(size_t size)
  {
    if (size > ARRAY_MAX_SIZE)
    {
      throw std::length_error("Size " + std::to_string(size) + " is not a valid array size!");
    }

    this->arr  = new T[size]();
    this->size = size;
  }

  Array<T>(const T * buf, size_t size)
  {
    if (size > ARRAY_MAX_SIZE)
    {
      throw std::length_error("Size" + std::to_string(size) + " is not a valid array size!");
    }

    if (buf == nullptr)
    {
      throw std::invalid_argument("Buffer must not be a nullpointer!");
    }

    this->arr = new T[size]();
    for (size_t i = 0; i < size; i++)
    {
      this->arr[i] = buf[i];
    }
    this->size = size;
  }

  Array<T>(const Array<T>& other)
  {
    this->arr = new T[other.size]();

    for (size_t i = 0; i < other.size; i++)
    {
      this->arr[i] = other.arr[i];
    }

    this->size = other.size;
  }

  template <typename U, typename = std::enable_if_t<std::is_constructible<T, U>::value>>
  Array<T>(const U * buf, size_t size)
  {
    if (size > ARRAY_MAX_SIZE)
    {
      throw std::length_error("Size" + std::to_string(size) + " is not a valid array size!");
    }
    this->arr = new T[size]();

    for (size_t i = 0; i < size; i++)
    {
      arr[i] = U(buf[i]);
    }
    this->size = size;
  }

  Array<T>(const std::initializer_list<T> init)
  {
    if (init.size() > ARRAY_MAX_SIZE)
    {
      throw std::length_error("Size" + std::to_string(size) + " is not a valid array size!");
    }
    this->size = init.size();
    this->arr  = new T[this->size]();

    size_t i = 0;
    for (auto x : init)
    {
      this->arr[i++] = x;
    }
  }

  Array<T>& operator=(const Array<T>& other)
  {
    if (&other != this)
    {
      delete[] this->arr;

      T * tmp = new T[other.size]();
      for (size_t i = 0; i < other.size; i++)
      {
        tmp[i] = other.arr[i];
      }

      this->arr  = tmp;
      this->size = other.size;
    }

    return *this;
  }

  operator T *() const
  {
    return this->arr;
  }

  ~Array<T>()
  {
    delete[] this->arr;
  }

  virtual T& operator[](size_t idx) final
  {
    if (idx < 0)
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " is not a valid array index!");
    }

    if (this->size > idx)
    {
      return this->arr[idx];
    }
    else
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " out of bounds for array of size " +
                              std::to_string(this->size) + "!");
    }
  }

  virtual T operator[](size_t idx) const final
  {
    if (idx < 0)
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " is not a valid array index!");
    }

    if (this->size > idx)
    {
      return this->arr[idx];
    }
    else
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " out of bounds for array of size " +
                              std::to_string(this->size) + "!");
    }
  }

  virtual size_t getSize() const final
  {
    return this->size;
  }

  virtual bool isEmpty() const final
  {
    return (this->size == 0);
  }

  Array<size_t> find(const T& el) const;

  bool has(const T& el) const
  {
    return (this->find(el).getSize() > 0);
  }

  bool hasAny(const Array<T>& el) const
  {
    for (int i = 0; i < el.getSize(); i++)
    {
      if (this->has(el[i]))
        return true;
    }
    return false;
  }

  bool hasAll(const Array<T>& el) const
  {
    for (int i = 0; i < el.getSize(); i++)
    {
      if (!this->has(el[i]))
        return false;
    }
    return true;
  }

  void shiftLeft(const size_t num)
  {
    for (int i = num; i < this->size; i++)
    {
      this->operator[](i - num) = this->operator[](i);
    }
  }

  void shiftRight(const size_t num)
  {
    for (int i = this->size - num; i > 0; i--)
    {
      this->operator[](i + num) = this->operator[](i);
    }
  }

  //ToDo: UnitTest
  virtual bool operator==(const Array<T>& other) const final
  {
    if (this->size != other.size)
      return false;

    for (size_t i = 0; i < this->size; i++)
      if (this->operator[](i) != other[i])
        return false;

    return true;
  }

  virtual bool operator!=(const Array<T>& other) const final
  {
    return !(this->operator==(other));
  }

  template <typename func> void foreach (const func&& f)
  {
    for (size_t i = 0; i < this->getSize(); i++)
    {
      if constexpr (std::is_invocable_v<func, T&>)
      {
        f(this->operator[](i));
      }
      else if constexpr (std::is_invocable_v<func, T&, size_t>)
      {
        f(this->operator[](i), i);
      }
      else
      {
        static_assert(std::is_invocable_v<func, T&> || std::is_invocable_v<func, T&, size_t>,
                      "Function must have signature 'void(T&)' or 'void(T&, size_t)'!");
      }
    }
  }

  template <typename func> void foreach (const func&& f) const
  {
    for (size_t i = 0; i < this->getSize(); i++)
    {
      if constexpr (std::is_invocable_v<func, const T&>)
      {
        f(this->operator[](i));
      }
      else if constexpr (std::is_invocable_v<func, const T&, size_t>)
      {
        f(this->operator[](i), i);
      }
      else
      {
        static_assert(std::is_invocable_v<func, const T&> || std::is_invocable_v<func, const T&, size_t>,
                      "Function must have signature 'void(const T&)' or 'void(const T&, size_t)'!");
      }
    }
  }

  template <typename func> bool all(const func&& f)
  {
    for (size_t i = 0; i < this->getSize(); i++)
    {
      if constexpr (std::is_invocable_r_v<bool, func, T&>)
      {
        if (!f(this->operator[](i)))
          return false;
      }
      else if constexpr (std::is_invocable_r_v<bool, func, T&, size_t>)
      {
        if (!f(this->operator[](i), i))
          return false;
      }
      else
      {
        static_assert(std::is_invocable_r_v<bool, func, T&> || std::is_invocable_r_v<bool, func, T&, size_t>,
                      "Function must have signature 'bool(T&)' or 'bool(T&, size_t)'!");
      }
    }
    return true;
  }

  template <typename func> bool all(const func&& f) const
  {
    for (size_t i = 0; i < this->getSize(); i++)
    {
      if constexpr (std::is_invocable_r_v<bool, func, const T&>)
      {
        if (!f(this->operator[](i)))
          return false;
      }
      else if constexpr (std::is_invocable_r_v<bool, func, const T&, size_t>)
      {
        if (!f(this->operator[](i), i))
          return false;
      }
      else
      {
        static_assert(std::is_invocable_r_v<bool, func, const T&> ||
                        std::is_invocable_r_v<bool, func, const T&, size_t>,
                      "Function must have signature 'bool(T&)' or 'bool(T&, size_t)'!");
      }
    }
    return true;
  }

  template <typename func> bool any(const func&& f)
  {
    for (size_t i = 0; i < this->getSize(); i++)
    {
      if constexpr (std::is_invocable_r_v<bool, func, T&>)
      {
        if (f(this->operator[](i)))
          return true;
      }
      else if constexpr (std::is_invocable_r_v<bool, func, T&, size_t>)
      {
        if (f(this->operator[](i), i))
          return true;
      }
      else
      {
        static_assert(std::is_invocable_r_v<bool, func, T&> || std::is_invocable_r_v<bool, func, T&, size_t>,
                      "Function must have signature 'bool(T&)' or 'bool(T&, size_t)'!");
      }
    }
    return false;
  }

  template <typename func> bool any(const func&& f) const
  {
    for (size_t i = 0; i < this->getSize(); i++)
    {
      if constexpr (std::is_invocable_r_v<bool, func, const T&>)
      {
        if (f(this->operator[](i)))
          return true;
      }
      else if constexpr (std::is_invocable_r_v<bool, func, const T&, size_t>)
      {
        if (f(this->operator[](i), i))
          return true;
      }
      else
      {
        static_assert(std::is_invocable_r_v<bool, func, const T&> ||
                        std::is_invocable_r_v<bool, func, const T&, size_t>,
                      "Function must have signature 'bool(const T&)' or 'bool(const T&, size_t)'!");
      }
    }
    return false;
  }
};

template <typename T> class ResizableArray : public Array<T>
{
public:
  ResizableArray(size_t size) : Array<T>(size){};

  ResizableArray(const T * arr, size_t size) : Array<T>(arr, size){};

  /**
     * Resizes the array.
     * 
     * @param newSize The size the array should be given.
     * @throws `length_error` for invalid sizes
     * @throws `length_error` when newSize is smaller than current size
     * @note Use `resizeForce()` to force a size down.
     */
  void resize(size_t newSize)
  {
    if (newSize > ARRAY_MAX_SIZE)
    {
      throw std::length_error("Size " + std::to_string(newSize) + " is not a valid array size!");
    }

    if (newSize > this->size)
    {
      T * tmp = new T[newSize]();
      for (size_t i = 0; i < this->size; i++)
      {
        tmp[i] = this->arr[i];
      }
      delete[] this->arr;
      this->arr = tmp;

      this->size = newSize;
    }
    else if (newSize == this->size)
    {
      return;
    }
    else
    {
      throw std::length_error("Cannot resize from " + std::to_string(this->size) + " to " + std::to_string(newSize) +
                              "!");
    }
  }
  /**
     * Force a resize of the array.
     * 
     * @param newSize The new size
     * 
     * @throws `length_error` for invalid array sizes.
     * 
     * @warning Does not throw when sizing down the array!
     * @warning Use `resize()` to prevent data loss.
     */
  void resizeForce(size_t newSize)
  {
    if (newSize > ARRAY_MAX_SIZE)
    {
      throw std::length_error("Size " + std::to_string(newSize) + " is not a valid array size!");
    }

    if (newSize >= this->size)
    {
      return this->resize(newSize);
    }
    else
    {
      T * tmp = new T[newSize]();
      for (size_t i = 0; i < newSize; i++)
      {
        tmp[i] = this->arr[i];
      }
      delete[] this->arr;

      this->arr  = tmp;
      this->size = newSize;
    }
  }
};

template <typename T> class DynamicArray
{
protected:
  size_t count        = 0;
  size_t resizeFactor = 2;

  ResizableArray<T> arr;

public:
  DynamicArray<T>() : arr(2)
  {
    this->count        = 0;
    this->resizeFactor = 2;
  }

  DynamicArray<T>(size_t cap) : arr(cap)
  {
    if (cap > ARRAY_MAX_SIZE)
    {
      throw std::length_error("Size " + std::to_string(cap) + " is not a valid array size!");
    }

    this->count        = 0;
    this->resizeFactor = 2;
  }

  DynamicArray<T>(size_t cap, size_t resizeFactor) : arr(cap)
  {
    if (cap > ARRAY_MAX_SIZE)
    {
      throw std::length_error("Size " + std::to_string(cap) + " is not a valid array size!");
    }

    if (resizeFactor < 2)
    {
      throw std::invalid_argument("Dynamic scaling factor must be bigger than 1!");
    }

    this->count        = 0;
    this->resizeFactor = resizeFactor;
  }

  T& operator[](size_t idx)
  {
    if (idx > ARRAY_MAX_SIZE)
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " is not a valid array index!");
    }

    if (idx >= this->count)
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " out of bounds for dynamic array with " +
                              std::to_string(this->count) + " elements!");
    }

    return this->arr[idx];
  }

  T operator[](size_t idx) const
  {
    if (idx > ARRAY_MAX_SIZE)
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " is not a valid array index!");
    }

    if (idx >= this->count)
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " out of bounds for dynamic array with " +
                              std::to_string(this->count) + " elements!");
    }

    return this->arr[idx];
  }

  size_t getCount() const
  {
    return this->count;
  }

  virtual size_t getCap() const final
  {
    return this->arr.getSize();
  }

  size_t getResizeFactor() const
  {
    return this->resizeFactor;
  }

  virtual void add(T item) final
  {
    if (this->count >= this->getCap())
    {
      this->arr.resize(this->count * this->resizeFactor);
    }

    this->arr[count++] = item;
  }

  virtual void add(T item, size_t idx) final
  {
    if (idx > ARRAY_MAX_SIZE)
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " is not a valid array index!");
    }

    if (idx > this->count)
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " out of bounds for dynamic array with " +
                              std::to_string(this->count) + " elements!");
    }

    if (this->count >= this->getCap())
    {
      this->arr.resize(this->getCap() * this->resizeFactor);
    }

    for (size_t i = this->getCap() - 1; i > idx; i--)
    {
      this->arr[i] = this->arr[i - 1];
    }

    this->arr[idx] = item;
    this->count++;
  }
  /**
     * Remove the last element (the one with the biggest index) from the array
     * 
     * @throws `length_error` if array is already empty.
     */
  virtual void remove() final
  {
    if (count > 0)
    {
      --count;
      if (count * resizeFactor <= this->getCap() && this->getCap() >= this->resizeFactor)
      {
        this->arr.resizeForce(this->getCap() / resizeFactor);
      }
    }
    else
    {
      throw std::length_error("Cannot remove element from empty dynamic array!");
    }
  }

  /**
     * Remove the element at index @param idx and shift all elements after @param idx to the left so all elements are in a row.ADJ_OFFSET_SINGLESHOT
     * 
     * @param idx The index whoose element is to be removed
     * 
     * @throw `out_of_range` if 
     */
  virtual void remove(size_t idx) final
  {
    if (idx > ARRAY_MAX_SIZE)
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " is not a valid array index!");
    }

    if (this->count == 0)
    {
      throw std::length_error("Cannot remove element from empty dynamic array!");
    }

    if (idx >= this->count)
    {
      throw std::out_of_range("Index " + std::to_string(idx) + " out of bounds for dynamic array with " +
                              std::to_string(this->count) + " elements!");
    }

    this->count--;

    for (size_t i = idx; i < this->count; i++)
    {
      this->arr[i] = this->arr[i + 1];
    }

    if (this->count * this->resizeFactor <= this->getCap() && this->getCap() >= this->resizeFactor)
    {
      this->arr.resizeForce(this->getCap() / this->resizeFactor);
    }
  }

  // ToDo: UnitTest all below!

  template <typename func> void foreach (size_t startIdx, func && f)
  {
    if (this->getCount() == 0)
      return;

    if (startIdx >= this->getCount())
      throw std::invalid_argument("Start index must be inside array bounds!");

    for (size_t i = startIdx; i < this->getCount(); i++)
    {
      if constexpr (std::is_invocable_v<func, T&>)
        f(this->operator[](i));
      else if constexpr (std::is_invocable_v<func, T&, size_t>)
        f(this->operator[](i), i);
      else
        static_assert(std::is_invocable_v<func, T&> || std::is_invocable_v<func, T&, size_t>,
                      "Function must have signature 'void(T&)' or 'void(T&, size_t)'!");
    }
  }

  template <typename func> void foreach (func&& f)
  {
    this->foreach (0, f);
  }

  template <typename func> bool any(func&& f)
  {
    for (size_t i = 0; i < this->getCount(); i++)
    {
      if constexpr (std::is_invocable_r_v<bool, func, T&>)
      {
        if (f(this->operator[](i)))
          return true;
      }
      else if constexpr (std::is_invocable_r_v<bool, func, T&, size_t>)
      {
        if (f(this->operator[](i), i))
          return true;
      }
      else
      {
        static_assert(std::is_invocable_r_v<bool, func, T&> || std::is_invocable_r_v<bool, func, T&, size_t>,
                      "Function must have signature 'bool(T&)' or 'bool(T&, size_t)'!");
      }
    }
    return false;
  }

  template <typename func> bool all(func&& f)
  {
    for (size_t i = 0; i < this->getCount(); i++)
    {
      if constexpr (std::is_invocable_r_v<bool, func, T&>)
      {
        if (!f(this->operator[](i)))
          return false;
      }
      else if constexpr (std::is_invocable_r_v<bool, func, T&, size_t>)
      {
        if (!f(this->operator[](i), i))
          return false;
      }
      else
      {
        static_assert(std::is_invocable_r_v<bool, func, T&> || std::is_invocable_r_v<bool, func, T&, size_t>,
                      "Function must have signature 'bool(T&)' or 'bool(T&, size_t)'!");
      }
    }
    return true;
  }

  Array<T> toArray()
  {
    Array<T> res(this->getCount());
    this->foreach ([&](T& x, size_t idx) { res[idx] = x; });
    return res;
  }
};

/******************************************************
   * Function that require classes further down in the file
   ******************************************************/

//ToDo: Unit Test
template <typename T> Array<size_t> Array<T>::find(const T& el) const
{
  DynamicArray<size_t> res;
  for (size_t i = 0; i < this->size; i++)
  {
    if (this->operator[](i) == el)
    {
      res.add(i);
    }
  }

  return res.toArray();
}

/******************************************************
   * Static / Class-less functions
   ******************************************************/

template <typename T, typename func> static void foreach (Array<T> arr, func && f)
{
  for (size_t i = 0; i < arr.getSize(); i++)
  {
    f(arr[i]);
  }
}

template <typename T, typename func> static void foreach (DynamicArray<T> arr, func && f)
{
  for (size_t i = 0; i < arr.getCount(); i++)
  {
    f(arr[i]);
  }
}

}; // namespace CppUtil
namespace std
{
template <typename T> string to_string(const CppUtil::Array<T> arr)
{
  string res = "{";
  arr.foreach ([&](const T& el) { res += to_string(el) + " "; });
  res += "}";
  return res;
}

template <typename T> string to_string(const CppUtil::Array<T> arr, size_t n)
{
  string res = "{";
  arr.foreach (
    [&](const T& el, size_t i)
    {
      if (i < n)
        res += to_string(el) + " ";
    });
  res += "}";
  return res;
}

template <typename T> string to_string(const CppUtil::DynamicArray<T>& arr)
{
  string res = "{";
  arr.foreach ([&](const T& el) { res += to_string(el) + " "; });
  res += "}";
  return res;
}
} // namespace std