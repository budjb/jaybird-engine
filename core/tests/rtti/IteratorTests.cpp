#include <catch2/catch_test_macros.hpp>

#include "Vector.hpp"
#include "rtti/ArrayType.hpp"
#include "rtti/FundamentalType.hpp"
#include "rtti/Iterator.hpp"
#include "rtti/TypeName.hpp"

using core::Vector;

/**
 * @brief This struct specializes @code TypeName@endcode for @code int@endcode.
 */
template <>
struct core::rtti::TypeName<int> {
  /**
   * @brief This static member provides the compile-time name for @code int@endcode.
   */
  static constexpr CString value{"int"};
};  // namespace core::rtti

namespace {
using core::IName;
using core::rtti::IArrayType;
using core::rtti::Iterator;
using core::rtti::ReverseIterator;
using core::rtti::TArrayType;
using core::rtti::TFundamentalType;

/**
 * @brief Returns a cached int array type descriptor for testing.
 *
 * The returned @code TArrayType<int>@endcode instance uses @code TFundamentalType<int>@endcode
 * as its element type. The same instance is reused across all test calls; it should not be deleted.
 *
 * @return A pointer to the cached @code IArrayType@endcode for int arrays.
 */
IArrayType* createIntArrayType() {
  static TFundamentalType<int> intType;
  static TArrayType<int> arrayType(&intType);
  return &arrayType;
}

/**
 * @brief Clarifies that a pointer is being treated as an array type.
 *
 * This identity operation makes test code intent explicit.
 *
 * @param arrayType The array type pointer.
 * @return The same pointer unchanged.
 */
IArrayType* asArrayType(IArrayType* arrayType) {
  return arrayType;
}

}  // namespace

// ============================================================================
// Forward Iterator Tests
// ============================================================================
TEST_CASE(
    "Given an RTTI array type and vector, when begin and end iterators are compared, then forward range length is "
    "correct",
    "[rtti][iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  Iterator<> begin = asArrayType(arrayType)->begin(&values);
  Iterator<> end = asArrayType(arrayType)->end(&values);

  REQUIRE(begin != end);
  REQUIRE(end - begin == 3);
}

TEST_CASE(
    "Given an RTTI int array descriptor, when array APIs are invoked with null and live data, then int specializations "
    "are covered",
    "[rtti][iterator][array_type][coverage]") {
  const auto arrayType = createIntArrayType();
  const IArrayType& constArrayType = *arrayType;

  Vector values{1, 2, 3};
  const auto& constValues = values;

  REQUIRE(arrayType->length(nullptr) == 0);
  REQUIRE(arrayType->capacity(nullptr) == 0);
  REQUIRE(arrayType->maxLength(nullptr) == 0);
  REQUIRE(arrayType->at(nullptr, 0) == nullptr);
  REQUIRE(constArrayType.at(nullptr, 0) == nullptr);
  REQUIRE(arrayType->front(nullptr) == nullptr);
  REQUIRE(constArrayType.front(nullptr) == nullptr);
  REQUIRE(arrayType->back(nullptr) == nullptr);
  REQUIRE(constArrayType.back(nullptr) == nullptr);

  REQUIRE(arrayType->length(&values) == 3);
  REQUIRE(arrayType->capacity(&values) >= 3);
  REQUIRE(arrayType->maxLength(&values) >= values.size());
  REQUIRE(*static_cast<int*>(arrayType->at(&values, 1)) == 2);
  REQUIRE(*static_cast<const int*>(constArrayType.at(&constValues, 2)) == 3);
  REQUIRE(*static_cast<int*>(arrayType->front(&values)) == 1);
  REQUIRE(*static_cast<const int*>(constArrayType.front(&constValues)) == 1);
  REQUIRE(*static_cast<int*>(arrayType->back(&values)) == 3);
  REQUIRE(*static_cast<const int*>(constArrayType.back(&constValues)) == 3);

  int insertValue = 99;
  int replaceValue = 42;
  arrayType->insert(&values, 1, &insertValue);
  REQUIRE(values[1] == 99);

  arrayType->replace(&values, 1, &replaceValue);
  REQUIRE(values[1] == 42);

  arrayType->pushBack(&values, &insertValue);
  REQUIRE(values.back() == 99);

  arrayType->erase(&values, 0);
  REQUIRE(values.front() == 42);

  arrayType->remove(&values, values.size() - 1);
  REQUIRE(values.back() != 99);

  arrayType->popBack(&values);
  REQUIRE(values.size() == 2);

  arrayType->reserve(&values, 16);
  REQUIRE(values.capacity() >= 16);

  arrayType->resize(&values, 5);
  REQUIRE(values.size() == 5);

  arrayType->shrinkToFit(&values);
  REQUIRE(values.capacity() >= values.size());

  arrayType->clear(&values);
  REQUIRE(values.empty());

  arrayType->insert(nullptr, 0, &insertValue);
  arrayType->insert(&values, 0, nullptr);
  arrayType->pushBack(nullptr, &insertValue);
  arrayType->pushBack(&values, nullptr);
  arrayType->replace(nullptr, 0, &replaceValue);
  arrayType->replace(&values, 0, nullptr);
  arrayType->erase(nullptr, 0);
  arrayType->remove(nullptr, 0);
  arrayType->popBack(nullptr);
  arrayType->reserve(nullptr, 4);
  arrayType->resize(nullptr, 4);
  arrayType->shrinkToFit(nullptr);
  arrayType->clear(nullptr);
}

