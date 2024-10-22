#include <catch2/catch_test_macros.hpp>

#include "constructor.h"

using namespace easystl;

struct TestObject {
    int value;

    TestObject() : value(0) {
    }

    explicit TestObject(const int v) : value(v) {
    }

    ~TestObject() = default;
};

TEST_CASE("Construct and Destroy") {
    SECTION("Default Construct") {
        const auto obj = static_cast<TestObject *>(operator new(sizeof(TestObject)));
        Construct(obj);
        REQUIRE(obj->value == 0);
        Destroy(obj);
        operator delete(obj);
    }

    SECTION("Parameterized Construct") {
        auto *obj = static_cast<TestObject *>(operator new(sizeof(TestObject)));
        Construct(obj, 42);
        REQUIRE(obj->value == 42);
        Destroy(obj);
        operator delete(obj);
    }

    SECTION("Array Construct and Destroy") {
        constexpr int size = 5;
        auto *arr = static_cast<TestObject *>(operator new[](size * sizeof(TestObject)));

        for (int i = 0; i < size; ++i) {
            Construct(arr + i, i + 1);
        }

        for (int i = 0; i < size; ++i) {
            REQUIRE(arr[i].value == i + 1);
        }

        for (int i = 0; i < size; ++i) {
            Destroy(arr + i);
        }

        operator delete[](arr);
    }
}
