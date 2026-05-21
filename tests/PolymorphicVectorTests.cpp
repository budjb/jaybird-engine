#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

#include "container/PolymorphicIterator.hpp"
#include "container/PolymorphicVector.hpp"
#include "container/Vector.hpp"

namespace {
struct Point {
  int x = 0;
  int y = 0;

  Point() = default;
  Point(const int x, const int y) : x(x), y(y) {}

  friend bool operator==(const Point&, const Point&) = default;
};
}  // namespace

using namespace core::container;

// ---------------------------------------------------------------------------
// PolymorphicIterator
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicIterator data() returns the pointer it was constructed with", "[polymorphic_iterator]") {
  int value = 42;
  PolymorphicIterator it(&value, sizeof(int));
  REQUIRE(it.data() == &value);
}

TEST_CASE("PolymorphicIterator const data() returns the same pointer", "[polymorphic_iterator]") {
  int value = 99;
  const PolymorphicIterator it(&value, sizeof(int));
  REQUIRE(it.data() == static_cast<const void*>(&value));
}

TEST_CASE("PolymorphicIterator at(0) returns the base pointer", "[polymorphic_iterator]") {
  int arr[3] = {10, 20, 30};
  PolymorphicIterator it(arr, sizeof(int));
  REQUIRE(it.at(0) == static_cast<void*>(&arr[0]));
}

TEST_CASE("PolymorphicIterator at(n) advances by n * elementSize bytes", "[polymorphic_iterator]") {
  int arr[3] = {10, 20, 30};
  PolymorphicIterator it(arr, sizeof(int));

  REQUIRE(*static_cast<int*>(it.at(0)) == 10);
  REQUIRE(*static_cast<int*>(it.at(1)) == 20);
  REQUIRE(*static_cast<int*>(it.at(2)) == 30);
}

TEST_CASE("PolymorphicIterator at(n) works with larger element types", "[polymorphic_iterator]") {
  Point pts[2] = {{1, 2}, {3, 4}};
  PolymorphicIterator it(pts, sizeof(Point));

  REQUIRE(*static_cast<Point*>(it.at(0)) == Point(1, 2));
  REQUIRE(*static_cast<Point*>(it.at(1)) == Point(3, 4));
}

TEST_CASE("PolymorphicIterator const at(n) returns correct pointer", "[polymorphic_iterator]") {
  int arr[2] = {5, 6};
  const PolymorphicIterator it(arr, sizeof(int));
  REQUIRE(*static_cast<const int*>(it.at(1)) == 6);
}

// ---------------------------------------------------------------------------
// PolymorphicVector — construction and metadata
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicVector reflects size and capacity of the underlying Vector", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);
  vec.pushBack(3);

  const PolymorphicVector pv(vec);

  REQUIRE(pv.size() == 3);
  REQUIRE(pv.capacity() >= 3);
}

TEST_CASE("PolymorphicVector empty() mirrors the underlying Vector", "[polymorphic_vector]") {
  Vector<int> vec;
  const PolymorphicVector pv(vec);

  REQUIRE(pv.empty());

  vec.pushBack(1);
  REQUIRE_FALSE(pv.empty());
}

TEST_CASE("PolymorphicVector data() returns the same raw pointer as Vector::data()", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(10);

  PolymorphicVector pv(vec);
  REQUIRE(pv.data() == static_cast<void*>(vec.data()));
}

TEST_CASE("PolymorphicVector const data() returns the same raw pointer", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(7);

  const PolymorphicVector pv(vec);
  REQUIRE(pv.data() == static_cast<const void*>(vec.data()));
}

// ---------------------------------------------------------------------------
// PolymorphicVector — element access
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicVector operator[] yields correct elements for int", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(10);
  vec.pushBack(20);
  vec.pushBack(30);

  PolymorphicVector pv(vec);
  REQUIRE(*static_cast<int*>(pv[0]) == 10);
  REQUIRE(*static_cast<int*>(pv[1]) == 20);
  REQUIRE(*static_cast<int*>(pv[2]) == 30);
}

TEST_CASE("PolymorphicVector operator[] yields correct elements for Point", "[polymorphic_vector]") {
  Vector<Point> vec;
  vec.pushBack(Point{1, 2});
  vec.pushBack(Point{3, 4});

  PolymorphicVector pv(vec);
  REQUIRE(*static_cast<Point*>(pv[0]) == Point(1, 2));
  REQUIRE(*static_cast<Point*>(pv[1]) == Point(3, 4));
}

TEST_CASE("PolymorphicVector at() yields same result as operator[] for valid indices", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(5);
  vec.pushBack(15);

  PolymorphicVector pv(vec);
  REQUIRE(pv.at(0) == pv[0]);
  REQUIRE(pv.at(1) == pv[1]);
}

TEST_CASE("PolymorphicVector at() throws on out-of-bounds index", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);

  PolymorphicVector pv(vec);
  REQUIRE_THROWS_AS(pv.at(1), std::out_of_range);
  REQUIRE_THROWS_AS(pv.at(100), std::out_of_range);
}

TEST_CASE("PolymorphicVector const at() throws on out-of-bounds index", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);

  const PolymorphicVector pv(vec);
  REQUIRE_THROWS_AS(pv.at(1), std::out_of_range);
}

