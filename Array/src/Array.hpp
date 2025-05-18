#pragma once

#include <stdexcept>
#include <string.h>

using namespace std;

template <typename T> class Array
{
protected:
  T * arr;
  int size;

public:
  Array<T>()
  {
    this->arr = new T [0]();
  }

  Array<T>(int size)
  {
    if (size < 1)
    {
      throw length_error("Size " + to_string(size) + " is not a valid array size!");
    }

    this->arr  = new T[size]();
    this->size = size;
  }

  Array<T> (const Array<T>& other)
  {
    this->arr = new T[other.size]();

    for (int i = 0; i < other.size; i++)
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
      for (int i = 0; i < other.size; i++)
      {
        tmp[i] = other.arr[i];
      }
      
      this->arr  = tmp;
      this->size = other.size;
    }

    return *this;
  }

  ~Array<T>()
  {
    delete[] this->arr;
  }

  T& operator[] (int idx)
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
  
  T operator[] (int idx) const
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

  int getSize()
  {
    return this->size;
  }
};

template <typename T> class ResizableArray : public Array<T>
{
public:

  ResizableArray<T>(int size) : Array<T>(size){};

  /**
   * Resizes the array.
   * 
   * @param newSize The size the array should be given.
   * @throws `length_error` for invalid sizes
   * @throws `length_error` when newSize is smaller than current size
   * @note Use `resizeForce()` to force a size down.
   */
  void resize(int newSize)
  {
    if (newSize < 1)
    {
      throw length_error("Size " + to_string(newSize) + " is not a valid array size!");
    }

    if (newSize > this->size)
    {
      T * tmp = new T[newSize]();
      for (int i = 0; i < this->size; i++)
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
  void resizeForce (int newSize)
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
      for (int i = 0; i < newSize; i++)
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
  int idx = 0;
  int resizeFactor = 2;

  using ResizableArray<T>::resize;
  using ResizableArray<T>::resizeForce;

public:
  DynamicArray<T>() : ResizableArray<T>(2)
  {
    this->idx = 0;
    this->resizeFactor = 2;
  }

  DynamicArray<T>(int size) :  ResizableArray<T>(size)
  {
    if (size < 1)
    {
      throw invalid_argument("Size must be bigger than 0!");
    }

    this->idx = 0;
    this->resizeFactor = 2;
  }
  
  DynamicArray<T>(int size, int resizeFactor) :  ResizableArray<T>(size)
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
  
  T& operator[] (int idx)
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
  
  T operator[] (int idx) const = delete;
  
  void add(T item)
  {
    if (this->idx >= this->size)
    {
      this->resize(this->idx * this->resizeFactor);
    }

    this->arr[idx++] = item; 
    
  }

  int getCount()
  {
    return this->idx;
  }
  
  void add(T item, int idx)
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
    
    for (int i = this->size - 1; i > idx; i--)
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
  void remove(int idx)
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

    for (int i = idx; i < this->idx; i++)
    {
      this->arr[i] = this->arr[i + 1];
    }

    if (this->idx * this->resizeFactor <= this->size && this->size >= this->resizeFactor)
    {
      this->resizeForce(this->size / this->resizeFactor);
    }
  }
};
