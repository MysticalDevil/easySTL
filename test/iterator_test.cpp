#include <catch2/catch_test_macros.hpp>

#include "iterator.h"

#include <list>

using namespace easystl;

#define CATCH_CONFIG_MAIN

using namespace easystl;

// Test for IteratorTraits
TEST_CASE("IteratorTraits") {
    SECTION("Test with raw pointer") {
        REQUIRE(std::is_same_v<IteratorTraits<int*>::IteratorCategory, RandomAccessIteratorTag>);
        REQUIRE(std::is_same_v<IteratorTraits<int*>::ValueType, int>);
        REQUIRE(std::is_same_v<IteratorTraits<int*>::Pointer, int*>);
        REQUIRE(std::is_same_v<IteratorTraits<int*>::Reference, int&>);
    }

    SECTION("Test with const pointer") {
        REQUIRE(std::is_same_v<IteratorTraits<const int*>::IteratorCategory, RandomAccessIteratorTag>);
        REQUIRE(std::is_same_v<IteratorTraits<const int*>::ValueType, int>);
        REQUIRE(std::is_same_v<IteratorTraits<const int*>::Pointer, const int*>);
        REQUIRE(std::is_same_v<IteratorTraits<const int*>::Reference, int&>);
    }
}