TEST_CASE("PolymorphicVector operator[] allows writes that are visible through the original Vector",
          "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(0);

  PolymorphicVector pv(vec);
  *static_cast<int*>(pv[0]) = 99;

  REQUIRE(vec[0] == 99);
}

TEST_CASE("PolymorphicVector at() allows writes that are visible through the original Vector", "[polymorphic_vector]") {
  Vector<Point> vec;
  vec.pushBack(Point{0, 0});

  PolymorphicVector pv(vec);
  static_cast<Point*>(pv.at(0))->x = 7;
  static_cast<Point*>(pv.at(0))->y = 8;

  REQUIRE(vec[0] == Point(7, 8));
}

// ---------------------------------------------------------------------------
// PolymorphicVector — iterators
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicVector begin() points at the first element", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(42);
  vec.pushBack(43);

  const PolymorphicVector pv(vec);
  REQUIRE(pv.begin().data() == static_cast<void*>(vec.data()));
}

TEST_CASE("PolymorphicVector end() points one-past-the-last element", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);

  const PolymorphicVector pv(vec);
  void* expected = static_cast<std::byte*>(static_cast<void*>(vec.data())) + 2 * sizeof(int);
  REQUIRE(pv.end().data() == expected);
}

TEST_CASE("PolymorphicVector begin() == end() when empty", "[polymorphic_vector]") {
  Vector<int> vec;
  const PolymorphicVector pv(vec);
  REQUIRE(pv.begin().data() == pv.end().data());
}

TEST_CASE("PolymorphicVector back() points at the last element", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(10);
  vec.pushBack(20);
  vec.pushBack(30);

  PolymorphicVector pv(vec);
  REQUIRE(*static_cast<int*>(pv.back()) == 30);
}

TEST_CASE("PolymorphicVector back() throws when empty", "[polymorphic_vector]") {
  Vector<int> vec;
  const PolymorphicVector pv(vec);
  REQUIRE_THROWS_AS(pv.back(), std::out_of_range);
}

TEST_CASE("PolymorphicVector cbegin() points at the first element", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(3);
  vec.pushBack(4);

  const PolymorphicVector pv(vec);
  REQUIRE(pv.cbegin().data() == static_cast<const void*>(vec.data()));
}

TEST_CASE("PolymorphicVector cend() points one-past-the-last element", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(3);
  vec.pushBack(4);

  const PolymorphicVector pv(vec);
  const void* expected = static_cast<const std::byte*>(static_cast<const void*>(vec.data())) + 2 * sizeof(int);
  REQUIRE(pv.cend().data() == expected);
}

TEST_CASE("PolymorphicVector rbegin() points at the last element", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(11);
  vec.pushBack(22);

  PolymorphicVector pv(vec);
  REQUIRE(*static_cast<int*>(pv.rbegin().data()) == 22);
}

TEST_CASE("PolymorphicVector rend() base() points at the first element", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(11);
  vec.pushBack(22);

  PolymorphicVector pv(vec);
  REQUIRE(pv.rend().base().data() == pv.begin().data());
}

TEST_CASE("PolymorphicVector crbegin() and crend() expose const reverse endpoints", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(7);
  vec.pushBack(9);

  const PolymorphicVector pv(vec);
  REQUIRE(*static_cast<const int*>(pv.crbegin().data()) == 9);
  REQUIRE(pv.crend().base().data() == pv.cbegin().data());
}

TEST_CASE("PolymorphicVector reverse endpoints compare equal when empty", "[polymorphic_vector]") {
  Vector<int> vec;
  const PolymorphicVector pv(vec);
  REQUIRE(pv.rbegin() == pv.rend());
  REQUIRE(pv.crbegin() == pv.crend());
}

TEST_CASE("PolymorphicVector begin().at(n) traverses elements correctly for int", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(10);
  vec.pushBack(20);
  vec.pushBack(30);

  const PolymorphicVector pv(vec);
  PolymorphicIterator it = pv.begin();

  REQUIRE(*static_cast<int*>(it.at(0)) == 10);
  REQUIRE(*static_cast<int*>(it.at(1)) == 20);
  REQUIRE(*static_cast<int*>(it.at(2)) == 30);
}

TEST_CASE("PolymorphicVector begin().at(n) traverses elements correctly for Point", "[polymorphic_vector]") {
  Vector<Point> vec;
  vec.pushBack(Point{1, 2});
  vec.pushBack(Point{3, 4});
  vec.pushBack(Point{5, 6});

  const PolymorphicVector pv(vec);
  PolymorphicIterator it = pv.begin();

  REQUIRE(*static_cast<Point*>(it.at(0)) == Point(1, 2));
  REQUIRE(*static_cast<Point*>(it.at(1)) == Point(3, 4));
  REQUIRE(*static_cast<Point*>(it.at(2)) == Point(5, 6));
}

// ---------------------------------------------------------------------------
// PolymorphicVector — resize
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicVector resize() growing default-constructs new elements", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);

  PolymorphicVector pv(vec);
  pv.resize(3);

  REQUIRE(pv.size() == 3);
  REQUIRE(vec.size() == 3);
  REQUIRE(*static_cast<int*>(pv[0]) == 1);
  REQUIRE(*static_cast<int*>(pv[1]) == 0);
  REQUIRE(*static_cast<int*>(pv[2]) == 0);
}

