#pragma once

#ifndef EASYSTL_MALLOC_ALLOCATOR_H_
#define EASYSTL_MALLOC_ALLOCATOR_H_

#include <cstdlib>

namespace easystl {
    // malloc allocator
    // directly use malloc and free to manage memory
    class MallocAllocator {
    public:
        static auto Allocate(std::size_t size) -> void * {
            void *result = malloc(size);
            if (result == nullptr) { result = MallocInOom(size); }
            return result;
        }

        static auto Deallocate(void *obj, std::size_t /*size*/) -> void {
            free(obj);
        }

        static auto Reallocate(void *obj, std::size_t /*oldsize*/, size_t newsize) -> void * {
            void *result = realloc(obj, newsize);
            if (result == nullptr) { result = ReallocInOom(obj, newsize); }
            return result;
        }

        // simulate set_new_handler in C++
        // customer can call this func to set their own out-of-memory handler
        static auto (*SetMallocHandler(void (*func)()))() {
            void (*old)() = CustomerOomHandler;
            CustomerOomHandler = func;
            return old;
        }

    private:
        // funcs to be called when out of memory
        static auto MallocInOom(std::size_t size) -> void *;

        static auto ReallocInOom(void *obj, std::size_t size) -> void *;

        static auto (*CustomerOomHandler)() -> void;
    };

    void (*MallocAllocator::CustomerOomHandler)() = nullptr;

    inline auto MallocAllocator::MallocInOom(const std::size_t size) -> void * {
        int retry_count = 0;
        constexpr int max_retries = 3;

        while (retry_count < max_retries) {
            void (*my_malloc_handler)() = CustomerOomHandler;
            // if CustomerOomHandler do not be set just exit
            if (my_malloc_handler == nullptr) {
                std::abort();
            }
            (*my_malloc_handler)();
            if (void *result = malloc(size)) { return result; }
            retry_count++;
        }
        return nullptr;
    }

    inline auto MallocAllocator::ReallocInOom(void *obj, const std::size_t size) -> void * {
        int retry_count = 0;
        constexpr int max_retries = 3;

        while (retry_count < max_retries) {
            void (*my_malloc_handler)() = CustomerOomHandler;
            if (my_malloc_handler == nullptr) {
                std::abort();
            }
            (*my_malloc_handler)();
            if (void *result = realloc(obj, size)) { return result; }
            retry_count++;
        }
        return nullptr;
    }
} // namespace easystl

#endif // !EASYSTL_MALLOC_ALLOCATOR_H_
