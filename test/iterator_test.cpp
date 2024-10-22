#include <catch2/catch_test_macros.hpp>

#include "iterator.h"

#include <list>
#include <vector>

using namespace easystl;

TEST_CASE("IteratorTraits for pointer types") {
    SECTION("Raw pointer type") {
        using traits = IteratorTraits<int *>;
        REQUIRE(std::is_same<traits::IteratorCategory, RandomAccessIteratorTag>::value);
        REQUIRE(std::is_same<traits::ValueType, int>::value);
        REQUIRE(std::is_same<traits::Pointer, int*>::value);
        REQUIRE(std::is_same<traits::Reference, int&>::value);
        REQUIRE(std::is_same<traits::DifferenceType, std::ptrdiff_t>::value);
    }
    SECTION("Const raw pointer type") {
        using traits = IteratorTraits<const int *>;
        REQUIRE(std::is_same<traits::IteratorCategory, RandomAccessIteratorTag>::value);
        REQUIRE(std::is_same<traits::ValueType, int>::value);
        REQUIRE(std::is_same<traits::Pointer, const int*>::value);
        REQUIRE(std::is_same<traits::Reference, int&>::value);
        REQUIRE(std::is_same<traits::DifferenceType, std::ptrdiff_t>::value);
    }
}

TEST_CASE("IsIterator test") {
    SECTION("True for raw pointer") {
        REQUIRE(IsIterator<int*>::value);
    }
    SECTION("True for std::vector<int>::iterator") {
        REQUIRE(IsIterator<std::vector<int>::iterator>::value);
    }
    SECTION("True for std::list<int>::iterator") {
        REQUIRE(IsIterator<std::list<int>::iterator>::value);
    }
    SECTION("False for non-iterator type") {
        REQUIRE_FALSE(IsIterator<int>::value);
    }
}

TEST_CASE("Distance function test") {
    SECTION("Distance for random access iterators (pointers)") {
        int arr[] = {1, 2, 3, 4 ,5};
        int *first = arr;
        int *last = arr + 5;
        REQUIRE(Distance(first, last) == 5);
    }
    // SECTION("Distance for bidirectional iterators (std::list)") {
    //     std::list lst = {1, 2, 3, 4 ,5};
    //     const auto first = lst.begin();
    //     const auto last = lst.end();
    //     REQUIRE(Distance(first, last) == 5);
    // }
}

TEST_CASE("Advance function test") {
    SECTION("Advance for random access iterator (pointers)") {
        int arr[] = {1, 2, 3, 4, 5};
        int* iter = arr;
        Advance(iter, 3);
        REQUIRE(*iter == 4);
    }

    // SECTION("Advance for bidirectional iterator (std::list)") {
    //     std::list<int> lst = {1, 2, 3, 4, 5};
    //     auto iter = lst.begin();
    //     Advance(iter, 2);
    //     REQUIRE(*iter == 3);
    // }
}