TEST_CASE("PolymorphicVector resize() shrinking destroys excess elements", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);
  vec.pushBack(3);

  PolymorphicVector pv(vec);
  pv.resize(1);

  REQUIRE(pv.size() == 1);
  REQUIRE(vec.size() == 1);
  REQUIRE(*static_cast<int*>(pv[0]) == 1);
}

TEST_CASE("PolymorphicVector resize() to same size is a no-op", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(5);
  vec.pushBack(6);

  PolymorphicVector pv(vec);
  pv.resize(2);

  REQUIRE(pv.size() == 2);
  REQUIRE(*static_cast<int*>(pv[0]) == 5);
  REQUIRE(*static_cast<int*>(pv[1]) == 6);
}

TEST_CASE("PolymorphicVector resize() to zero empties the vector", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);

  PolymorphicVector pv(vec);
  pv.resize(0);

  REQUIRE(pv.empty());
  REQUIRE(vec.empty());
}

TEST_CASE("PolymorphicVector resize() works for non-trivial types", "[polymorphic_vector]") {
  Vector<Point> vec;
  vec.pushBack(Point{9, 8});

  PolymorphicVector pv(vec);
  pv.resize(3);

  REQUIRE(pv.size() == 3);
  REQUIRE(*static_cast<Point*>(pv[0]) == Point(9, 8));
  REQUIRE(*static_cast<Point*>(pv[1]) == Point(0, 0));
  REQUIRE(*static_cast<Point*>(pv[2]) == Point(0, 0));
}

// ---------------------------------------------------------------------------
// PolymorphicVector — IVector& reference semantics
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicVector reflects live mutations made through the original Vector", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);

  PolymorphicVector pv(vec);

  vec.pushBack(3);
  REQUIRE(pv.size() == 3);
  REQUIRE(*static_cast<int*>(pv[2]) == 3);

  vec.popBack();
  REQUIRE(pv.size() == 2);
}

TEST_CASE("PolymorphicVector wrapping the same IVector as two views stays consistent", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(10);
  vec.pushBack(20);

  PolymorphicVector pv1(vec);
  PolymorphicVector pv2(vec);

  *static_cast<int*>(pv1[0]) = 100;

  REQUIRE(*static_cast<int*>(pv2[0]) == 100);
}

// ---------------------------------------------------------------------------
// PolymorphicVector — additional methods mirroring Vector API
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicVector maxSize() returns the allocator's max_size", "[polymorphic_vector]") {
  Vector<int> vec;
  const PolymorphicVector pv(vec);
  REQUIRE(pv.maxSize() > 0);
}

TEST_CASE("PolymorphicVector elementSize() returns correct element size for int", "[polymorphic_vector]") {
  Vector<int> vec;
  const PolymorphicVector pv(vec);
  REQUIRE(pv.elementSize() == sizeof(int));
}

TEST_CASE("PolymorphicVector elementSize() returns correct element size for Point", "[polymorphic_vector]") {
  Vector<Point> vec;
  const PolymorphicVector pv(vec);
  REQUIRE(pv.elementSize() == sizeof(Point));
}

TEST_CASE("PolymorphicVector reserve() increases capacity", "[polymorphic_vector]") {
  Vector<int> vec;
  PolymorphicVector pv(vec);

  pv.reserve(100);
  REQUIRE(pv.capacity() >= 100);
}

TEST_CASE("PolymorphicVector reserve() doesn't affect size", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);

  PolymorphicVector pv(vec);
  pv.reserve(50);

  REQUIRE(pv.size() == 2);
  REQUIRE(*static_cast<int*>(pv[0]) == 1);
  REQUIRE(*static_cast<int*>(pv[1]) == 2);
}

TEST_CASE("PolymorphicVector shrinkToFit() reduces capacity to size", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.reserve(100);
  vec.pushBack(1);
  vec.pushBack(2);

  PolymorphicVector pv(vec);
  REQUIRE(pv.capacity() > pv.size());

  pv.shrinkToFit();
  REQUIRE(pv.capacity() == pv.size());
}

TEST_CASE("PolymorphicVector clear() empties the vector", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);
  vec.pushBack(3);

  PolymorphicVector pv(vec);
  REQUIRE_FALSE(pv.empty());

  pv.clear();
  REQUIRE(pv.empty());
  REQUIRE(pv.capacity() > 0);
}

TEST_CASE("PolymorphicVector pushBack() appends a copied element and returns its address", "[polymorphic_vector]") {
  Vector<int> vec;
  PolymorphicVector pv(vec);
  const int value = 25;

  void* inserted = pv.pushBack(&value);

  REQUIRE(pv.size() == 1);
  REQUIRE(inserted == pv.back());
  REQUIRE(*static_cast<int*>(inserted) == 25);
}

TEST_CASE("PolymorphicVector pushBack() copies non-trivial values", "[polymorphic_vector]") {
  Vector<Point> vec;
  PolymorphicVector pv(vec);
  const Point value{4, 5};

  void* inserted = pv.pushBack(&value);

  REQUIRE(*static_cast<Point*>(inserted) == Point(4, 5));
  REQUIRE(vec[0] == Point(4, 5));
}

TEST_CASE("PolymorphicVector pushBack() rejects null source pointers", "[polymorphic_vector]") {
  Vector<int> vec;
  PolymorphicVector pv(vec);
  REQUIRE_THROWS_AS(pv.pushBack(nullptr), std::invalid_argument);
}

