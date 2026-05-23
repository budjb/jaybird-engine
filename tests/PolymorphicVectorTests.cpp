#include <array>
#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

#include "container/Container.hpp"
#include "container/RawIterator.hpp"
#include "container/RawVector.hpp"

namespace {
struct Point {
  int x = 0;
  int y = 0;

  Point() = default;
  Point(const int x, const int y) : x(x), y(y) {}

  friend bool operator==(const Point&, const Point&) = default;
};

template <typename T>
class TestVector : public core::container::RawVector {
 public:
  TestVector() : RawVector(sizeof(T), alignof(T)) {}

 protected:
  void constructAt(void* destination) noexcept override {
    new (destination) T();
  }

  void constructAt(void* destination, void* source) noexcept override {
    new (destination) T(std::move_if_noexcept(*static_cast<T*>(source)));
  }

  void copyConstructAt(void* destination, const void* source) override {
    new (destination) T(*static_cast<const T*>(source));
  }

  void destroyAt(void* element) noexcept override {
    static_cast<T*>(element)->~T();
  }
};
}  // namespace

using namespace core::container;

// ---------------------------------------------------------------------------
// RawIterator
// ---------------------------------------------------------------------------

TEST_CASE(
    "Given a RawIterator over integer storage, when pointer and arithmetic APIs are used, then offsets and "
    "dereferences are correct",
    "[raw_iterator]") {
  int arr[4] = {10, 20, 30, 40};
  RawIterator it(arr, sizeof(int));

  REQUIRE(*static_cast<int*>(it.data()) == 10);
  REQUIRE(*static_cast<int*>(it.at(2)) == 30);

  auto old = it++;
  REQUIRE(*static_cast<int*>(old.data()) == 10);
  REQUIRE(*static_cast<int*>(it.data()) == 20);

  it += 2;
  REQUIRE(*static_cast<int*>(it.data()) == 40);
  REQUIRE((it - RawIterator(arr, sizeof(int))) == 3);
}

// ---------------------------------------------------------------------------
// RawVector<int> core behavior
// ---------------------------------------------------------------------------

TEST_CASE(
    "Given a newly created RawVector<int>, when queried, then it reports empty state and correct element metadata",
    "[raw_vector]") {
  const TestVector<int> values;

  REQUIRE(values.empty());
  REQUIRE(values.capacity() == 0);
  REQUIRE(values.elementSize() == sizeof(int));
}

TEST_CASE(
    "Given a RawVector<int>, when values are pushed and accessed, then front back index and data pointers are valid",
    "[raw_vector]") {
  TestVector<int> values;
  constexpr int a = 7;
  constexpr int b = 9;

  values.pushBack(&a);
  values.pushBack(&b);

  REQUIRE(values.size() == 2);
  REQUIRE(*static_cast<int*>(values.front()) == 7);
  REQUIRE(*static_cast<int*>(values.back()) == 9);
  REQUIRE(*static_cast<int*>(values.at(0)) == 7);
  REQUIRE(*static_cast<int*>(values[1]) == 9);
  REQUIRE(values.data() != nullptr);
}

TEST_CASE("Given a RawVector<int>, when pushBack receives a null source, then invalid_argument is thrown",
          "[raw_vector]") {
  TestVector<int> values;
  REQUIRE_THROWS_AS(values.pushBack(nullptr), std::invalid_argument);
}

TEST_CASE(
    "Given a RawVector<int>, when out-of-range access and empty pop operations are requested, then bounds errors are "
    "reported",
    "[raw_vector]") {
  TestVector<int> values;
  constexpr int a = 1;
  values.pushBack(&a);

  REQUIRE_THROWS_AS(values.at(1), std::out_of_range);
  values.popBack();
  REQUIRE(values.empty());
  REQUIRE_THROWS_AS(values.popBack(), std::out_of_range);
  REQUIRE_THROWS_AS(values.back(), std::out_of_range);
}

TEST_CASE(
    "Given a RawVector<int>, when iterator endpoints are requested, then begin end rbegin and rend map to expected "
    "addresses",
    "[raw_vector]") {
  TestVector<int> values;
  constexpr int a = 1;
  constexpr int b = 2;
  values.pushBack(&a);
  values.pushBack(&b);

  REQUIRE(values.begin().data() == values.data());
  REQUIRE(values.end().data() == static_cast<std::byte*>(values.data()) + 2 * sizeof(int));
  REQUIRE(*static_cast<int*>(values.rbegin().data()) == 2);
  REQUIRE(values.rend().base().data() == values.begin().data());
}

TEST_CASE(
    "Given a RawVector<int>, when reserve resize shrinkToFit and clear are called, then capacity and size transitions "
    "are correct",
    "[raw_vector]") {
  TestVector<int> values;
  constexpr int a = 3;
  constexpr int b = 4;
  values.pushBack(&a);
  values.pushBack(&b);

  values.reserve(16);
  REQUIRE(values.capacity() >= 16);

  values.resize(4);
  REQUIRE(values.size() == 4);
  REQUIRE(*static_cast<int*>(values[0]) == 3);
  REQUIRE(*static_cast<int*>(values[1]) == 4);
  REQUIRE(*static_cast<int*>(values[2]) == 0);
  REQUIRE(*static_cast<int*>(values[3]) == 0);

  values.shrinkToFit();
  REQUIRE(values.capacity() == values.size());

  values.clear();
  REQUIRE(values.empty());
}

