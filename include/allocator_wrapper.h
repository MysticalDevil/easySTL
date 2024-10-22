#pragma once

#ifndef EASYSTL_ALLOCATOR_WRAPPER_H_
#define EASYSTL_ALLOCATOR_WRAPPER_H_

namespace easystl {
#define USEMALLOC
#ifdef USEMALLOC
    using Alloc = MemoryPoolAllocator;
#else // !USEMALLOC
    using Alloc = MallocAllocator;
#endif // !USEMALLOC
    template<class T, class Allocator = Alloc>
    class AllocatorWrapper {
    public:
        static auto Allocate(const std::size_t n) -> T * {
            return n == 0 ? nullptr : static_cast<T *>(Allocator::Allocate(n * sizeof(T)));
        }

        static auto Allocate() -> T * {
            return static_cast<T *>(Allocator::Allocate(sizeof(T)));
        }

        static auto Deallocate(T *ptr, std::size_t n) -> void {
            if (n != 0) {
                Allocator::Deallocate(ptr, n * sizeof(T));
            }
        }

        static auto Deallocate(T *ptr) -> void {
            Allocator::Deallocate(ptr, sizeof(T));
        }
    };
}

#endif // !EASYSTL_ALLOCATOR_WRAPPER_H_