TEST_CASE("Given an RTTI array type and vector, when begin and end are iterated, then elements are visited in order",
          "[rtti][iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{10, 20, 30};

  Iterator<> it = asArrayType(arrayType)->begin(&values);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 10);

  ++it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 20);

  ++it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  ++it;
  REQUIRE(it == asArrayType(arrayType)->end(&values));
}

TEST_CASE("Given an RTTI iterator, when post-incremented, then the previous position is returned and iterator advances",
          "[rtti][iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{5, 6};

  Iterator<> it = asArrayType(arrayType)->begin(&values);
  Iterator<> old = it++;

  REQUIRE(old == asArrayType(arrayType)->begin(&values));
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 6);
}

TEST_CASE("Given an RTTI iterator at end, when pre-decremented, then it moves backward one element at a time",
          "[rtti][iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  Iterator<> it = asArrayType(arrayType)->end(&values);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 3);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 2);
}

TEST_CASE("Given an RTTI iterator, when post-decremented, then the old position is returned and iterator moves back",
          "[rtti][iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{7, 8, 9};

  Iterator<> it = asArrayType(arrayType)->end(&values);
  --it;
  const Iterator<> old = it--;

  REQUIRE(*static_cast<int*>(static_cast<void*>(old)) == 9);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 8);
}

TEST_CASE("Given an RTTI iterator, when adjusted with += and -=, then it moves by multiple element offsets",
          "[rtti][iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{10, 20, 30, 40};

  Iterator<> it = asArrayType(arrayType)->begin(&values);
  it += 2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  it -= 1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 20);
}

TEST_CASE(
    "Given an RTTI iterator, when offset arithmetic uses + and -, then new iterators are returned without mutation",
    "[rtti][iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3, 4};

  const Iterator<> begin = asArrayType(arrayType)->begin(&values);
  const Iterator<> third = begin + 2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(third)) == 3);
  REQUIRE(begin == asArrayType(arrayType)->begin(&values));

  const Iterator<> back = third - 1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(back)) == 2);
}

TEST_CASE(
    "Given an RTTI iterator, when scalar-plus-iterator arithmetic is used, then the original iterator is unchanged",
    "[rtti][iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{10, 20, 30};

  const Iterator<> begin = asArrayType(arrayType)->begin(&values);
  const Iterator<> shifted = 2 + begin;
  REQUIRE(*static_cast<int*>(static_cast<void*>(shifted)) == 30);
  REQUIRE(begin == asArrayType(arrayType)->begin(&values));
}

