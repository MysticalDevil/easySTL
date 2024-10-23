#pragma once

#ifndef EASYSTL_CONSTRUCTOR_H_
#define EASYSTL_CONSTRUCTOR_H_

namespace easystl {
/**
 * @brief Constructs an object of type T in the memory pointed to by p.
 *
 * This function uses placement new to construct a default-constructed
 * object of type T at the given memory address.
 *
 * @tparam T The type of the object to be constructed.
 * @param p Pointer to the memory location where the object will be constructed.
 */
template <class T> void Construct(T *p) { new (p) T(); }

/**
 * @brief Constructs an object of type T in the memory pointed to by p
 *        with the provided value.
 *
 * This function uses placement new to construct an object of type T
 * at the given memory address, initialized with the value of the
 * provided argument.
 *
 * @tparam T The type of the object to be constructed.
 * @tparam U The type of the value used to initialize the object.
 * @param p Pointer to the memory location where the object will be constructed.
 * @param value The value used to initialize the constructed object.
 */
template <class T, class U> void Construct(T *p, const U &value) {
  new (p) T(value);
}

/**
 * @brief Destroys an object of type T pointed to by p.
 *
 * This function calls the destructor of the object, releasing any resources
 * associated with the object.
 *
 * @tparam T The type of the object to be destroyed.
 * @param p Pointer to the object to be destroyed.
 */
template <class T> void Destroy(T *p) { p->~T(); }

/**
 * @brief Destroys a range of objects pointed to by the iterators.
 *
 * This function iterates through the range defined by [first, last)
 * and calls the Destroy function on each object, releasing resources.
 *
 * @tparam ForwardIterator The type of the iterator used to traverse the range.
 * @param first The beginning of the range (inclusive).
 * @param last The end of the range (exclusive).
 */
template <class ForwardIterator>
void Destroy(ForwardIterator first, ForwardIterator last) {
  for (; first != last; ++first) {
    Destroy(&*first);
  }
}
} // namespace easystl

#endif // !EASYSTL_CONSTRUCTOR_H_
