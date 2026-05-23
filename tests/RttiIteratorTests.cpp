#include <catch2/catch_test_macros.hpp>
#include <vector>

#include "RttiTestUtils.hpp"
#include "rtti/Iterator.hpp"

namespace {
using core::rtti::IArrayType;
using core::rtti::Iterator;
using tests::rtti::asArrayType;
using tests::rtti::intArrayType;
}  // namespace

TEST_CASE("Given an RTTI array type and vector, when begin and end are iterated, then elements are visited in order",
          "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{10, 20, 30};

  Iterator<> it = arrayType->begin(&values);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 10);

  ++it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 20);

  ++it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  ++it;
  REQUIRE(it == arrayType->end(&values));
}

TEST_CASE("Given an RTTI iterator, when post-incremented, then the previous position is returned and iterator advances",
          "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{5, 6};

  Iterator<> it = arrayType->begin(&values);
  Iterator<> old = it++;

  REQUIRE(old == arrayType->begin(&values));
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 6);
}

TEST_CASE("Given an RTTI iterator at end, when pre-decremented, then it moves backward one element at a time",
          "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{1, 2, 3};

  Iterator<> it = arrayType->end(&values);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 3);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 2);
}

TEST_CASE("Given an RTTI iterator, when post-decremented, then the old position is returned and iterator moves back",
          "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{7, 8, 9};

  Iterator<> it = arrayType->end(&values);
  --it;
  const Iterator<> old = it--;

  REQUIRE(*static_cast<int*>(static_cast<void*>(old)) == 9);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 8);
}

TEST_CASE("Given an RTTI iterator, when adjusted with += and -=, then it moves by multiple element offsets",
          "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{10, 20, 30, 40};

  Iterator<> it = arrayType->begin(&values);
  it += 2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  it -= 1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 20);
}

TEST_CASE(
    "Given an RTTI iterator, when offset arithmetic uses + and -, then new iterators are returned without mutation",
    "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{1, 2, 3, 4};

  const Iterator<> begin = arrayType->begin(&values);
  const Iterator<> third = begin + 2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(third)) == 3);
  REQUIRE(begin == arrayType->begin(&values));

  const Iterator<> back = third - 1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(back)) == 2);
}

TEST_CASE(
    "Given an RTTI iterator, when scalar-plus-iterator arithmetic is used, then the original iterator is unchanged",
    "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{10, 20, 30};

  const Iterator<> begin = arrayType->begin(&values);
  const Iterator<> shifted = 2 + begin;
  REQUIRE(*static_cast<int*>(static_cast<void*>(shifted)) == 30);
  REQUIRE(begin == arrayType->begin(&values));
}

TEST_CASE("Given RTTI begin and end iterators, when subtracting them, then signed element distance is returned",
          "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{1, 2, 3, 4, 5};

  const Iterator<> first = arrayType->begin(&values);
  const Iterator<> last = arrayType->end(&values);

  REQUIRE(last - first == 5);
  REQUIRE(first - last == -5);
}

TEST_CASE("Given RTTI iterators at different positions, when compared, then ordering and equality reflect position",
          "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{1, 2, 3};

  Iterator<> a = arrayType->begin(&values);
  Iterator<> b = a + 1;
  Iterator<> c = a + 1;

  REQUIRE(a < b);
  REQUIRE(a <= b);
  REQUIRE(b > a);
  REQUIRE(b >= a);
  REQUIRE(b == c);
  REQUIRE(a != b);
  REQUIRE(b <= c);
  REQUIRE(b >= c);
}

TEST_CASE("Given an RTTI iterator, when indexed with operator[], then elements are read at offsets without moving",
          "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{100, 200, 300};

  Iterator<> it = arrayType->begin(&values);
  REQUIRE(*static_cast<int*>(it[0]) == 100);
  REQUIRE(*static_cast<int*>(it[1]) == 200);
  REQUIRE(*static_cast<int*>(it[2]) == 300);
  REQUIRE(it == arrayType->begin(&values));
}

TEST_CASE(
    "Given an RTTI iterator, when base get and implicit pointer conversion are used, then all pointer views are "
    "consistent",
    "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{11, 22, 33};

  const Iterator<> it = arrayType->begin(&values);

  void* asBase = it.base();
  void* asGet = it.get();
  void* asImplicit = it;

  REQUIRE(asBase == values.data());
  REQUIRE(asGet == values.data());
  REQUIRE(asImplicit == values.data());
}

TEST_CASE(
    "Given an RTTI iterator, when negative offsets and scalar-minus-iterator arithmetic are used, then movement "
    "remains correct",
    "[rtti][iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{10, 20, 30, 40};

  Iterator<> it = arrayType->begin(&values) + 2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  it += -1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 20);

  it -= -1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  const Iterator<> shifted = 1 - it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(shifted)) == 20);
}
