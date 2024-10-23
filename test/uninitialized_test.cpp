#include <catch2/catch_test_macros.hpp>

#include "uninitialized.h"

using namespace easystl;

struct TestObject {
    int value;

    TestObject() : value(0) {
    }

    explicit TestObject(const int v) : value(v) {
    }

    ~TestObject() = default;
};

TEST_CASE("Uninitialized Functions") {
    SECTION("Uninitialized Copy") {
        constexpr int size = 5;
        TestObject source[size] = {TestObject(1), TestObject(2), TestObject(3), TestObject(4), TestObject(5)};

        auto *dest = static_cast<TestObject *>(operator new[](size * sizeof(TestObject)));

        auto end = uninitialized_copy(source, source + size, dest);

        REQUIRE(end == dest + size);
        for (int i = 0; i < size; ++i) {
            REQUIRE(dest[i].value == source[i].value);
        }

        Destroy(dest, dest + size);
        operator delete[](dest);
    }

    SECTION("Uninitialized Fill") {
        constexpr int size = 5;
        auto *dest = static_cast<TestObject *>(operator new[](size * sizeof(TestObject)));

        uninitialized_fill(dest, dest + size, TestObject(42));

        for (int i = 0; i < size; ++i) {
            REQUIRE(dest[i].value == 42);
        }

        Destroy(dest, dest + size);
        operator delete[](dest);
    }

    SECTION("Uninitialized Fill N") {
        constexpr int size = 5;
        auto *dest = static_cast<TestObject *>(operator new[](size * sizeof(TestObject)));

        uninitialized_fill_n(dest, size, TestObject(99));

        for (int i = 0; i < size; ++i) {
            REQUIRE(dest[i].value == 99);
        }

        Destroy(dest, dest + size);
        operator delete[](dest);
    }

    // SECTION("Exception Handling in Uninitialized Copy") {
    //     struct FaultyObject {
    //         FaultyObject() { throw std::runtime_error("Construction failed"); }
    //
    //         ~FaultyObject() = default;
    //     };
    //
    //     constexpr int size = 5;
    //     FaultyObject source[size];
    //
    //     auto *dest = static_cast<FaultyObject *>(operator new[](size * sizeof(FaultyObject)));
    //
    //     REQUIRE_THROWS(uninitialized_copy(source, source + size, dest));
    //
    //     operator delete[](dest);
    // }
}