TEST_CASE("PolymorphicVector popBack() removes the last element", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);
  vec.pushBack(3);

  PolymorphicVector pv(vec);
  pv.popBack();

  REQUIRE(pv.size() == 2);
  REQUIRE(*static_cast<int*>(pv.back()) == 2);
}

TEST_CASE("PolymorphicVector popBack() throws when empty", "[polymorphic_vector]") {
  Vector<int> vec;
  PolymorphicVector pv(vec);
  REQUIRE_THROWS_AS(pv.popBack(), std::out_of_range);
}

TEST_CASE("PolymorphicVector insert() inserts a copied element at the requested position", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(10);
  vec.pushBack(30);

  PolymorphicVector pv(vec);
  const int value = 20;

  const auto position = pv.cbegin() + 1;
  void* inserted = pv.insert(position, &value);

  REQUIRE(inserted == pv[1]);
  REQUIRE(*static_cast<int*>(pv[0]) == 10);
  REQUIRE(*static_cast<int*>(pv[1]) == 20);
  REQUIRE(*static_cast<int*>(pv[2]) == 30);
}

TEST_CASE("PolymorphicVector insert(count) inserts repeated copies", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(4);

  PolymorphicVector pv(vec);
  const int value = 2;

  const auto position = pv.cbegin() + 1;
  void* inserted = pv.insert(position, 2, &value);

  REQUIRE(inserted == pv[1]);
  REQUIRE(pv.size() == 4);
  REQUIRE(*static_cast<int*>(pv[0]) == 1);
  REQUIRE(*static_cast<int*>(pv[1]) == 2);
  REQUIRE(*static_cast<int*>(pv[2]) == 2);
  REQUIRE(*static_cast<int*>(pv[3]) == 4);
}

TEST_CASE("PolymorphicVector insert() rejects invalid positions and null pointers", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);

  PolymorphicVector pv(vec);
  constexpr int value = 2;
  const void* invalid = static_cast<const std::byte*>(pv.cend().data()) + 1;

  REQUIRE_THROWS_AS(pv.insert(invalid, &value), std::out_of_range);
  REQUIRE_THROWS_AS(pv.insert(pv.cend(), nullptr), std::invalid_argument);
}

TEST_CASE("PolymorphicVector erase(position) removes one element and returns the next address",
          "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);
  vec.pushBack(3);

  PolymorphicVector pv(vec);
  void* next = pv.erase(pv[1]);

  REQUIRE(next == pv[1]);
  REQUIRE(pv.size() == 2);
  REQUIRE(*static_cast<int*>(pv[0]) == 1);
  REQUIRE(*static_cast<int*>(pv[1]) == 3);
}

TEST_CASE("PolymorphicVector erase(range) removes a span and returns the following address", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);
  vec.pushBack(3);
  vec.pushBack(4);

  PolymorphicVector pv(vec);
  void* next = pv.erase(pv[1], pv[3]);

  REQUIRE(next == pv[1]);
  REQUIRE(pv.size() == 2);
  REQUIRE(*static_cast<int*>(pv[0]) == 1);
  REQUIRE(*static_cast<int*>(pv[1]) == 4);
}

TEST_CASE("PolymorphicVector erase() returns end when erasing the last element", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);

  PolymorphicVector pv(vec);
  void* next = pv.erase(pv[1]);

  REQUIRE(next == pv.end().data());
  REQUIRE(pv.size() == 1);
}

TEST_CASE("PolymorphicVector erase() rejects invalid positions", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);

  PolymorphicVector pv(vec);
  const void* invalid = static_cast<const std::byte*>(pv.cend().data()) + 1;

  REQUIRE_THROWS_AS(pv.erase(pv.cend()), std::out_of_range);
  REQUIRE_THROWS_AS(pv.erase(invalid), std::out_of_range);
  REQUIRE_THROWS_AS(pv.erase(pv[1], pv[0]), std::out_of_range);
}

TEST_CASE("PolymorphicVector erase() supports non-trivial element types", "[polymorphic_vector]") {
  Vector<Point> vec;
  vec.pushBack(Point{1, 1});
  vec.pushBack(Point{2, 2});
  vec.pushBack(Point{3, 3});

  PolymorphicVector pv(vec);
  pv.erase(pv[1]);

  REQUIRE(pv.size() == 2);
  REQUIRE(*static_cast<Point*>(pv[0]) == Point(1, 1));
  REQUIRE(*static_cast<Point*>(pv[1]) == Point(3, 3));
}

TEST_CASE("PolymorphicVector operations maintain consistency through the wrapper", "[polymorphic_vector]") {
  Vector<int> vec;

  PolymorphicVector pv(vec);

  vec.pushBack(100);
  REQUIRE(pv.size() == 1);

  pv.clear();
  REQUIRE(vec.empty());

  vec.pushBack(1);
  vec.pushBack(2);
  pv.resize(1);
  REQUIRE(vec.size() == 1);
}

// ---------------------------------------------------------------------------
// PolymorphicIterator — arithmetic operations
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicIterator pre-increment advances the iterator", "[polymorphic_iterator]") {
  int arr[3] = {10, 20, 30};
  PolymorphicIterator it(arr, sizeof(int));

  ++it;
  REQUIRE(*static_cast<int*>(it.data()) == 20);
}

