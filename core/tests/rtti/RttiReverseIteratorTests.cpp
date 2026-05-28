#include <catch2/catch_test_macros.hpp>
#include "Vector.hpp"


#include "RttiTestUtils.hpp"
#include "rtti/Iterator.hpp"

using core::Vector;


namespace {
using core::rtti::IArrayType;
using core::rtti::ReverseIterator;
using tests::rtti::asArrayType;
using tests::rtti::intArrayType;
}  // namespace

TEST_CASE(
    "Given an RTTI array type and vector, when rbegin and rend are compared, then reverse range length is correct",
    "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{1, 2, 3};

  ReverseIterator<> rbegin = arrayType->rbegin(&values);
  ReverseIterator<> rend = arrayType->rend(&values);

  REQUIRE(rbegin != rend);
  REQUIRE(rend - rbegin == 3);
}

TEST_CASE("Given an RTTI reverse iterator, when pre-incremented, then it traverses from back toward front",
          "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{10, 20, 30};

  ReverseIterator<> it = arrayType->rbegin(&values);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  ++it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 20);

  ++it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 10);

  ++it;
  REQUIRE(it == arrayType->rend(&values));
}

TEST_CASE("Given an RTTI reverse iterator, when post-incremented, then old position is returned and iterator advances",
          "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{5, 6, 7};

  ReverseIterator<> it = arrayType->rbegin(&values);
  const ReverseIterator<> old = it++;

  REQUIRE(*static_cast<int*>(static_cast<void*>(old)) == 7);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 6);
}

TEST_CASE("Given an RTTI reverse iterator at rend, when pre-decremented, then it moves toward the back",
          "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{1, 2, 3};

  ReverseIterator<> it = arrayType->rend(&values);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 1);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 2);
}

TEST_CASE("Given an RTTI reverse iterator, when post-decremented, then old position is returned and iterator retreats",
          "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{4, 5, 6};

  ReverseIterator<> it = arrayType->rend(&values);
  --it;
  const ReverseIterator<> old = it--;

  REQUIRE(*static_cast<int*>(static_cast<void*>(old)) == 4);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 5);
}

TEST_CASE("Given an RTTI reverse iterator, when adjusted with += and -=, then it moves in reverse offset space",
          "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{1, 2, 3, 4, 5};

  ReverseIterator<> it = arrayType->rbegin(&values);
  it += 2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 3);

  it -= 1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 4);
}

TEST_CASE(
    "Given an RTTI reverse iterator, when + and - are used, then new iterators are returned without mutating source",
    "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{10, 20, 30, 40};

  const ReverseIterator<> rbegin = arrayType->rbegin(&values);
  const ReverseIterator<> shifted = rbegin + 2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(shifted)) == 20);
  REQUIRE(rbegin == arrayType->rbegin(&values));

  const ReverseIterator<> back = shifted - 1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(back)) == 30);
}

TEST_CASE("Given an RTTI reverse iterator, when scalar-plus-iterator arithmetic is used, then source is unchanged",
          "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{100, 200, 300};

  const ReverseIterator<> rbegin = arrayType->rbegin(&values);
  const ReverseIterator<> shifted = 2 + rbegin;
  REQUIRE(*static_cast<int*>(static_cast<void*>(shifted)) == 100);
  REQUIRE(rbegin == arrayType->rbegin(&values));
}

TEST_CASE("Given RTTI reverse iterators, when subtracted, then signed distance follows reverse traversal direction",
          "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{1, 2, 3, 4, 5};

  const ReverseIterator<> rbegin = arrayType->rbegin(&values);
  const ReverseIterator<> rend = arrayType->rend(&values);

  REQUIRE(rend - rbegin == 5);
  REQUIRE(rbegin - rend == -5);
}

TEST_CASE("Given RTTI reverse iterators, when compared, then ordering reflects inverted reverse-iterator semantics",
          "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{1, 2, 3};

  ReverseIterator<> a = arrayType->rbegin(&values);
  ReverseIterator<> b = a + 1;
  ReverseIterator<> c = a + 1;

  REQUIRE(a < b);
  REQUIRE(a <= b);
  REQUIRE(b > a);
  REQUIRE(b >= a);
  REQUIRE(b == c);
  REQUIRE(a != b);
}

TEST_CASE("Given an RTTI reverse iterator, when indexed, then elements are read using reverse offsets",
          "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{10, 20, 30};

  ReverseIterator<> it = arrayType->rbegin(&values);
  REQUIRE(*static_cast<int*>(it[0]) == 30);
  REQUIRE(*static_cast<int*>(it[1]) == 20);
  REQUIRE(*static_cast<int*>(it[2]) == 10);
  REQUIRE(it == arrayType->rbegin(&values));
}

TEST_CASE(
    "Given an RTTI reverse iterator, when base get and implicit pointer conversion are used, then they map to reverse "
    "semantics",
    "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{10, 20, 30};

  const ReverseIterator<> it = arrayType->rbegin(&values);

  void* asBase = it.base();
  void* asGet = it.get();
  void* asImplicit = it;

  REQUIRE(asBase == values.data() + values.size());
  REQUIRE(asGet == values.data() + values.size() - 1);
  REQUIRE(asImplicit == values.data() + values.size() - 1);
}

TEST_CASE(
    "Given an RTTI reverse iterator, when negative offsets and scalar-minus-reverse-iterator arithmetic are used, then "
    "movement remains correct",
    "[rtti][reverse_iterator]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  Vector values{10, 20, 30, 40};

  ReverseIterator<> it = arrayType->rbegin(&values) + 1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  it += -1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 40);

  it -= -2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 20);

  const ReverseIterator<> shifted = 1 - it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(shifted)) == 30);
}
