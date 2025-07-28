#pragma once

#include <stdexcept>
#include <functional>
#include "Exception.hpp"

// Hope a billion is enough elements you sick people
#define ARRAY_MAX_SIZE 1'000'000'000


using namespace std;

template <typename T> class Array
{
protected:
  T * arr;
  size_t size;

public:
  Array<T>()
  {
    this->arr = new T [0]();
  }

  Array<T>(size_t size)
  {
    if (size > ARRAY_MAX_SIZE)
    {
      throw length_error("Size " + to_string(size) + " is not a valid array size!");
    }

    this->arr  = new T[size]();
    this->size = size;
  }

  Array<T>(const T * buf, size_t size)
  {
    if (size > ARRAY_MAX_SIZE)
    {
      throw length_error("Size" + to_string(size) + " is not a valid array size!");
    }

    if (buf == nullptr)
    {
      throw invalid_argument("Buffer must not be a nullpointer!");
    }

    this->arr  = new T[size]();
    for (size_t i = 0; i < size; i++)
    {
      this->arr[i] = buf[i];
    }
    this->size = size;
  }

  Array<T> (const Array<T>& other)
  {
    this->arr = new T[other.size]();

    for (size_t i = 0; i < other.size; i++)
    {
      this->arr[i] = other.arr[i];
    }
    
    this->size = other.size;
  }
  
  Array<T>& operator = (const Array<T>& other)
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

  operator T * () const
  {
    return this->arr;
  }

  ~Array<T>()
  {
    delete[] this->arr;
  }

  T& operator[] (size_t idx)
  {
    if (idx < 0)
    {
      throw out_of_range("Index " + to_string(idx) + " is not a valid array index!");
    }

    if (this->size > idx)
    {
      return this->arr[idx];
    }
    else
    {
      throw out_of_range("Index " + to_string(idx) + " out of bounds for array of size " + to_string(this->size) + "!");
    }
  }
  
  T operator[] (size_t idx) const
  {
    if (idx < 0)
    {
      throw out_of_range("Index " + to_string(idx) + " is not a valid array index!");
    }

    if (this->size > idx)
    {
      return this->arr[idx];
    }
    else
    {
      throw out_of_range("Index " + to_string(idx) + " out of bounds for array of size " + to_string(this->size) + "!");
    }
  }

  size_t getSize() const
  {
    return this->size;
  }

  bool isEmpty() const
  {
    return (this->size == 0);
  }

  Array<size_t> find(T el) const;

  //ToDo: UnitTest
  bool operator== (const Array<T>& other) const
  {
    if (this->size != other.size)
      return false;

    for (size_t i = 0; i < this->size; i++)
      if (this->operator[](i) != other[i]) 
        return false;

    return true;
  }

  bool operator!=(const Array<T>& other) const
  {
    return !(this->operator==(other));
  }

  template<typename func>
  void foreach(const func&& f)
  {
    for (int i = 0; i < this->getSize(); i++)
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
        static_assert(std::is_invocable_v<func, T&> || 
                      std::is_invocable_v<func, T&, size_t>, 
                        "Function must have signature 'void(T&)' or 'void(T&, size_t)'!");
      }
    }
  }
  
  template<typename func>
  bool all(const func&& f)
  {
    for (int i = 0; i < this->getSize(); i++)
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
        static_assert(std::is_invocable_r_v<bool, func, T&> || 
                      std::is_invocable_r_v<bool, func, T&, size_t>, 
                        "Function must have signature 'bool(T&)' or 'bool(T&, size_t)'!");
      }
    }
    return true;
  }
  
  template<typename func>
  bool any(const func&& f)
  {
    for (int i = 0; i < this->getSize(); i++)
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
        static_assert(std::is_invocable_r_v<bool, func, T&> || 
                      std::is_invocable_r_v<bool, func, T&, size_t>, 
                        "Function must have signature 'bool(T&)' or 'bool(T&, size_t)'!");
      }
    }
    return false;
  }
};

