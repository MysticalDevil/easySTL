#pragma once

#ifndef EASYSTL_ALLOCATOR_WRAPPER_H_
#define EASYSTL_ALLOCATOR_WRAPPER_H_

namespace easystl {
    // Switch between different allocators: MemoryPoolAllocator or MallocAllocator.
    // The preprocessor directive allows choosing the allocator based on `USEMALLOC`.
#define USEMALLOC
#ifdef USEMALLOC
    using Alloc = MemoryPoolAllocator; ///< Default to use MemoryPoolAllocator if `USEMALLOC` is defined.
#else // !USEMALLOC
    using Alloc = MallocAllocator; ///< Use MallocAllocator if `USEMALLOC` is not defined.
#endif // !USEMALLOC
    /**
    * @class AllocatorWrapper
    * @brief A wrapper class that provides allocation and deallocation methods for a specific type T.
    * The default allocator used can be chosen based on th definition of `USEMALLOC`
    * @tparam T The type of objects this allocator will manage.
    * @tparam Allocator The allocator class to use for memory management. (default is `Alloc`)
    */
    template<class T, class Allocator = Alloc>
    class AllocatorWrapper {
    public:
        /**
        * @brief Allocates memory fot `n` objects of type `T`.
        * @param n The number of objects to allocate memory for.
        * @return A pointer to the allocated memory, or nullptr if `n == 0`.
        */
        static auto Allocate(const std::size_t n) -> T * {
            return n == 0 ? nullptr : static_cast<T *>(Allocator::Allocate(n * sizeof(T)));
        }

        /**
        * @brief Allocates memory for a single object of type `T`.
        * @return A pointer to the allocated memory.
        */
        static auto Allocate() -> T * {
            return static_cast<T *>(Allocator::Allocate(sizeof(T)));
        }

        /**
        * @brief Deallocates memory for `n` objects of type `T`.
        * @param ptr The pointer to the memory to deallocate.
        * @param n The number of objects to deallocate memory for.
        */
        static auto Deallocate(T *ptr, std::size_t n) -> void {
            if (n != 0) {
                Allocator::Deallocate(ptr, n * sizeof(T));
            }
        }

        /**
        * @brief Deallocates memory for a single object of type `T`/
        * @param ptr The pointer to the memory yo deallocate.
        */
        static auto Deallocate(T *ptr) -> void {
            Allocator::Deallocate(ptr, sizeof(T));
        }
    };
}

#endif // !EASYSTL_ALLOCATOR_WRAPPER_H_
