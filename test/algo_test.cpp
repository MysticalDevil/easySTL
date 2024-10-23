#include <catch2/catch_test_macros.hpp>

#include "algo.h"

using namespace easystl;

TEST_CASE("Easystl Algorithms") {
    SECTION("Max Function") {
        REQUIRE(Max(3, 5) == 5);
        REQUIRE(Max(10.5, 10.2) == 10.5);
        REQUIRE(Max(-1, -5) == -1);
    }
    SECTION("Copy Function") {
        int sources[] = {1, 2, 3, 4, 5};
        int dest[5] = {0};
        Copy(sources, sources + 5, dest);
        for (int i = 0; i < 5; i++) {
            REQUIRE(dest[i] == sources[i]);
        }
    }
    SECTION("CopyBackward Function") {
        int sources[] = {1, 2, 3, 4, 5};
        int dest[5] = {0};
        CopyBackward(sources, sources + 5, dest + 5);
        for (int i = 0; i < 5; i++) {
            REQUIRE(dest[i] == sources[i]);
        }
    }
    SECTION("Fill Function") {
        int array[5] = {0};
        Fill(array, array + 5, 42);
        for (int i: array) {
            REQUIRE(i == 42);
        }
    }
    SECTION("Swap Function") {
        int a = 10, b = 20;
        Swap(a, b);
        REQUIRE(a == 20);
        REQUIRE(b == 10);
    }
}
