#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <new>
#include <stdexcept>
#include <vector>

#include "RttiTestUtils.hpp"

namespace {
using core::IName;
using core::rtti::IArrayType;
using core::rtti::IntArrayType;
using core::rtti::IType;
using core::rtti::TypeKind;
using tests::rtti::asArrayType;
using tests::rtti::asType;
using tests::rtti::intArrayType;
using tests::rtti::intElementType;
}  // namespace

TEST_CASE(
    "Given an IntArrayType, when accessed through IType, then name size alignment and kind are reported correctly",
    "[rtti][int_array_type]") {
  const IType* type = asType(intArrayType());

  REQUIRE(type->name() == IName("int[]"));
  REQUIRE(type->size() == sizeof(std::vector<int>));
  REQUIRE(type->alignment() == alignof(std::vector<int>));
  REQUIRE(type->kind() == TypeKind::ARRAY);
}

TEST_CASE(
    "Given an IntArrayType, when inner is queried through IArrayType, then the registered element type is returned",
    "[rtti][int_array_type]") {
  const IArrayType* arrayType = asArrayType(intArrayType());

  REQUIRE(arrayType->inner() == static_cast<IType*>(&intElementType()));
}

TEST_CASE("Given source and destination vectors, when assign is called through IType, then destination equals source",
          "[rtti][int_array_type]") {
  IType* type = asType(intArrayType());

  std::vector destination{1};
  const std::vector source{2, 4, 6};

  type->assign(&destination, &source);

  REQUIRE(destination == source);
}

TEST_CASE(
    "Given raw aligned storage, when construct and destroy are called through IType, then a vector is "
    "zero-initialized and safely destructed",
    "[rtti][int_array_type]") {
  IType* type = asType(intArrayType());

  alignas(std::vector<int>) std::array<std::byte, sizeof(std::vector<int>)> storage{};
  void* raw = storage.data();

  type->construct(raw);
  auto* value = std::launder(static_cast<std::vector<int>*>(raw));
  REQUIRE(value->empty());

  value->push_back(33);
  REQUIRE(value->size() == 1);
  REQUIRE((*value)[0] == 33);

  type->destruct(raw);
}

TEST_CASE(
    "Given an IntArrayType, when create is called through IType, then a usable heap-allocated empty vector is "
    "returned and freed cleanly",
    "[rtti][int_array_type]") {
  IType* type = asType(intArrayType());

  void* raw = type->create();
  REQUIRE(raw != nullptr);

  auto* value = static_cast<std::vector<int>*>(raw);
  REQUIRE(value->empty());

  value->push_back(99);
  value->push_back(100);
  REQUIRE(value->size() == 2);
  REQUIRE((*value)[0] == 99);
  REQUIRE((*value)[1] == 100);

  type->destroy(raw);
}

TEST_CASE(
    "Given two vectors with equal and unequal contents, when equals is called through IType, then only "
    "matching pairs compare equal",
    "[rtti][int_array_type]") {
  const IType* type = asType(intArrayType());

  const std::vector lhs{1, 2, 3};
  const std::vector rhsSame{1, 2, 3};
  const std::vector rhsDifferent{1, 2, 4};

  REQUIRE(type->equals(&lhs, &rhsSame));
  REQUIRE_FALSE(type->equals(&lhs, &rhsDifferent));
  REQUIRE(type->equals(nullptr, nullptr));
  REQUIRE_FALSE(type->equals(&lhs, nullptr));
}

TEST_CASE(
    "Given a vector, when length and at are called through IArrayType, then the correct size and element pointers "
    "are returned",
    "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{10, 20, 30};

  REQUIRE(arrayType->length(&values) == values.size());

  auto* mutableSecond = static_cast<int*>(arrayType->at(&values, 1));
  REQUIRE(*mutableSecond == 20);

  const IArrayType* constArrayType = arrayType;
  const auto* third = static_cast<const int*>(constArrayType->at(&values, 2));
  REQUIRE(*third == 30);

  REQUIRE_THROWS_AS(arrayType->at(&values, 3), std::out_of_range);
}