TEST_CASE(
    "Given a RawVector<int>, when assign count and assign range overloads are used, then contents are replaced with "
    "provided sequence",
    "[raw_vector]") {
  TestVector<int> values;
  constexpr int seed = 99;
  values.pushBack(&seed);

  constexpr int v = 5;
  values.assign(3, &v);
  REQUIRE(values.size() == 3);
  REQUIRE(*static_cast<int*>(values[0]) == 5);
  REQUIRE(*static_cast<int*>(values[2]) == 5);

  constexpr std::array src = {1, 2, 3, 4};
  values.assign(src.data(), src.data() + src.size());
  REQUIRE(values.size() == 4);
  REQUIRE(*static_cast<int*>(values[0]) == 1);
  REQUIRE(*static_cast<int*>(values[3]) == 4);
}

TEST_CASE("Given a RawVector<int>, when assign receives invalid pointers, then invalid_argument is thrown",
          "[raw_vector]") {
  TestVector<int> values;
  constexpr int src[2] = {1, 2};

  REQUIRE_THROWS_AS(values.assign(2, nullptr), std::invalid_argument);
  REQUIRE_THROWS_AS(values.assign(src, nullptr), std::invalid_argument);
  REQUIRE_THROWS_AS(values.assign(nullptr, src), std::invalid_argument);
}

TEST_CASE(
    "Given a RawVector<int>, when single count and range insert overloads are applied, then elements are inserted in "
    "the right order",
    "[raw_vector]") {
  TestVector<int> values;
  constexpr int a = 1;
  constexpr int c = 3;
  values.pushBack(&a);
  values.pushBack(&c);

  constexpr int b = 2;
  values.insert(values.cbegin() + 1, &b);

  constexpr int z = 0;
  values.insert(values.cbegin(), 2, &z);

  constexpr int tail[2] = {4, 5};
  values.insert(values.cend(), tail, tail + 2);

  REQUIRE(values.size() == 7);
  REQUIRE(*static_cast<int*>(values[0]) == 0);
  REQUIRE(*static_cast<int*>(values[1]) == 0);
  REQUIRE(*static_cast<int*>(values[2]) == 1);
  REQUIRE(*static_cast<int*>(values[3]) == 2);
  REQUIRE(*static_cast<int*>(values[4]) == 3);
  REQUIRE(*static_cast<int*>(values[5]) == 4);
  REQUIRE(*static_cast<int*>(values[6]) == 5);
}

TEST_CASE(
    "Given a RawVector<int>, when single and range erase operations are called, then targeted elements are removed",
    "[raw_vector]") {
  TestVector<int> values;
  const int data[5] = {1, 2, 3, 4, 5};
  values.assign(data, data + 5);

  values.erase(values[1]);             // remove 2
  values.erase(values[2], values[3]);  // remove 4

  REQUIRE(values.size() == 3);
  REQUIRE(*static_cast<int*>(values[0]) == 1);
  REQUIRE(*static_cast<int*>(values[1]) == 3);
  REQUIRE(*static_cast<int*>(values[2]) == 5);
}

TEST_CASE(
    "Given a RawVector<int>, when emplaceBack and emplace callbacks construct elements, then inserted values match "
    "callback writes",
    "[raw_vector]") {
  TestVector<int> values;

  values.emplaceBack([](void* dest) { *static_cast<int*>(dest) = 10; });
  values.emplace(values.end().data(), [](void* dest) { *static_cast<int*>(dest) = 30; });
  values.emplace(values[1], [](void* dest) { *static_cast<int*>(dest) = 20; });

  REQUIRE(values.size() == 3);
  REQUIRE(*static_cast<int*>(values[0]) == 10);
  REQUIRE(*static_cast<int*>(values[1]) == 20);
  REQUIRE(*static_cast<int*>(values[2]) == 30);
}

TEST_CASE("Given a RawVector<int>, when emplace APIs receive a null callback, then invalid_argument is thrown",
          "[raw_vector]") {
  TestVector<int> values;
  const std::function<void(void*)> nullCtor;

  REQUIRE_THROWS_AS(values.emplaceBack(nullCtor), std::invalid_argument);
  values.resize(1);
  REQUIRE_THROWS_AS(values.emplace(values[0], nullCtor), std::invalid_argument);
}

// ---------------------------------------------------------------------------
// RawVector<Point> non-trivial element coverage
// ---------------------------------------------------------------------------

TEST_CASE(
    "Given a RawVector<Point>, when non-trivial elements are pushed resized and erased, then object values remain "
    "correct",
    "[raw_vector]") {
  TestVector<Point> values;
  const Point a{1, 2};
  const Point b{3, 4};

  values.pushBack(&a);
  values.pushBack(&b);

  REQUIRE(*static_cast<Point*>(values[0]) == Point(1, 2));
  REQUIRE(*static_cast<Point*>(values[1]) == Point(3, 4));

  values.resize(3);
  REQUIRE(*static_cast<Point*>(values[2]) == Point(0, 0));

  values.erase(values[0]);
  REQUIRE(values.size() == 2);
  REQUIRE(*static_cast<Point*>(values[0]) == Point(3, 4));
}