template <typename T> class ResizableArray : public Array<T>
{
public:

  ResizableArray(size_t size)                : Array<T>(size)      {};

  ResizableArray(const T* arr, size_t size)  : Array<T>(arr, size) {};

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
    if (newSize < 1)
    {
      throw length_error("Size " + to_string(newSize) + " is not a valid array size!");
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
      throw length_error("Cannot resize from " + to_string(this->size) + " to " + to_string(newSize) + "!");
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
  void resizeForce (size_t newSize)
  {
    if (newSize < 1)
    {
      throw length_error("Size " + to_string(newSize) + " is not a valid array size!");
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

template <typename T> class DynamicArray : public ResizableArray<T>
{
protected:
  size_t idx = 0;
  size_t resizeFactor = 2;

  using ResizableArray<T>::resize;
  using ResizableArray<T>::resizeForce;

public:
  DynamicArray<T>() : ResizableArray<T>(2)
  {
    this->idx = 0;
    this->resizeFactor = 2;
  }

  DynamicArray<T>(size_t size) :  ResizableArray<T>(size)
  {
    if (size < 1)
    {
      throw invalid_argument("Size must be bigger than 0!");
    }

    this->idx = 0;
    this->resizeFactor = 2;
  }
  
  DynamicArray<T>(size_t size, size_t resizeFactor) :  ResizableArray<T>(size)
  {
    if (size < 1)
    {
      throw invalid_argument("Size must be bigger than 0!");
    }

    if (resizeFactor < 2)
    {
      throw invalid_argument("Dynamic scaling factor must be bigger than 1!");
    }
    this->idx = 0;
    this->resizeFactor = resizeFactor;
  }
  
  T& operator[] (size_t idx)
  {
    if (idx < 0)
    {
      throw out_of_range("Index " + to_string(idx) + " is not a valid array index!");
    }
    
    if (idx >= this->idx)
    {
      throw out_of_range("Index " + to_string(idx) + " out of bounds for dynamic array with " + to_string(this->idx) + " elements!");
    }

    return this->arr[idx];
  }
  
  T operator[] (size_t idx) const = delete;
  
  void add(T item)
  {
    if (this->idx >= this->size)
    {
      this->resize(this->idx * this->resizeFactor);
    }

    this->arr[idx++] = item; 
    
  }

  size_t getCount()
  {
    return this->idx;
  }
  
  void add(T item, size_t idx)
  {
    if (idx < 0)
    {
      throw out_of_range("Index " + to_string(idx) + " is not a valid array index!");
    }
    
    if (idx > this->idx)
    {
      throw out_of_range("Index " + to_string(idx) + " out of bounds for dynamic array with " + to_string(this->idx) + " elements!");
    }
    
    if (this->idx >= this->size)
    {
      this->resize(this->size * this->resizeFactor);
    }
    
    for (size_t i = this->size - 1; i > idx; i--)
    {
      this->arr[i] = this->arr[i-1];
    }
    
    this->arr[idx] = item;
    this->idx++;
  }
  /**
   * Remove the last element (the one with the biggest index) from the array
   * 
   * @throws `length_error` if array is already empty.
   */
  void remove()
  {
    if (idx > 0)
    {
      --idx;
      if (idx * resizeFactor <= this->size && this->size >= this->resizeFactor)
      {
        this->resizeForce(this->size / resizeFactor);
      }
    }
    else
    {
      throw length_error("Cannot remove element from empty dynamic array!");
    }
  }
  
  /**
   * Remove the element at index @param idx and shift all elements after @param idx to the left so all elements are in a row.ADJ_OFFSET_SINGLESHOT
   * 
   * @param idx The index whoose element is to be removed
   * 
   * @throw `out_of_range` if 
   */
  void remove(size_t idx)
  {
    if (idx < 0)
    {
      throw out_of_range("Index " + to_string(idx) + " is not a valid array index!");
    }

    if (idx > this->idx)
    {
      throw out_of_range("Index " + to_string(idx) + " out of bounds for dynamic array with " + to_string(this->idx) + " elements!");
    }

    this->idx--;

    for (size_t i = idx; i < this->idx; i++)
    {
      this->arr[i] = this->arr[i + 1];
    }

    if (this->idx * this->resizeFactor <= this->size && this->size >= this->resizeFactor)
    {
      this->resizeForce(this->size / this->resizeFactor);
    }
  }

  template<typename func>
  /**
   * foreach - Do something for each element in the DynamicArray
   * @param f - lambda or function pointer with signature `void(T)`
   * @return nothing
   */
  void foreach(func&& f)
  {
    for (size_t i = 0; i < this->getCount(); i++)
    {
      if constexpr (std::is_invocable_v<func, T>)
        f(this->operator[](i));
      else if constexpr (std::is_invocable_v<func, T, size_t>) 
        f(this->operator[](i), i);
      else
        static_assert(std::is_invocable_v<func, T> || std::is_invocable_v<func, T, int>, "Function must have signature 'void(T)' or 'void(T, int)'!");
    }
  }

  template<typename func>
  void foreach(size_t startIdx, func&& f)
  {
    if (startIdx < 0 || startIdx >= this->getCount())
      throw invalid_argument("Start index must be inside array bounds!");

    for (size_t i = startIdx; i < this->getCount(); i++)
    {
      if constexpr (std::is_invocable_v<func, T>)
        f(this->operator[](i));
      else if constexpr (std::is_invocable_v<func, T, size_t>)
        f(this->operator[](i), i);
      else
        static_assert(std::is_invocable_v<func, T> || std::is_invocable_v<func, T, size_t>, "Function must have signature 'void(T)' or 'void(T, size_t)'!");
    }
  }
};


/******************************************************
 * Function that require classes further down in the file
 ******************************************************/

//ToDo: Unit Test
template <typename T> Array<size_t> Array<T>::find(T el) const
{
  DynamicArray<size_t> res;
  for (size_t i = 0; i < this->size; i++)
  {
    if (this->operator[](i) == el)
    {
      res.add(i);
    }
  }

  if (res.getCount() == 0)
  {
    throw not_found("Could not find element!");
  }

  return res;
}

template<typename T, typename func>
static void foreach(Array<T> arr, func&& f)
{
  for (int i = 0; i < arr.getSize(); i++)
  {
    f(arr[i]);
  }
}

template<typename T, typename func>
static void foreach(DynamicArray<T> arr, func&& f)
{
  for (int i = 0; i < arr.getCount(); i++)
  {
    f(arr[i]);
  }
}