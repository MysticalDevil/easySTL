#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include "malloc_allocator.h"
#include "memory_pool_allocator.h"
#include "allocator_wrapper.h"

using namespace easystl;

// malloc_allocator test cases

TEST_CASE("MallocAllocator: Test Allocate and Deallocate") {
    constexpr std::size_t size = 100;
    void *ptr = MallocAllocator::Allocate(size);

    REQUIRE(ptr != nullptr);

    MallocAllocator::Deallocate(ptr, size);
}

TEST_CASE("MallocAllocator: Test Reallocate") {
    constexpr std::size_t size = 100;
    void *ptr = MallocAllocator::Allocate(size);

    REQUIRE(ptr != nullptr);

    constexpr std::size_t new_size = size * 2;
    void *new_ptr = MallocAllocator::Reallocate(ptr, size, new_size);

    REQUIRE(new_ptr != nullptr);

    MallocAllocator::Deallocate(new_ptr, size);
}

TEST_CASE("MallocAllocator: Test OOM Recovery") {
    static bool oom_called = false;

    auto OomRecoveryHandler = [] {
        oom_called = true;
    };

    MallocAllocator::SetMallocHandler(OomRecoveryHandler);

    constexpr std::size_t huge_size = static_cast<std::size_t>(-1) / 2;
    void *ptr = MallocAllocator::Allocate(huge_size);
    REQUIRE(ptr == nullptr);
    REQUIRE(oom_called == true);

    MallocAllocator::SetMallocHandler(nullptr);
}

// memory_pool_allocator test cases
TEST_CASE("MemoryPoolAllocator: Test basic allocate and deallocate") {
    constexpr std::size_t size = 16;
    void *ptr = MemoryPoolAllocator::Allocate(size);

    REQUIRE(ptr != nullptr);

    MemoryPoolAllocator::Deallocate(ptr, size);
}

TEST_CASE("MemoryPoolAllocator: Test small block reuse") {
    constexpr std::size_t size = 8;
    void *ptr1 = MemoryPoolAllocator::Allocate(size);
    void *ptr2 = MemoryPoolAllocator::Allocate(size);

    REQUIRE(ptr1 != nullptr);
    REQUIRE(ptr2 != nullptr);

    MemoryPoolAllocator::Deallocate(ptr1, size);
    MemoryPoolAllocator::Deallocate(ptr2, size);

    void *ptr3 = MemoryPoolAllocator::Allocate(size);
    REQUIRE((ptr3 == ptr1 || ptr3 == ptr2));


    MemoryPoolAllocator::Deallocate(ptr3, size);
}

TEST_CASE("MemoryPoolAllocator: Test large block allocate") {
    constexpr std::size_t large_size = kMaxBytes + 16;
    void *ptr = MemoryPoolAllocator::Allocate(large_size);

    REQUIRE(ptr != nullptr);

    MemoryPoolAllocator::Deallocate(ptr, large_size);
}

TEST_CASE("MemoryPoolAllocator: Test pool overflow handling") {
    constexpr std::size_t size = 64;

    void *ptrs[20];
    for (auto &ptr: ptrs) {
        ptr = MemoryPoolAllocator::Allocate(size);
        REQUIRE(ptr != nullptr);
    }

    for (auto &ptr: ptrs) {
        MemoryPoolAllocator::Deallocate(ptr, size);
    }
}

TEST_CASE("MemoryPoolAllocator: Test alignment") {
    constexpr std::size_t size = 32;
    void *ptr = MemoryPoolAllocator::Allocate(size);

    REQUIRE(reinterpret_cast<std::uintptr_t>(ptr) % kAlign == 0);

    MemoryPoolAllocator::Deallocate(ptr, size);
}

// allocator_wrapper test cases

TEST_CASE("AllocatorWrapper: Memory allocation  using MemoryPoolAllocator") {
#ifndef USEMALLOC
    REQUIRE(true);
#else
    using AllocType = AllocatorWrapper<int, MemoryPoolAllocator>;

    SECTION("Allocate single object") {
        int *ptr = AllocType::Allocate();
        REQUIRE(ptr != nullptr);
        AllocType::Deallocate(ptr);
    }

    SECTION("Allocate multiple objects") {
        constexpr std::size_t n = 10;
        int *ptr = AllocType::Allocate(n);
        REQUIRE(ptr != nullptr);
        AllocType::Deallocate(ptr);
    }

    SECTION("Allocate zero object") {
        int *ptr = AllocType::Allocate(0);
        REQUIRE(ptr == nullptr);
    }
#endif
}

TEST_CASE("AllocatorWrapper: Memory allocation using MallocAllocator") {
#ifndef USEMALLOC
    REQUIRE(TRUE)
#else
    using AllocType = AllocatorWrapper<int, MallocAllocator>;

    SECTION("Allocate single object") {
        int *ptr = AllocType::Allocate();
        REQUIRE(ptr != nullptr);
        AllocType::Deallocate(ptr);
    }

    SECTION("Allocate multiple objects") {
        constexpr std::size_t n = 10;
        int *ptr = AllocType::Allocate(n);
        REQUIRE(ptr != nullptr);
        AllocType::Deallocate(ptr);
    }

    SECTION("Allocate zero object") {
        int *ptr = AllocType::Allocate(0);
        REQUIRE(ptr == nullptr);
    }
#endif
}
