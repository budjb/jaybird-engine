#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <memory>
#include <new>
#include <string>

#include "Hash.hpp"
#include "INamePool.hpp"
#include "rtti/types/IntType.hpp"

namespace {
using core::IName;
using core::rtti::IntType;
using core::rtti::IType;
using core::rtti::TypeKind;

IType* asType(IntType& value) {
  return &value;
}

std::string uniqueNameText(const char* prefix) {
  static std::size_t counter = 0;
  ++counter;
  return std::string(prefix) + "_" + std::to_string(counter);
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

TEST_CASE(
    "Given non-explicit IName constructors in RTTI code, when assigned from a string literal and std::string, then the "
    "same hash is produced",
    "[rtti][int_type]") {
  const std::string text = uniqueNameText("iname_implicit_ctor");

  const IName fromLiteral = text.c_str();
  const IName fromString = std::string(text);

  REQUIRE(fromLiteral.hash() == core::fnv1a_64(text));
  REQUIRE(fromString.hash() == core::fnv1a_64(text));
  REQUIRE(fromLiteral == fromString);
}

TEST_CASE(
    "Given non-explicit IName constructors in RTTI code, when passed to a function by value, then implicit conversion "
    "works for string literal and std::string",
    "[rtti][int_type]") {
  const std::string text = uniqueNameText("iname_implicit_param");

  auto hashOf = [](const IName name) { return name.hash(); };

  REQUIRE(hashOf(text.c_str()) == core::fnv1a_64(text));
  REQUIRE(hashOf(std::string(text)) == core::fnv1a_64(text));
}

TEST_CASE("Given an IntType without a registered array companion, when asArray is called, then nullptr is returned",
          "[rtti][int_type]") {
  core::INamePool& pool = core::INamePool::get();
  const IName name = pool.addName(uniqueNameText("as_array_missing"));
  IntType concrete(name);

  REQUIRE(asType(concrete)->asArray() == nullptr);
}

TEST_CASE(
    "Given an IntType and its matching array type registered in TypeRegistry, when asArray is called, then the array "
    "descriptor is returned",
    "[rtti][int_type]") {
  using core::rtti::IArrayType;
  using core::rtti::IntArrayType;
  using core::rtti::TypeRegistry;

  core::INamePool& pool = core::INamePool::get();
  const std::string baseText = uniqueNameText("as_array_base");
  const IName baseName = pool.addName(baseText);
  const IName arrayName = pool.addName(std::string("array:") + baseText);

  auto baseType = std::make_unique<IntType>(baseName);
  IntType* basePtr = baseType.get();
  auto arrayType = std::make_unique<IntArrayType>(arrayName, basePtr);
  IArrayType* expectedArray = arrayType.get();

  TypeRegistry* registry = TypeRegistry::get();
  REQUIRE(registry->registerType(std::move(baseType)));
  REQUIRE(registry->registerType(std::move(arrayType)));

  REQUIRE(asType(*basePtr)->asArray() == expectedArray);
}
