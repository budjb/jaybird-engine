#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <new>

#include "rtti/types/IntType.hpp"

namespace {
using core::IName;
using core::rtti::IntType;
using core::rtti::IType;
using core::rtti::TypeKind;

IType* asType(IntType& value) {
  return &value;
}

}  // namespace

TEST_CASE("Given an IntType, when accessed through IType, then name size alignment and kind are reported correctly",
          "[rtti][int_type]") {
  IntType concrete(IName("int"));
  const IType* type = asType(concrete);

  REQUIRE(type->name() == IName("int"));
  REQUIRE(type->size() == sizeof(int));
  REQUIRE(type->alignment() == alignof(int));
  REQUIRE(type->kind() == TypeKind::SIMPLE);
}

TEST_CASE("Given a source int, when assign is called through IType, then destination equals source",
          "[rtti][int_type]") {
  IntType concrete(IName("int"));
  IType* type = asType(concrete);

  int destination = 0;
  constexpr int source = 42;
  type->assign(&destination, &source);

  REQUIRE(destination == 42);
}

TEST_CASE(
    "Given raw aligned storage, when construct and destroy are called through IType, then the int is "
    "zero-initialized and safely destructed",
    "[rtti][int_type]") {
  IntType concrete(IName("int"));
  IType* type = asType(concrete);

  alignas(int) std::array<std::byte, sizeof(int)> storage{};
  void* raw = storage.data();

  type->construct(raw);
  auto* value = std::launder(static_cast<int*>(raw));
  REQUIRE(*value == 0);

  *value = 33;
  type->destroy(raw);

  type->construct(raw);
  value = std::launder(static_cast<int*>(raw));
  REQUIRE(*value == 0);
  type->destroy(raw);
}

TEST_CASE(
    "Given an IntType, when create is called through IType, then a heap-allocated zero-initialized int is returned "
    "and freed cleanly",
    "[rtti][int_type]") {
  IntType concrete(IName("int"));
  IType* type = asType(concrete);

  void* raw = type->create();
  REQUIRE(raw != nullptr);

  auto* value = static_cast<int*>(raw);
  REQUIRE(*value == 0);
  *value = 99;
  REQUIRE(*value == 99);

  type->free(raw);
}

TEST_CASE(
    "Given two ints with equal and unequal values, when equals is called through IType, then only matching values "
    "compare equal",
    "[rtti][int_type]") {
  IntType concrete(IName("int"));
  const IType* type = asType(concrete);

  constexpr int lhs = 12;
  constexpr int rhsSame = 12;
  constexpr int rhsDifferent = 13;

  REQUIRE(type->equals(&lhs, &rhsSame));
  REQUIRE_FALSE(type->equals(&lhs, &rhsDifferent));
}

TEST_CASE(
    "Given two IntTypes sharing a name and one with a different name, when compared with == and !=, then identity is "
    "determined by descriptor name",
    "[rtti][int_type]") {
  IntType first(IName("int"));
  IntType sameName(IName("int"));
  IntType differentName(IName("other"));

  const IType* firstType = asType(first);
  const IType* sameNameType = asType(sameName);
  const IType* differentNameType = asType(differentName);

  REQUIRE(*firstType == *sameNameType);
  REQUIRE(*firstType != *differentNameType);
}