TEST_CASE("PolymorphicIterator post-increment returns old iterator", "[polymorphic_iterator]") {
  int arr[3] = {10, 20, 30};
  PolymorphicIterator it(arr, sizeof(int));
  PolymorphicIterator old = it++;

  REQUIRE(*static_cast<int*>(old.data()) == 10);
  REQUIRE(*static_cast<int*>(it.data()) == 20);
}

TEST_CASE("PolymorphicIterator pre-decrement reverses the iterator", "[polymorphic_iterator]") {
  int arr[3] = {10, 20, 30};
  PolymorphicIterator it(arr + 2, sizeof(int));

  --it;
  REQUIRE(*static_cast<int*>(it.data()) == 20);
}

TEST_CASE("PolymorphicIterator operator+= advances by offset", "[polymorphic_iterator]") {
  int arr[5] = {1, 2, 3, 4, 5};
  PolymorphicIterator it(arr, sizeof(int));

  it += 2;
  REQUIRE(*static_cast<int*>(it.data()) == 3);

  it += 2;
  REQUIRE(*static_cast<int*>(it.data()) == 5);
}

TEST_CASE("PolymorphicIterator operator-= reverses by offset", "[polymorphic_iterator]") {
  int arr[5] = {1, 2, 3, 4, 5};
  PolymorphicIterator it(arr + 4, sizeof(int));

  it -= 1;
  REQUIRE(*static_cast<int*>(it.data()) == 4);

  it -= 2;
  REQUIRE(*static_cast<int*>(it.data()) == 2);
}

TEST_CASE("PolymorphicIterator operator+ creates advanced iterator", "[polymorphic_iterator]") {
  int arr[3] = {10, 20, 30};
  PolymorphicIterator it(arr, sizeof(int));

  PolymorphicIterator it2 = it + 1;
  REQUIRE(*static_cast<int*>(it2.data()) == 20);
  REQUIRE(*static_cast<int*>(it.data()) == 10);  // Original unchanged
}

TEST_CASE("PolymorphicIterator operator- creates reversed iterator", "[polymorphic_iterator]") {
  int arr[3] = {10, 20, 30};
  PolymorphicIterator it(arr + 2, sizeof(int));

  PolymorphicIterator it2 = it - 1;
  REQUIRE(*static_cast<int*>(it2.data()) == 20);
  REQUIRE(*static_cast<int*>(it.data()) == 30);  // Original unchanged
}

TEST_CASE("PolymorphicIterator difference operator calculates element distance", "[polymorphic_iterator]") {
  int arr[5] = {1, 2, 3, 4, 5};
  PolymorphicIterator it1(arr, sizeof(int));
  PolymorphicIterator it2(arr + 3, sizeof(int));

  REQUIRE(it2 - it1 == 3);
  REQUIRE(it1 - it2 == -3);
}

// ---------------------------------------------------------------------------
// PolymorphicIterator — comparison operations
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicIterator operator== compares equality", "[polymorphic_iterator]") {
  int arr[2] = {10, 20};
  PolymorphicIterator it1(arr, sizeof(int));
  PolymorphicIterator it2(arr, sizeof(int));
  PolymorphicIterator it3(arr + 1, sizeof(int));

  REQUIRE(it1 == it2);
  REQUIRE_FALSE(it1 == it3);
}

TEST_CASE("PolymorphicIterator operator!= compares inequality", "[polymorphic_iterator]") {
  int arr[2] = {10, 20};
  PolymorphicIterator it1(arr, sizeof(int));
  PolymorphicIterator it2(arr, sizeof(int));
  PolymorphicIterator it3(arr + 1, sizeof(int));

  REQUIRE_FALSE(it1 != it2);
  REQUIRE(it1 != it3);
}

TEST_CASE("PolymorphicIterator operator< compares ordering", "[polymorphic_iterator]") {
  int arr[3] = {10, 20, 30};
  PolymorphicIterator it1(arr, sizeof(int));
  PolymorphicIterator it2(arr + 1, sizeof(int));
  PolymorphicIterator it3(arr + 2, sizeof(int));

  REQUIRE(it1 < it2);
  REQUIRE(it2 < it3);
  REQUIRE_FALSE(it3 < it1);
}

TEST_CASE("PolymorphicIterator operator> compares reverse ordering", "[polymorphic_iterator]") {
  int arr[3] = {10, 20, 30};
  PolymorphicIterator it1(arr, sizeof(int));
  PolymorphicIterator it2(arr + 1, sizeof(int));

  REQUIRE(it2 > it1);
  REQUIRE_FALSE(it1 > it2);
}

TEST_CASE("PolymorphicIterator operator<= compares inclusive ordering", "[polymorphic_iterator]") {
  int arr[2] = {10, 20};
  PolymorphicIterator it1(arr, sizeof(int));
  PolymorphicIterator it2(arr, sizeof(int));
  PolymorphicIterator it3(arr + 1, sizeof(int));

  REQUIRE(it1 <= it2);
  REQUIRE(it1 <= it3);
  REQUIRE_FALSE(it3 <= it1);
}