TEST_CASE("Given RTTI begin and end iterators, when subtracting them, then signed element distance is returned",
          "[rtti][iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3, 4, 5};

  const Iterator<> first = asArrayType(arrayType)->begin(&values);
  const Iterator<> last = asArrayType(arrayType)->end(&values);

  REQUIRE(last - first == 5);
  REQUIRE(first - last == -5);
}

TEST_CASE("Given RTTI iterators at different positions, when compared, then ordering and equality reflect position",
          "[rtti][iterator]") {
  auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  Iterator<> a = asArrayType(arrayType)->begin(&values);
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
  const auto arrayType = createIntArrayType();
  Vector values{100, 200, 300};

  Iterator<> it = asArrayType(arrayType)->begin(&values);
  REQUIRE(*static_cast<int*>(it[0]) == 100);
  REQUIRE(*static_cast<int*>(it[1]) == 200);
  REQUIRE(*static_cast<int*>(it[2]) == 300);
  REQUIRE(it == asArrayType(arrayType)->begin(&values));
}

TEST_CASE(
    "Given an RTTI iterator, when base get and implicit pointer conversion are used, then all pointer views are "
    "consistent",
    "[rtti][iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{11, 22, 33};

  const Iterator<> it = asArrayType(arrayType)->begin(&values);

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
  const auto arrayType = createIntArrayType();
  Vector values{10, 20, 30, 40};

  Iterator<> it = asArrayType(arrayType)->begin(&values) + 2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  it += -1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 20);

  it -= -1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  const Iterator<> shifted = 1 - it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(shifted)) == 20);
}

TEST_CASE("Given an empty RTTI array, when begin and end are compared, then they are equal",
          "[rtti][iterator][edge_case]") {
  const auto arrayType = createIntArrayType();
  Vector<int> values;

  Iterator<> begin = asArrayType(arrayType)->begin(&values);
  Iterator<> end = asArrayType(arrayType)->end(&values);

  REQUIRE(begin == end);
  REQUIRE(end - begin == 0);
}

TEST_CASE("Given an RTTI array with a single element, when iterated, then the element is visited correctly",
          "[rtti][iterator][edge_case]") {
  const auto arrayType = createIntArrayType();
  Vector values{42};

  Iterator<> it = asArrayType(arrayType)->begin(&values);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 42);

  ++it;
  REQUIRE(it == asArrayType(arrayType)->end(&values));
}

TEST_CASE("Given an RTTI iterator at end, when decremented to begin, then all elements are accessible in reverse",
          "[rtti][iterator][edge_case]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  Iterator<> it = asArrayType(arrayType)->end(&values);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 3);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 2);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 1);
}

TEST_CASE("Given RTTI iterators to the same position, when subtracted, then zero distance is returned",
          "[rtti][iterator][edge_case]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  Iterator<> it1 = asArrayType(arrayType)->begin(&values);
  Iterator<> it2 = asArrayType(arrayType)->begin(&values);

  REQUIRE(it1 - it2 == 0);
  REQUIRE(it1 == it2);
}

// ============================================================================
// Reverse Iterator Tests
// ============================================================================
TEST_CASE(
    "Given an RTTI array type and vector, when rbegin and rend are compared, then reverse range length is correct",
    "[rtti][reverse_iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  ReverseIterator<> rbegin = asArrayType(arrayType)->rbegin(&values);
  ReverseIterator<> rend = asArrayType(arrayType)->rend(&values);

  REQUIRE(rbegin != rend);
  REQUIRE(rend - rbegin == 3);
}

TEST_CASE("Given an RTTI reverse iterator, when pre-incremented, then it traverses from back toward front",
          "[rtti][reverse_iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{10, 20, 30};

  ReverseIterator<> it = asArrayType(arrayType)->rbegin(&values);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  ++it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 20);

  ++it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 10);

  ++it;
  REQUIRE(it == asArrayType(arrayType)->rend(&values));
}

