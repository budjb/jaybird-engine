#include <catch2/catch_test_macros.hpp>

#include "container/Iterator.hpp"

namespace {
struct Item {
  int value;
};
}  // namespace

TEST_CASE("Iterator dereference returns current element", "[iterator]") {
  int values[] = {10, 20, 30};
  const core::container::Iterator it(values);

  REQUIRE(*it == 10);
}

TEST_CASE("Iterator arrow returns pointer to current element", "[iterator]") {
  Item values[] = {{7}, {9}};
  const core::container::Iterator it(values);

  REQUIRE(it->value == 7);
}

TEST_CASE("Pre-increment advances iterator and returns self", "[iterator]") {
  int values[] = {1, 2, 3};
  core::container::Iterator it(values);

  auto& advanced = ++it;

  REQUIRE(&advanced == &it);
  REQUIRE(*it == 2);
}

TEST_CASE("Post-increment returns previous iterator and advances", "[iterator]") {
  int values[] = {1, 2, 3};
  core::container::Iterator it(values);

  const auto previous = it++;

  REQUIRE(*previous == 1);
  REQUIRE(*it == 2);
}

TEST_CASE("Pre-decrement moves iterator back and returns self", "[iterator]") {
  int values[] = {1, 2, 3};
  core::container::Iterator it(values + 2);

  auto& moved = --it;

  REQUIRE(&moved == &it);
  REQUIRE(*it == 2);
}

TEST_CASE("Post-decrement returns previous iterator and moves back", "[iterator]") {
  int values[] = {1, 2, 3};
  core::container::Iterator it(values + 2);

  const auto previous = it--;

  REQUIRE(*previous == 3);
  REQUIRE(*it == 2);
}

TEST_CASE("Equality and inequality compare underlying pointers", "[iterator]") {
  int values[] = {1, 2, 3};
  core::container::Iterator first(values);
  core::container::Iterator same(values);
  core::container::Iterator other(values + 1);

  REQUIRE(first == same);
  REQUIRE_FALSE(first != same);
  REQUIRE_FALSE(first == other);
  REQUIRE(first != other);
}

TEST_CASE("Iterator supports random access operations", "[iterator]") {
  int values[] = {4, 5, 6, 7};
  core::container::Iterator first(values);

  const auto third = first + 2;

  REQUIRE(*third == 6);
  REQUIRE(third - first == 2);
  REQUIRE(third[1] == 7);
  REQUIRE(first < third);
  REQUIRE(third > first);
}

TEST_CASE("Mutable iterator converts to const iterator", "[iterator]") {
  int values[] = {8, 9};
  core::container::Iterator mutableIt(values);
  core::container::Iterator<const int> constIt(mutableIt);

  REQUIRE(*constIt == 8);
  REQUIRE(constIt == mutableIt);
}