TEST_CASE("PolymorphicIterator operator>= compares inclusive reverse ordering", "[polymorphic_iterator]") {
  int arr[2] = {10, 20};
  PolymorphicIterator it1(arr, sizeof(int));
  PolymorphicIterator it2(arr, sizeof(int));
  PolymorphicIterator it3(arr + 1, sizeof(int));

  REQUIRE(it2 >= it1);
  REQUIRE(it3 >= it1);
  REQUIRE_FALSE(it1 >= it3);
}

// ---------------------------------------------------------------------------
// PolymorphicVector — assign() variants
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicVector assign(count, value) fills vector with copies", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(999);  // Will be replaced

  PolymorphicVector pv(vec);
  constexpr int value = 42;
  pv.assign(3, &value);

  REQUIRE(pv.size() == 3);
  REQUIRE(*static_cast<int*>(pv[0]) == 42);
  REQUIRE(*static_cast<int*>(pv[1]) == 42);
  REQUIRE(*static_cast<int*>(pv[2]) == 42);
}

TEST_CASE("PolymorphicVector assign(count, value) clears existing elements", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);
  vec.pushBack(3);
  vec.pushBack(4);
  vec.pushBack(5);

  PolymorphicVector pv(vec);
  constexpr int value = 0;
  pv.assign(2, &value);

  REQUIRE(pv.size() == 2);
  REQUIRE(*static_cast<int*>(pv[0]) == 0);
  REQUIRE(*static_cast<int*>(pv[1]) == 0);
}

TEST_CASE("PolymorphicVector assign(count, value) works for Point type", "[polymorphic_vector]") {
  Vector<Point> vec;
  PolymorphicVector pv(vec);
  const Point value{7, 8};

  pv.assign(2, &value);

  REQUIRE(pv.size() == 2);
  REQUIRE(*static_cast<Point*>(pv[0]) == Point(7, 8));
  REQUIRE(*static_cast<Point*>(pv[1]) == Point(7, 8));
}

TEST_CASE("PolymorphicVector assign(count, value) rejects null value", "[polymorphic_vector]") {
  Vector<int> vec;
  PolymorphicVector pv(vec);
  REQUIRE_THROWS_AS(pv.assign(3, nullptr), std::invalid_argument);
}

TEST_CASE("PolymorphicVector assign(first, last) assigns from range for int", "[polymorphic_vector]") {
  constexpr int src[3] = {10, 20, 30};

  Vector<int> vec;
  vec.pushBack(999);  // Will be replaced

  PolymorphicVector pv(vec);
  pv.assign(src, src + 3);

  REQUIRE(pv.size() == 3);
  REQUIRE(*static_cast<int*>(pv[0]) == 10);
  REQUIRE(*static_cast<int*>(pv[1]) == 20);
  REQUIRE(*static_cast<int*>(pv[2]) == 30);
}

TEST_CASE("PolymorphicVector assign(first, last) assigns from range for Point", "[polymorphic_vector]") {
  const Point src[2] = {{1, 2}, {3, 4}};

  Vector<Point> vec;
  PolymorphicVector pv(vec);
  pv.assign(src, src + 2);

  REQUIRE(pv.size() == 2);
  REQUIRE(*static_cast<Point*>(pv[0]) == Point(1, 2));
  REQUIRE(*static_cast<Point*>(pv[1]) == Point(3, 4));
}

TEST_CASE("PolymorphicVector assign(first, last) handles empty range", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);

  PolymorphicVector pv(vec);
  pv.assign(nullptr, nullptr);

  REQUIRE(pv.empty());
}

TEST_CASE("PolymorphicVector assign(first, last) rejects mismatched pointers", "[polymorphic_vector]") {
  constexpr int src[2] = {1, 2};
  Vector<int> vec;
  PolymorphicVector pv(vec);

  REQUIRE_THROWS_AS(pv.assign(src, nullptr), std::invalid_argument);
  REQUIRE_THROWS_AS(pv.assign(nullptr, src), std::invalid_argument);
}

// ---------------------------------------------------------------------------
// PolymorphicVector — range insert()
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicVector insert(pos, first, last) inserts range for int", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(4);

  int src[2] = {2, 3};

  PolymorphicVector pv(vec);
  const void* insertPos = pv[1];
  void* result = pv.insert(insertPos, src, src + 2);

  REQUIRE(pv.size() == 4);
  REQUIRE(*static_cast<int*>(pv[0]) == 1);
  REQUIRE(*static_cast<int*>(pv[1]) == 2);
  REQUIRE(*static_cast<int*>(pv[2]) == 3);
  REQUIRE(*static_cast<int*>(pv[3]) == 4);
  REQUIRE(result == pv[1]);
}

TEST_CASE("PolymorphicVector insert(pos, first, last) inserts range for Point", "[polymorphic_vector]") {
  Vector<Point> vec;
  vec.pushBack(Point{1, 1});
  vec.pushBack(Point{4, 4});

  Point src[2] = {{2, 2}, {3, 3}};

  PolymorphicVector pv(vec);
  pv.insert(pv[1], src, src + 2);

  REQUIRE(pv.size() == 4);
  REQUIRE(*static_cast<Point*>(pv[0]) == Point(1, 1));
  REQUIRE(*static_cast<Point*>(pv[1]) == Point(2, 2));
  REQUIRE(*static_cast<Point*>(pv[2]) == Point(3, 3));
  REQUIRE(*static_cast<Point*>(pv[3]) == Point(4, 4));
}