TEST_CASE(
    "Given a vector and IntArrayType, when reserve resize clear and shrinkToFit are called, then capacity and "
    "size change as expected",
    "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{1, 2, 3};

  arrayType->reserve(&values, 128);
  REQUIRE(values.capacity() >= 128);

  arrayType->resize(&values, 5);
  REQUIRE(values.size() == 5);
  REQUIRE(values[0] == 1);
  REQUIRE(values[1] == 2);
  REQUIRE(values[2] == 3);
  REQUIRE(values[3] == 0);
  REQUIRE(values[4] == 0);

  arrayType->clear(&values);
  REQUIRE(values.empty());

  arrayType->shrinkToFit(&values);
  REQUIRE(values.capacity() >= values.size());
}

TEST_CASE(
    "Given a vector and IArrayType, when at is used to write through the mutable pointer, then the element is "
    "updated in-place",
    "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{10, 20, 30};

  auto* second = static_cast<int*>(arrayType->at(&values, 1));
  *second = 999;

  REQUIRE(values[1] == 999);
}

TEST_CASE(
    "Given null array and type pointers, when operations are called through IArrayType and IType, then they "
    "behave as safe no-ops",
    "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  IType* type = asType(intArrayType());

  REQUIRE(arrayType->length(nullptr) == 0);
  REQUIRE(arrayType->at(nullptr, 0) == nullptr);
  REQUIRE(static_cast<const IArrayType*>(arrayType)->at(nullptr, 0) == nullptr);

  arrayType->reserve(nullptr, 5);
  arrayType->resize(nullptr, 5);
  arrayType->shrinkToFit(nullptr);
  arrayType->clear(nullptr);

  const std::vector source{7, 8, 9};
  type->assign(nullptr, &source);
  type->construct(nullptr);
  type->destruct(nullptr);
  type->destroy(nullptr);
}

TEST_CASE(
    "Given two IntArrayTypes sharing a name and one with a different name, when compared with == and !=, then "
    "identity is determined by descriptor name",
    "[rtti][int_array_type]") {
  IntArrayType sameName(IName("int[]"), &intElementType());
  IntArrayType differentName(IName("other[]"), &intElementType());

  const IType* singletonType = asType(intArrayType());
  const IType* sameNameType = asType(sameName);
  const IType* differentNameType = asType(differentName);

  REQUIRE(*singletonType == *sameNameType);
  REQUIRE(*singletonType != *differentNameType);
}

// ─── capacity and maxLength ───────────────────────────────────────────────────

TEST_CASE(
    "Given a vector with reserved capacity, when capacity is queried through IArrayType, then the reserved "
    "capacity is reflected",
    "[rtti][int_array_type]") {
  const IArrayType* arrayType = asArrayType(intArrayType());
  std::vector<int> values;

  values.reserve(64);
  REQUIRE(arrayType->capacity(&values) >= 64);

  REQUIRE(arrayType->capacity(nullptr) == 0);
}

TEST_CASE(
    "Given an IntArrayType, when maxLength is queried on a valid and a null pointer, then a non-zero limit and zero "
    "are returned respectively",
    "[rtti][int_array_type]") {
  const IArrayType* arrayType = asArrayType(intArrayType());
  const std::vector<int> values;

  REQUIRE(arrayType->maxLength(&values) > 0);
  REQUIRE(arrayType->maxLength(nullptr) == 0);
}

// ─── front and back ───────────────────────────────────────────────────────────

TEST_CASE(
    "Given a vector, when front is accessed through IArrayType, then a pointer to the first element is returned and "
    "mutation through it is reflected in the vector",
    "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{10, 20, 30};

  REQUIRE(*static_cast<int*>(arrayType->front(&values)) == 10);

  const IArrayType* constArrayType = arrayType;
  REQUIRE(*static_cast<const int*>(constArrayType->front(&values)) == 10);

  // mutate through front pointer
  *static_cast<int*>(arrayType->front(&values)) = 99;
  REQUIRE(values.front() == 99);

  REQUIRE(arrayType->front(nullptr) == nullptr);
  REQUIRE(constArrayType->front(nullptr) == nullptr);
}

