#include <catch2/catch_test_macros.hpp>

#include "container/Iterator.hpp"

namespace {
struct Item {
  int value;
};
}  // namespace

TEST_CASE("Given an Iterator pointing at an array, when dereferenced, then the first element is returned",
          "[container][iterator]") {
  int values[] = {10, 20, 30};
  const core::container::Iterator it(values);

  REQUIRE(*it == 10);
}

TEST_CASE("Given an Iterator over structs, when arrow operator is used, then the current member is accessible",
          "[container][iterator]") {
  Item values[] = {{7}, {9}};
  const core::container::Iterator it(values);

  REQUIRE(it->value == 7);
}

TEST_CASE(
    "Given an Iterator, when pre-incremented, then it points to the next element and the iterator itself is returned",
    "[container][iterator]") {
  int values[] = {1, 2, 3};
  core::container::Iterator it(values);

  auto& advanced = ++it;

  REQUIRE(&advanced == &it);
  REQUIRE(*it == 2);
}

TEST_CASE("Given an Iterator, when post-incremented, then the old position is returned and the iterator advances",
          "[container][iterator]") {
  int values[] = {1, 2, 3};
  core::container::Iterator it(values);

  const auto previous = it++;

  REQUIRE(*previous == 1);
  REQUIRE(*it == 2);
}

TEST_CASE(
    "Given an Iterator past the first element, when pre-decremented, then it points to the previous element and "
    "the iterator itself is returned",
    "[container][iterator]") {
  int values[] = {1, 2, 3};
  core::container::Iterator it(values + 2);

  auto& moved = --it;

  REQUIRE(&moved == &it);
  REQUIRE(*it == 2);
}

TEST_CASE(
    "Given an Iterator not at the front, when post-decremented, then the old position is returned and the iterator "
    "moves back",
    "[container][iterator]") {
  int values[] = {1, 2, 3};
  core::container::Iterator it(values + 2);

  const auto previous = it--;

  REQUIRE(*previous == 3);
  REQUIRE(*it == 2);
}

TEST_CASE(
    "Given two Iterators at the same and different positions, when compared, then equality reflects pointer identity",
    "[container][iterator]") {
  int values[] = {1, 2, 3};
  core::container::Iterator first(values);
  core::container::Iterator same(values);
  core::container::Iterator other(values + 1);

  REQUIRE(first == same);
  REQUIRE_FALSE(first != same);
  REQUIRE_FALSE(first == other);
  REQUIRE(first != other);
}

TEST_CASE(
    "Given an Iterator, when random access operators are applied, then elements are reached at the correct offset",
    "[container][iterator]") {
  int values[] = {4, 5, 6, 7};
  core::container::Iterator first(values);

  const auto third = first + 2;

  REQUIRE(*third == 6);
  REQUIRE(third - first == 2);
  REQUIRE(third[1] == 7);
  REQUIRE(first < third);
  REQUIRE(third > first);
}

TEST_CASE("Given a mutable Iterator, when converted to a const Iterator, then both point to the same element",
          "[container][iterator]") {
  int values[] = {8, 9};
  core::container::Iterator mutableIt(values);
  core::container::Iterator<const int> constIt(mutableIt);

  REQUIRE(*constIt == 8);
  REQUIRE(constIt == mutableIt);
}
