#pragma once

#ifndef EASYSTL_UNINITIALIZED_H_
#define EASYSTL_UNINITIALIZED_H_

#include "constructor.h"
#include <cstdlib>

// helper funcs to construct value in uninitialized place which is already
// allocated
namespace easystl {
/**
 * @brief Copies uninitialized objects from one range to another.
 *
 * This function constructs a copy of the objects in the range [first, last)
 * into the uninitialized memory range starting at result. If an exception is
 * thrown during the construction, previously constructed objects are destroyed.
 *
 * @tparam InputIter The type of the input iterator.
 * @tparam ForwardIter The type of the output iterator.
 * @param first The beginning of the source range (inclusive).
 * @param last The end of the source range (exclusive).
 * @param result The beginning of the destination range.
 * @return ForwardIter An iterator to the end of the constructed range.
 */
template <class InputIter, class ForwardIter>
auto uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
    -> ForwardIter {
  auto current = result;
  try {
    for (; first != last; ++first, ++current) {
      Construct(&*current, *first);
    }
  } catch (...) {
    Destroy(result, current);
    std::abort();
  }
  return current;
}

/**
 * @brief Fills uninitialized memory with a specified value.
 *
 * This function constructs objects with the specified value in the range
 * [first, last). If an exception is thrown during the construction,
 * previously constructed objects are destroyed.
 *
 * @tparam ForwardIter The type of the iterator.
 * @tparam T The type of the value to be used for filling.
 * @param first The beginning of the range (inclusive).
 * @param last The end of the range (exclusive).
 * @param value The value to fill the range with.
 */
template <class ForwardIter, class T>
auto uninitialized_fill(ForwardIter first, ForwardIter last, const T &value)
    -> void {
  auto current = first;
  try {
    for (; current != last; ++current) {
      Construct(&*current, value);
    }
  } catch (...) {
    Destroy(first, current);
    std::abort();
  }
}

/**
 * @brief Fills uninitialized memory with a specified value for a given number
 * of elements.
 *
 * This function constructs objects with the specified value in the range
 * starting at first for n elements. If an exception is thrown during the
 * construction, previously constructed objects are destroyed.
 *
 * @tparam ForwardIter The type of the iterator.
 * @tparam Size The type of the size parameter (e.g., int).
 * @tparam T The type of the value to be used for filling.
 * @param first The beginning of the range (inclusive).
 * @param n The number of elements to fill.
 * @param value The value to fill the range with.
 * @return ForwardIter An iterator to the end of the filled range.
 */
template <class ForwardIter, class Size, class T>
auto uninitialized_fill_n(ForwardIter first, Size n, const T &value)
    -> ForwardIter {
  auto current = first;
  try {
    for (; n > 0; --n, ++current) {
      Construct(&*current, value);
    }
  } catch (...) {
    Destroy(first, current);
    std::abort();
  }
  return current;
}
} // namespace easystl

#endif // !EASYSTL_UNINITIALIZED_H_