TEST_CASE("PolymorphicVector insert(pos, first, last) returns position for empty range", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);

  PolymorphicVector pv(vec);
  void* result = pv.insert(pv[1], nullptr, nullptr);

  REQUIRE(pv.size() == 2);
  REQUIRE(result == pv[1]);
}

TEST_CASE("PolymorphicVector insert(pos, first, last) rejects mismatched pointers", "[polymorphic_vector]") {
  constexpr int src[2] = {1, 2};
  Vector<int> vec;
  vec.pushBack(1);

  PolymorphicVector pv(vec);
  REQUIRE_THROWS_AS(pv.insert(pv[0], src, nullptr), std::invalid_argument);
}

// ---------------------------------------------------------------------------
// PolymorphicVector — emplace-style functions
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicVector emplaceBack() constructs element with callback for int", "[polymorphic_vector]") {
  Vector<int> vec;
  PolymorphicVector pv(vec);

  void* result = pv.emplaceBack([](void* dest) { *static_cast<int*>(dest) = 42; });

  REQUIRE(pv.size() == 1);
  REQUIRE(*static_cast<int*>(result) == 42);
  REQUIRE(*static_cast<int*>(pv.back()) == 42);
}

TEST_CASE("PolymorphicVector emplaceBack() constructs element with callback for Point", "[polymorphic_vector]") {
  Vector<Point> vec;
  PolymorphicVector pv(vec);

  void* result = pv.emplaceBack([](void* dest) { new (dest) Point(5, 6); });

  REQUIRE(pv.size() == 1);
  REQUIRE(*static_cast<Point*>(result) == Point(5, 6));
}

TEST_CASE("PolymorphicVector emplaceBack() multiple times preserves order", "[polymorphic_vector]") {
  Vector<int> vec;
  PolymorphicVector pv(vec);

  pv.emplaceBack([](void* dest) { *static_cast<int*>(dest) = 10; });
  pv.emplaceBack([](void* dest) { *static_cast<int*>(dest) = 20; });
  pv.emplaceBack([](void* dest) { *static_cast<int*>(dest) = 30; });

  REQUIRE(pv.size() == 3);
  REQUIRE(*static_cast<int*>(pv[0]) == 10);
  REQUIRE(*static_cast<int*>(pv[1]) == 20);
  REQUIRE(*static_cast<int*>(pv[2]) == 30);
}

TEST_CASE("PolymorphicVector emplaceBack() rejects null callback", "[polymorphic_vector]") {
  Vector<int> vec;
  PolymorphicVector pv(vec);

  const std::function<void(void*)> nullCtor;
  REQUIRE_THROWS_AS(pv.emplaceBack(nullCtor), std::invalid_argument);
}

TEST_CASE("PolymorphicVector emplace() constructs element at position for int", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(10);
  vec.pushBack(30);

  PolymorphicVector pv(vec);
  void* result = pv.emplace(pv[1], [](void* dest) { *static_cast<int*>(dest) = 20; });

  REQUIRE(pv.size() == 3);
  REQUIRE(*static_cast<int*>(pv[0]) == 10);
  REQUIRE(*static_cast<int*>(pv[1]) == 20);
  REQUIRE(*static_cast<int*>(pv[2]) == 30);
  REQUIRE(result == pv[1]);
}

TEST_CASE("PolymorphicVector emplace() constructs element at end", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);
  vec.pushBack(2);

  PolymorphicVector pv(vec);
  pv.emplace(pv.end().data(), [](void* dest) { *static_cast<int*>(dest) = 3; });

  REQUIRE(pv.size() == 3);
  REQUIRE(*static_cast<int*>(pv[2]) == 3);
}

TEST_CASE("PolymorphicVector emplace() constructs element at beginning", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(2);
  vec.pushBack(3);

  PolymorphicVector pv(vec);
  pv.emplace(pv.begin().data(), [](void* dest) { *static_cast<int*>(dest) = 1; });

  REQUIRE(pv.size() == 3);
  REQUIRE(*static_cast<int*>(pv[0]) == 1);
  REQUIRE(*static_cast<int*>(pv[1]) == 2);
  REQUIRE(*static_cast<int*>(pv[2]) == 3);
}

TEST_CASE("PolymorphicVector emplace() rejects null callback", "[polymorphic_vector]") {
  Vector<int> vec;
  vec.pushBack(1);

   PolymorphicVector pv(vec);
   const std::function<void(void*)> nullCtor;

   REQUIRE_THROWS_AS(pv.emplace(pv[0], nullCtor), std::invalid_argument);
}

// ---------------------------------------------------------------------------
// PolymorphicReverseIterator Tests
// ---------------------------------------------------------------------------

TEST_CASE("PolymorphicReverseIterator rbegin() points at the last element", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(11);
   vec.pushBack(22);

   PolymorphicVector pv(vec);
   auto rit = pv.rbegin();
   REQUIRE(*static_cast<int*>(rit.data()) == 22);
}

TEST_CASE("PolymorphicReverseIterator rend() points one before the first element", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(11);
   vec.pushBack(22);
   vec.pushBack(33);

   PolymorphicVector pv(vec);
   auto rit = pv.rend();
   REQUIRE(rit.base().data() == pv.begin().data());
}

