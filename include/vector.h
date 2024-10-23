#pragma once

#ifndef EASYSTL_VECTOR_H_
#define EASYSTL_VECTOR_H_

#include <initializer_list>
#include <type_traits>

#include "algo.h"
#include "allocator_wrapper.h"
#include "constructor.h"
#include "iterator.h"
#include "uninitialized.h"

namespace easystl {
template <class T, class Alloc = Allo> class vector {
public:
  // type alias
  using value_type = T;
  using pointer = T *;
  using iterator = T *;
  using reference = T &;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  ///< @brief Default constructor.
  vector() noexcept : begin_(nullptr), end_(nullptr), capacity_(nullptr) {}
  /**
   * @brief Constructs a vector with a specified length, initialized with a
   * value.
   *
   * @param len The number of elements.
   * @param value The value to initialize the elements with.
   */
  vector(const size_type len, const T &value) noexcept { NumsInit(len, value); }
  /**
   * @brief Constructs a vector with a specified length, initialized with
   * default values.
   *
   * @param len The number of elements.
   */
  explicit vector(const size_type len) noexcept { NumsInit(len, T()); }
  /**
   * @brief Constructs a vector from a range of iterators.
   *
   * @tparam Iterator Type of the input iterator.
   * @param first The beginning of the range.
   * @param last The end of the range.
   */
  template <class Iterator,
            std::enable_if_t<IsIterator<Iterator>::value, int> = 0>
  vector(Iterator first, Iterator last) noexcept {
    RangeInit(first, last);
  }
  /**
   * @brief Constructs a vector from an initializer list.
   *
   * @param ilist The initializer list.
   */
  vector(std::initializer_list<value_type> ilist) {
    RangeInit(ilist.begin(), ilist.end());
  }

  /**
   * @brief Copy constructor.
   *
   * @param other The vector to copy from.
   */
  vector(const vector &other) noexcept { RangeInit(other.begin_, other.end_); }

  /**
   * @brief Copy assignment operator.
   *
   * @param rhs The vector to assign from.
   * @return A reference to the current vector.
   */
  vector &operator=(const vector &rhs) noexcept {
    if (this != &rhs) {
      const size_type rhslen = rhs.size();
      if (rhslen > capacity()) {
        vector tmp(rhs);
        swap(tmp);
      } else if (size() >= rhslen) {
        auto i = Copy(rhs.begin_, rhs.end_, begin_);
        Destroy(i, end_);
        end_ = begin_ + rhslen;
      } else {
        Copy(rhs.begin_, rhs.end_, begin_);
        uninitialized_copy(rhs.begin_ + size(), rhs.end_, end_);
        end_ = begin_ + rhslen;
      }
    }
    return *this;
  }

  vector &operator=(std::initializer_list<value_type> ilist) noexcept {
    vector<T> tmp(ilist);
    swap(tmp);
    return *this;
  }

  ///< @brief Destructor.
  ~vector() noexcept { DestroyDeallocate(begin_, end_, capacity_ - begin_); }

  auto begin() noexcept -> iterator { return begin_; }
  auto end() noexcept -> iterator { return end_; }
  [[nodiscard]] auto size() const noexcept -> size_type {
    return static_cast<size_type>(end_ - begin_);
  }
  [[nodiscard]] auto empty() const noexcept -> bool { return begin_ == end_; }
  [[nodiscard]] auto capacity() const noexcept -> size_type {
    return static_cast<size_type>(capacity_ - begin_);
  }
  auto operator[](size_type n) noexcept -> reference { return *(begin_ + n); }
  auto front() noexcept -> reference { return *begin_; }
  auto back() noexcept -> reference { return *(end_ - 1); }

  /**
   * @brief Adds an element to the end of the vector.
   *
   * @param x The element to add.
   */
  auto push_back(const T &x) noexcept -> void {
    if (end_ != capacity_) {
      Construct(end_, x);
      ++end_;
    } else {
      InsertAux(end_, 1, x);
    }
  }

  /**
   * @brief Removes the last element from the vector.
   *
   * @return A reference to the removed element.
   */
  auto pop_back() noexcept -> T & {
    if (empty()) {
      return nullptr;
    }
    --end_;
    Destroy(end_);
    return *end_;
  }

  /**
   * @brief Erases an element at a specified position.
   *
   * @param pos The position of the element to erase.
   * @return An iterator to the next element.
   */
  auto erase(iterator pos) noexcept -> iterator {
    Copy(pos + 1, end_, pos);
    Destroy(end_);
    --end_;
    return pos;
  }

  /**
   * @brief Erases a range of elements.
   *
   * @param first The beginning of the range to erase.
   * @param last The end of the range to erase.
   * @return An iterator to the end of the erased range.
   */
  auto erase(iterator first, iterator last) noexcept -> iterator {
    if (first != last) {
      auto i = Copy(last, end_, first);
      Destroy(i, end_);
      end_ = end_ - (last - first);
    }
    return last;
  }

  /**
   * @brief Resizes the vector to a specified size, filling new elements with a
   * value.
   *
   * @param newsize The new size.
   * @param x The value to fill new elements with.
   */
  auto resize(size_type newsize, const T &x) noexcept -> void {
    if (newsize < size()) {
      erase(begin_ + newsize, end_);
    } else {
      insert(end_, newsize - size(), x);
    }
  }

  /**
   * @brief Resizes the vector to a specified size, filling new elements with
   * default values.
   *
   * @param newsize The new size.
   */
  auto resize(size_type newsize) noexcept -> void { resize(newsize, T()); }

  ///<  @brief Clears the vector, removing all elements.
  auto clear() noexcept { erase(begin_, end_); }

  /**
   * @brief Returns a pointer to the underlying array.
   *
   * @return A pointer to the data.
   */
  auto data() noexcept -> pointer { return begin_; }

  /**
   * @brief Swaps the contents of this vector with another.
   *
   * @param rhs The vector to swap with.
   */
  auto swap(vector &rhs) noexcept -> void {
    if (this != &rhs) {
      Swap(begin_, rhs.begin_);
      Swap(end_, rhs.end_);
      Swap(capacity_, rhs.capacity_);
    }
  }

  /**
   * @brief Inserts elements at a specified position.
   *
   * @param pos The position to insert at.
   * @param size The number of elements to insert.
   * @param x The value of the elements to insert.
   * @return An iterator to the position where elements were inserted.
   */
  auto insert(iterator pos, size_type size, const T &x) noexcept -> iterator {
    if (static_cast<size_type>(capacity_ - end_) >= size) {
      const size_type elemsafter = end_ - pos;
      iterator oldend = end_;
      if (elemsafter > size) {
        uninitialized_copy(end_ - size, end_, end_);
        end_ += size;
        CopyBackward(pos, oldend - size, oldend);
        Fill(pos, pos + size, x);
      } else {
        uninitialized_fill_n(end_, size - elemsafter, x);
        end_ += size - elemsafter;
        uninitialized_copy(pos, oldend, end_);
        end_ += elemsafter;
        Fill(pos, oldend, x);
      }
    } else {
      InsertAux(pos, size, x);
    }
    return pos;
  }

  /**
   * @brief Inserts a range of elements from iterators.
   *
   * @tparam Iter1 Type of the first input iterator.
   * @tparam Iter2 Type of the second input iterator.
   * @param pos The position to insert at.
   * @param first The beginning of the range to insert from.
   * @param last The end of the range to insert from.
   * @return An iterator to the position where elements were inserted.
   */
  template <class Iter1, class Iter2,
            std::enable_if_t<IsIterator<Iter1>::value, int> = 0,
            std::enable_if_t<IsIterator<Iter2>::value, int> = 0>
  auto insert(Iter1 pos, Iter2 first, Iter2 last) noexcept -> iterator {
    if (first == last)
      return pos;
    const auto size = last - first;
    if (static_cast<size_type>(capacity_ - end_) >= size) {
      const size_type elemsafter = end_ - pos;
      iterator oldend = end_;
      if (elemsafter > size) {
        uninitialized_copy(end_ - size, end_, end_);
        end_ += size;
        Copybackward(pos, oldend - size, oldend);
        Copy(first, last, pos);
      } else {
        uninitialized_fill_n(end_, size - elemsafter, T());
        end_ += size - elemsafter;
        uninitialized_copy(pos, oldend, end_);
        end_ += elemsafter;
        Copy(first, last, pos);
      }
    } else {
      InsertAux(pos, first, last);
    }
    return pos;
  }

  /**
   * @brief Inserts a single element at a specified position.
   *
   * @param pos The position to insert at.
   * @param x The element to insert.
   * @return An iterator to the position where the element was inserted.
   */
  auto insert(iterator pos, const T &x) noexcept -> iterator {
    return insert(pos, 1, x);
  }

  /**
   * @brief Assigns new values to the vector, replacing its contents.
   *
   * @param n The number of elements to assign.
   * @param value The value to fill the vector with.
   */
  auto assign(size_type n, const T &value) noexcept -> void {
    clear();
    if (n > capacity()) {
      vector tmp(n, value);
      swap(tmp);
    } else {
      uninitialized_fill_n(begin_, n, value);
      end_ = begin_ + n;
    }
  }

  /**
   * @brief Assigns values from a range of iterators.
   *
   * @tparam Iter Type of the input iterator.
   * @param first The beginning of the range.
   * @param last The end of the range.
   */
  template <class Iter, std::enable_if_t<IsIterator<Iter>::value, int> = 0>
  auto assign(Iter first, Iter last) noexcept -> void {
    clear();
    const size_type len = last - first;
    if (len > capacity()) {
      vector tmp(first, last);
      swap(tmp);
    } else {
      uninitialized_copy(first, last, begin_);
      end_ = begin_ + len;
    }
  }

  /**
   * @brief Assigns values from an initializer list.
   *
   * @param ilist The initializer list.
   */
  auto assign(std::initializer_list<value_type> ilist) noexcept -> void {
    assign(ilist.begin(), ilist.end());
  }

private:
  using DataAllocator = AllocatorWrapper<T, Alloc>;

  /**
   * @brief Initializes the vector with a specific number of elements and a
   * value.
   *
   * @param n The number of elements.
   * @param value The value to initialize the elements with.
   */
  auto NumsInit(size_type n, const T &value) noexcept {
    size_type initsize = Max(n, static_cast<size_type>(16));
    iterator current = DataAllocator::Allocate(initsize);
    begin_ = current;
    end_ = uninitialized_fill_n(current, n, value);
    capacity_ = begin_ + initsize;
  }

  /**
   * @brief Initializes the vector from a range of iterators.
   *
   * @tparam Iter The type of the input iterator.
   * @param first The beginning of the range.
   * @param last The end of the range.
   */
  template <class Iter> auto RangeInit(Iter first, Iter last) noexcept -> void {
    size_type initsize =
        Max(static_cast<size_type>(last - first), static_cast<size_type>(16));
    begin_ = DataAllocator::Allocate(initsize);
    end_ = uninitialized_copy(first, last, begin_);
    capacity_ = begin_ + initsize;
  }

  /**
   * @brief Destroys and deallocates the memory used by the vector.
   *
   * @param first The beginning of the range to destroy.
   * @param last The end of the range to destroy.
   * @param len The length of the range.
   */
  auto DestroyDeallocate(iterator first, iterator last, size_type len) noexcept
      -> void {
    if (first) {
      Destroy(first, last);
      DataAllocator::Deallocate(first, len);
    }
  }

  /**
   * @brief Auxiliary function for inserting multiple elements at a specified
   * position.
   *
   * @param pos The position to insert at.
   * @param nums The number of elements to insert.
   * @param x The value of the elements to insert.
   */
  auto InsertAux(iterator pos, size_type nums, const T &x) noexcept -> void {
    const size_type newsize = Max(size() * 2, 16);
    iterator newbegin = DataAllocator::Allocate(newsize);
    iterator newend = uninitialized_copy(begin_, pos, newbegin);
    newend = uninitialized_fill_n(newend, nums, x);
    newend = uninitialized_copy(pos, end_, newend);
    DestroyDeallocate(begin_, end_, capacity_ - begin_);
    begin_ = newbegin;
    end_ = newend;
    capacity_ = begin_ + newsize;
  }

  /**
   * @brief Auxiliary function for inserting a range of elements from iterators.
   *
   * @tparam Iter1 Type of the first input iterator.
   * @tparam Iter2 Type of the second input iterator.
   * @param pos The position to insert at.
   * @param first The beginning of the range to insert from.
   * @param last The end of the range to insert from.
   */
  template <class Iter1, class Iter2,
            std::enable_if_t<IsIterator<Iter1>::value, int> = 0,
            std::enable_if_t<IsIterator<Iter2>::value, int> = 0>
  auto InsertAux(Iter1 pos, Iter2 first, Iter2 last) noexcept -> void {
    const size_type newsize = Max(size() * 2, 16);
    iterator newbegin = DataAllocator::Allocate(newsize);
    iterator newend = uninitialized_copy(begin_, pos, newbegin);
    newend = uninitialized_copy(first, last, newend);
    newend = uninitialized_copy(pos, end_, newend);
    DestroyDeallocate(begin_, end_, capacity_ - begin_);
    begin_ = newbegin;
    end_ = newend;
    capacity_ = begin_ + newsize;
  }

  iterator begin_ = nullptr;
  iterator end_ = nullptr;
  iterator capacity_ = nullptr;
};
} // namespace easystl

#endif // !EASYSTL_VECTOR_H_
