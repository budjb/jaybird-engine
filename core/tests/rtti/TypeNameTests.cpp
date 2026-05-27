#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

#include "CString.hpp"
#include "rtti/TypeKind.hpp"
#include "rtti/TypeName.hpp"

namespace test_types {
struct MemberNamedType {
  static constexpr auto NAME = core::CString("member_named");
};

struct MappedNamedType {};

struct MemberAndMappedType {
  static constexpr auto NAME = core::CString("member_priority");
};
}  // namespace test_types

namespace core::rtti {
template <>
struct TypeName<test_types::MappedNamedType> {
  static constexpr auto value = CString("mapped_named");
};

template <>
struct TypeName<test_types::MemberAndMappedType> {
  // Keep the same length as MemberAndMappedType::NAME so GetTypeName/GetPrefixedTypeName deduce one return type.
  static constexpr auto value = CString("mapped_prioritx");
};
}  // namespace core::rtti

namespace {
static_assert(core::rtti::HasTypeNameMember<test_types::MemberNamedType>);
static_assert(core::rtti::HasTypeName<test_types::MemberNamedType>);
static_assert(core::rtti::HasTypeNameMapping<test_types::MappedNamedType>);
static_assert(core::rtti::HasTypeName<test_types::MappedNamedType>);

}  // namespace

TEST_CASE("Given a TypeKind, when TypePrefix is requested, then each supported kind returns the expected prefix", "[rtti][type_name]") {
  REQUIRE(static_cast<std::string_view>(core::rtti::TypePrefix<core::rtti::TypeKind::NAME>()) == "");
  REQUIRE(static_cast<std::string_view>(core::rtti::TypePrefix<core::rtti::TypeKind::ARRAY>()) == "array:");
  REQUIRE(static_cast<std::string_view>(core::rtti::TypePrefix<core::rtti::TypeKind::REF>()) == "ref:");
  REQUIRE(static_cast<std::string_view>(core::rtti::TypePrefix<core::rtti::TypeKind::WEAK_REF>()) == "wref:");
}

TEST_CASE(
    "Given a type with TypeName mapping only, when GetTypeName is used, then the mapped value is returned",
    "[rtti][type_name]") {
  constexpr auto name = core::rtti::GetTypeName<test_types::MappedNamedType>();

  REQUIRE(static_cast<std::string_view>(name) == "mapped_named");
}

TEST_CASE(
    "Given a type with both NAME and TypeName mapping, when GetTypeName is used, then the NAME value is selected",
    "[rtti][type_name]") {
  constexpr auto name = core::rtti::GetTypeName<test_types::MemberAndMappedType>();

  REQUIRE(static_cast<std::string_view>(name) == "member_priority");
}

TEST_CASE(
    "Given a type with a compile-time name, when GetPrefixedTypeName is used, then prefix and base name are "
    "concatenated",
    "[rtti][type_name]") {
  constexpr auto arrayName =
      core::rtti::GetPrefixedTypeName<core::rtti::TypeKind::ARRAY, test_types::MemberAndMappedType>();
  constexpr auto refName = core::rtti::GetPrefixedTypeName<core::rtti::TypeKind::REF, test_types::MappedNamedType>();

  REQUIRE(static_cast<std::string_view>(arrayName) == "array:member_priority");
  REQUIRE(static_cast<std::string_view>(refName) == "ref:mapped_named");
}

TEST_CASE(
    "Given a runtime base name, when runtime GetPrefixedTypeName is used, then the expected prefixed string is "
    "returned",
    "[rtti][type_name]") {
  const std::string name = core::rtti::GetPrefixedTypeName<core::rtti::TypeKind::WEAK_REF>("Entity");

  REQUIRE(name == "wref:Entity");
}
