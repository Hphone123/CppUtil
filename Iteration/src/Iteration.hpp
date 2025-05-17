#include <iterator>

template <typename T> class Iterator
{
  T* ptr;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = T;
  using difference_type   = std::ptrdiff_t;
  using pointer           = T*;
  using reference         = T&;

  Iterator(T* p) : ptr(p) {}

  T& operator*() const { return *ptr; }
  T* operator->() const { return ptr; }

  Iterator& operator++() { ++ptr; return *this; }
  Iterator operator++(int) { MyIterator tmp = *this; ++(*this); return tmp; }

  friend bool operator==(const Iterator& other) const { return ptr == other.ptr; }
  friend bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
};

template <typename T> class Iterable
{
public:
  virtual Iterator<T> begin() = 0;
  virtual Iterator<T> end  () = 0;
};