TEST_CASE("PolymorphicReverseIterator pre-increment moves backward through vector", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(1);
   vec.pushBack(2);
   vec.pushBack(3);

   PolymorphicVector pv(vec);
   auto rit = pv.rbegin();

   REQUIRE(*static_cast<int*>(rit.data()) == 3);
   ++rit;
   REQUIRE(*static_cast<int*>(rit.data()) == 2);
   ++rit;
   REQUIRE(*static_cast<int*>(rit.data()) == 1);
}

TEST_CASE("PolymorphicReverseIterator post-increment returns old iterator", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(1);
   vec.pushBack(2);

   PolymorphicVector pv(vec);
   auto rit = pv.rbegin();
   auto old = rit++;

   REQUIRE(*static_cast<int*>(old.data()) == 2);
   REQUIRE(*static_cast<int*>(rit.data()) == 1);
}

TEST_CASE("PolymorphicReverseIterator pre-decrement moves forward through vector", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(1);
   vec.pushBack(2);
   vec.pushBack(3);

   PolymorphicVector pv(vec);
   auto rit = pv.rbegin();
   ++rit;  // Move to 2
   --rit;  // Back to 3
   REQUIRE(*static_cast<int*>(rit.data()) == 3);
}

TEST_CASE("PolymorphicReverseIterator += operator moves backward by offset", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(1);
   vec.pushBack(2);
   vec.pushBack(3);
   vec.pushBack(4);
   vec.pushBack(5);

   PolymorphicVector pv(vec);
   auto rit = pv.rbegin();

   rit += 2;
   REQUIRE(*static_cast<int*>(rit.data()) == 3);
}

TEST_CASE("PolymorphicReverseIterator + operator creates advanced reverse iterator", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(1);
   vec.pushBack(2);
   vec.pushBack(3);

   PolymorphicVector pv(vec);
   auto rit = pv.rbegin();

   auto rit2 = rit + 1;
   REQUIRE(*static_cast<int*>(rit.data()) == 3);   // Original unchanged
   REQUIRE(*static_cast<int*>(rit2.data()) == 2);
}

TEST_CASE("PolymorphicReverseIterator distance calculation", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(1);
   vec.pushBack(2);
   vec.pushBack(3);
   vec.pushBack(4);
   vec.pushBack(5);

   PolymorphicVector pv(vec);
   auto rit1 = pv.rbegin();
   auto rit2 = pv.rbegin() + 2;

   REQUIRE(rit2 - rit1 == 2);
   REQUIRE(rit1 - rit2 == -2);
}

TEST_CASE("PolymorphicReverseIterator comparison operators", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(1);
   vec.pushBack(2);
   vec.pushBack(3);

   PolymorphicVector pv(vec);
   auto rit1 = pv.rbegin();
   auto rit2 = pv.rbegin() + 1;

   REQUIRE(rit1 < rit2);
   REQUIRE(rit2 > rit1);
   REQUIRE(rit1 != rit2);

   auto rit3 = pv.rbegin();
   REQUIRE(rit1 == rit3);  // Same position
}

TEST_CASE("PolymorphicReverseIterator crbegin() and crend() provide const reverse iterators", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(1);
   vec.pushBack(2);

   const PolymorphicVector pv(vec);
   auto crit = pv.crbegin();
   REQUIRE(*static_cast<const int*>(crit.data()) == 2);

   crit = pv.crend();
   REQUIRE(crit.base().data() == pv.cbegin().data());
}

TEST_CASE("PolymorphicReverseIterator full backward traversal", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(10);
   vec.pushBack(20);
   vec.pushBack(30);
   vec.pushBack(40);

   PolymorphicVector pv(vec);

   // Backward traversal: 40, 30, 20, 10
   int expected[] = {40, 30, 20, 10};
   int idx = 0;

   for (auto rit = pv.rbegin(); rit != pv.rend(); ++rit) {
     REQUIRE(*static_cast<int*>(rit.data()) == expected[idx++]);
   }
   REQUIRE(idx == 4);  // Verified all 4 elements
}

TEST_CASE("PolymorphicReverseIterator base() returns forward iterator", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   vec.pushBack(1);
   vec.pushBack(2);
   vec.pushBack(3);

   PolymorphicVector pv(vec);
   auto rit = pv.rbegin() + 1;

   auto it = rit.base();

   REQUIRE(*static_cast<int*>(it.data()) == 3);
}

TEST_CASE("PolymorphicReverseIterator with Point type", "[polymorphic_reverse_iterator]") {
   Vector<Point> vec;
   vec.pushBack(Point{1, 1});
   vec.pushBack(Point{2, 2});
   vec.pushBack(Point{3, 3});

   PolymorphicVector pv(vec);
   auto rit = pv.rbegin();

   REQUIRE(*static_cast<Point*>(rit.data()) == Point(3, 3));
   ++rit;
   REQUIRE(*static_cast<Point*>(rit.data()) == Point(2, 2));
   ++rit;
   REQUIRE(*static_cast<Point*>(rit.data()) == Point(1, 1));
}

TEST_CASE("PolymorphicReverseIterator empty vector operations", "[polymorphic_reverse_iterator]") {
   Vector<int> vec;
   PolymorphicVector pv(vec);

   // rbegin() == rend() when empty
   REQUIRE(pv.rbegin() == pv.rend());
}