TEST_CASE(
    "Given a vector, when back is accessed through IArrayType, then a pointer to the last element is returned and "
    "mutation through it is reflected in the vector",
    "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{10, 20, 30};

  REQUIRE(*static_cast<int*>(arrayType->back(&values)) == 30);

  const IArrayType* constArrayType = arrayType;
  REQUIRE(*static_cast<const int*>(constArrayType->back(&values)) == 30);

  // mutate through back pointer
  *static_cast<int*>(arrayType->back(&values)) = 77;
  REQUIRE(values.back() == 77);

  REQUIRE(arrayType->back(nullptr) == nullptr);
  REQUIRE(constArrayType->back(nullptr) == nullptr);
}

// ─── pushBack and popBack ─────────────────────────────────────────────────────

TEST_CASE("Given a vector, when pushBack is called through IArrayType, then the element is appended",
          "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{1, 2};

  constexpr int three = 3;
  arrayType->pushBack(&values, &three);

  REQUIRE(values.size() == 3);
  REQUIRE(values[2] == 3);

  // null-safe
  arrayType->pushBack(nullptr, &three);
  arrayType->pushBack(&values, nullptr);
  REQUIRE(values.size() == 3);  // unchanged
}

TEST_CASE("Given a vector, when popBack is called through IArrayType, then the last element is removed",
          "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{1, 2, 3};

  arrayType->popBack(&values);

  REQUIRE(values.size() == 2);
  REQUIRE(values.back() == 2);

  // null-safe
  arrayType->popBack(nullptr);
}

// ─── insert, erase, remove, replace ──────────────────────────────────────────

TEST_CASE(
    "Given a vector, when insert is called through IArrayType, then the element is placed at the given index and "
    "existing elements shift right",
    "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{1, 3, 4};

  constexpr int two = 2;
  arrayType->insert(&values, 1, &two);

  REQUIRE(values.size() == 4);
  REQUIRE(values[0] == 1);
  REQUIRE(values[1] == 2);
  REQUIRE(values[2] == 3);
  REQUIRE(values[3] == 4);

  // null-safe
  arrayType->insert(nullptr, 0, &two);
  arrayType->insert(&values, 0, nullptr);
  REQUIRE(values.size() == 4);
}

TEST_CASE(
    "Given a vector, when erase is called through IArrayType, then the element at the given index is removed and "
    "remaining elements shift left",
    "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{1, 2, 3, 4};

  arrayType->erase(&values, 1);

  REQUIRE(values.size() == 3);
  REQUIRE(values[0] == 1);
  REQUIRE(values[1] == 3);
  REQUIRE(values[2] == 4);

  // null-safe
  arrayType->erase(nullptr, 0);
}

TEST_CASE("Given a vector, when remove is called through IArrayType, then the element at the given index is removed",
          "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{10, 20, 30};

  arrayType->remove(&values, 0);

  REQUIRE(values.size() == 2);
  REQUIRE(values[0] == 20);
  REQUIRE(values[1] == 30);

  // null-safe
  arrayType->remove(nullptr, 0);
}

TEST_CASE(
    "Given a vector, when replace is called through IArrayType, then the element at the given index is overwritten "
    "without changing the vector size",
    "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{1, 2, 3};

  constexpr int newVal = 99;
  arrayType->replace(&values, 1, &newVal);

  REQUIRE(values[1] == 99);
  REQUIRE(values.size() == 3);  // no size change

  // null-safe
  arrayType->replace(nullptr, 0, &newVal);
  arrayType->replace(&values, 0, nullptr);
  REQUIRE(values[0] == 1);  // unchanged
}

// ─── typed at<T> template helper ─────────────────────────────────────────────

TEST_CASE(
    "Given a vector and IArrayType, when at<T> is used, then a correctly typed pointer to the element is returned",
    "[rtti][int_array_type]") {
  IArrayType* arrayType = asArrayType(intArrayType());
  std::vector values{5, 10, 15};

  const int* typed = arrayType->at<int>(&values, 2);
  REQUIRE(*typed == 15);

  const IArrayType* constArrayType = arrayType;
  const int* constTyped = constArrayType->at<int>(&values, 0);
  REQUIRE(*constTyped == 5);
}

// Iterator tests are intentionally split into RttiIteratorTests.cpp and RttiReverseIteratorTests.cpp.