TEST_CASE("Given an RTTI reverse iterator, when post-incremented, then old position is returned and iterator advances",
          "[rtti][reverse_iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{5, 6, 7};

  ReverseIterator<> it = asArrayType(arrayType)->rbegin(&values);
  const ReverseIterator<> old = it++;

  REQUIRE(*static_cast<int*>(static_cast<void*>(old)) == 7);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 6);
}

TEST_CASE("Given an RTTI reverse iterator at rend, when pre-decremented, then it moves toward the back",
          "[rtti][reverse_iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  ReverseIterator<> it = asArrayType(arrayType)->rend(&values);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 1);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 2);
}

TEST_CASE("Given an RTTI reverse iterator, when post-decremented, then old position is returned and iterator retreats",
          "[rtti][reverse_iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{4, 5, 6};

  ReverseIterator<> it = asArrayType(arrayType)->rend(&values);
  --it;
  const ReverseIterator<> old = it--;

  REQUIRE(*static_cast<int*>(static_cast<void*>(old)) == 4);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 5);
}

TEST_CASE("Given an RTTI reverse iterator, when adjusted with += and -=, then it moves in reverse offset space",
          "[rtti][reverse_iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3, 4, 5};

  ReverseIterator<> it = asArrayType(arrayType)->rbegin(&values);
  it += 2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 3);

  it -= 1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 4);
}

TEST_CASE(
    "Given an RTTI reverse iterator, when + and - are used, then new iterators are returned without mutating source",
    "[rtti][reverse_iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{10, 20, 30, 40};

  const ReverseIterator<> rbegin = asArrayType(arrayType)->rbegin(&values);
  const ReverseIterator<> shifted = rbegin + 2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(shifted)) == 20);
  REQUIRE(rbegin == asArrayType(arrayType)->rbegin(&values));

  const ReverseIterator<> back = shifted - 1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(back)) == 30);
}

TEST_CASE("Given an RTTI reverse iterator, when scalar-plus-iterator arithmetic is used, then source is unchanged",
          "[rtti][reverse_iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{100, 200, 300};

  const ReverseIterator<> rbegin = asArrayType(arrayType)->rbegin(&values);
  const ReverseIterator<> shifted = 2 + rbegin;
  REQUIRE(*static_cast<int*>(static_cast<void*>(shifted)) == 100);
  REQUIRE(rbegin == asArrayType(arrayType)->rbegin(&values));
}

TEST_CASE("Given RTTI reverse iterators, when subtracted, then signed distance follows reverse traversal direction",
          "[rtti][reverse_iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3, 4, 5};

  const ReverseIterator<> rbegin = asArrayType(arrayType)->rbegin(&values);
  const ReverseIterator<> rend = asArrayType(arrayType)->rend(&values);

  REQUIRE(rend - rbegin == 5);
  REQUIRE(rbegin - rend == -5);
}

TEST_CASE("Given RTTI reverse iterators, when compared, then ordering reflects inverted reverse-iterator semantics",
          "[rtti][reverse_iterator]") {
  auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  ReverseIterator<> a = asArrayType(arrayType)->rbegin(&values);
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
  const auto arrayType = createIntArrayType();
  Vector values{10, 20, 30};

  ReverseIterator<> it = asArrayType(arrayType)->rbegin(&values);
  REQUIRE(*static_cast<int*>(it[0]) == 30);
  REQUIRE(*static_cast<int*>(it[1]) == 20);
  REQUIRE(*static_cast<int*>(it[2]) == 10);
  REQUIRE(it == asArrayType(arrayType)->rbegin(&values));
}

TEST_CASE(
    "Given an RTTI reverse iterator, when base get and implicit pointer conversion are used, then they map to reverse "
    "semantics",
    "[rtti][reverse_iterator]") {
  const auto arrayType = createIntArrayType();
  Vector values{10, 20, 30};

  const ReverseIterator<> it = asArrayType(arrayType)->rbegin(&values);

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
  const auto arrayType = createIntArrayType();
  Vector values{10, 20, 30, 40};

  ReverseIterator<> it = asArrayType(arrayType)->rbegin(&values) + 1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 30);

  it += -1;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 40);

  it -= -2;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 20);

  const ReverseIterator<> shifted = 1 - it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(shifted)) == 30);
}

