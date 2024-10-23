#include <catch2/catch_test_macros.hpp>

#include "vector.h"

using namespace easystl;

TEST_CASE("Vector construction and initialization") {
  vector<int> v1;
  REQUIRE(v1.size() == 0);
  REQUIRE(v1.empty());

  vector<int> v2(5, 10);
  REQUIRE(v2.size() == 5);
  REQUIRE(v2[0] == 10);
  REQUIRE(v2[1] == 10);
  REQUIRE(v2[2] == 10);
  REQUIRE(v2[3] == 10);
  REQUIRE(v2[4] == 10);
}

TEST_CASE("Vector copy constructor") {
  vector<int> v1(3, 5);
  vector<int> v2(v1);

  REQUIRE(v2.size() == 3);
  REQUIRE(v2[0] == 5);
  REQUIRE(v2[1] == 5);
  REQUIRE(v2[2] == 5);
}

TEST_CASE("Vector assignment operator") {
  vector<int> v1(2, 3);
  vector<int> v2;
  v2 = v1;

  REQUIRE(v2.size() == 2);
  REQUIRE(v2[0] == 3);
  REQUIRE(v2[1] == 3);
}

TEST_CASE("Vector push_back and pop_back") {
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);

  REQUIRE(v.size() == 3);
  REQUIRE(v.back() == 3);

  v.pop_back();
  REQUIRE(v.size() == 2);
  REQUIRE(v.back() == 2);
}

TEST_CASE("Vector resize") {
  vector<int> v(2, 5);
  v.resize(5, 10);

  REQUIRE(v.size() == 5);
  REQUIRE(v[2] == 10);
  REQUIRE(v[3] == 10);
  REQUIRE(v[4] == 10);

  v.resize(1);
  REQUIRE(v.size() == 1);
  REQUIRE(v[0] == 5);
}

TEST_CASE("Vector insert and erase") {
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);

  v.insert(v.begin() + 1, 4);
  REQUIRE(v.size() == 4);
  REQUIRE(v[1] == 4);

  v.erase(v.begin() + 1);
  REQUIRE(v.size() == 3);
  REQUIRE(v[1] == 2);
}

TEST_CASE("Vector clear") {
  vector<int> v(3, 7);
  v.clear();
  REQUIRE(v.size() == 0);
  REQUIRE(v.empty());
}