TEST_CASE("Given an empty RTTI array, when rbegin and rend are compared, then they are equal",
          "[rtti][reverse_iterator][edge_case]") {
  const auto arrayType = createIntArrayType();
  Vector<int> values;

  ReverseIterator<> rbegin = asArrayType(arrayType)->rbegin(&values);
  ReverseIterator<> rend = asArrayType(arrayType)->rend(&values);

  REQUIRE(rbegin == rend);
  REQUIRE(rend - rbegin == 0);
}

TEST_CASE("Given an RTTI array with a single element, when reverse iterated, then the element is visited correctly",
          "[rtti][reverse_iterator][edge_case]") {
  const auto arrayType = createIntArrayType();
  Vector values{42};

  ReverseIterator<> it = asArrayType(arrayType)->rbegin(&values);
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 42);

  ++it;
  REQUIRE(it == asArrayType(arrayType)->rend(&values));
}

TEST_CASE(
    "Given an RTTI reverse iterator at rend, when decremented to rbegin, then all elements are accessible in order",
    "[rtti][reverse_iterator][edge_case]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  ReverseIterator<> it = asArrayType(arrayType)->rend(&values);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 1);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 2);
  --it;
  REQUIRE(*static_cast<int*>(static_cast<void*>(it)) == 3);
}

TEST_CASE("Given RTTI reverse iterators to the same position, when subtracted, then zero distance is returned",
          "[rtti][reverse_iterator][edge_case]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  ReverseIterator<> it1 = asArrayType(arrayType)->rbegin(&values);
  ReverseIterator<> it2 = asArrayType(arrayType)->rbegin(&values);

  REQUIRE(it1 - it2 == 0);
  REQUIRE(it1 == it2);
}

// ============================================================================
// Cross Iterator Tests (Forward vs. Reverse Semantics Compatibility)
// ============================================================================
TEST_CASE(
    "Given forward and reverse iterators from the same array, when compared to begin/end equivalents, then forward and "
    "reverse cover identical range",
    "[rtti][iterator][reverse_iterator][semantics]") {
  const auto arrayType = createIntArrayType();
  Vector values{1, 2, 3};

  const Iterator<> begin = asArrayType(arrayType)->begin(&values);
  const Iterator<> end = asArrayType(arrayType)->end(&values);
  const ReverseIterator<> rbegin = asArrayType(arrayType)->rbegin(&values);
  const ReverseIterator<> rend = asArrayType(arrayType)->rend(&values);

  REQUIRE(end - begin == rend - rbegin);
}

TEST_CASE(
    "Given an RTTI array, when forward and reverse iterators visit elements in opposite directions, then all elements "
    "are "
    "visited exactly once in each direction",
    "[rtti][iterator][reverse_iterator][semantics]") {
  auto arrayType = createIntArrayType();
  Vector values{10, 20, 30};

  Vector<int> forward_order;
  for (auto it = asArrayType(arrayType)->begin(&values); it != asArrayType(arrayType)->end(&values); ++it) {
    forward_order.pushBack(*static_cast<int*>(static_cast<void*>(it)));
  }

  Vector<int> reverse_order;
  for (auto it = asArrayType(arrayType)->rbegin(&values); it != asArrayType(arrayType)->rend(&values); ++it) {
    reverse_order.pushBack(*static_cast<int*>(static_cast<void*>(it)));
  }

  REQUIRE(forward_order.size() == 3);
  REQUIRE(reverse_order.size() == 3);
  REQUIRE(forward_order[0] == 10);
  REQUIRE(forward_order[1] == 20);
  REQUIRE(forward_order[2] == 30);
  REQUIRE(reverse_order[0] == 30);
  REQUIRE(reverse_order[1] == 20);
  REQUIRE(reverse_order[2] == 10);
}
