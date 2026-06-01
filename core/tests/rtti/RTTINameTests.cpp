#include <catch2/catch_test_macros.hpp>
#include <string>
#include <string_view>

#include "rtti/RTTIName.hpp"
#include "rtti/RTTITypeKind.hpp"
#include "types/CString.hpp"

namespace test_types {
struct MemberNamedType {
  // ReSharper disable once CppDeclaratorNeverUsed
  static constexpr auto NAME = core::CString("member_named");
};

struct MappedNamedType {};

struct MemberAndMappedType {
  // ReSharper disable once CppDeclaratorNeverUsed
  static constexpr auto NAME = core::CString("member_priority");
};

/**
 * @brief A plain struct with no @c NAME member and no @c TypeName<> specialization.
 *
 * It is used as the negative case in concept checks that require @code NamedType@endcode.
 */
struct PlainStruct {};
}  // namespace test_types

REGISTER_TYPE_NAME(test_types::MappedNamedType, "mapped_named");
// Keep the same length as MemberAndMappedType::NAME so GetRTTIName/GetPrefixedRTTIName deduce one return type.
REGISTER_TYPE_NAME(test_types::MemberAndMappedType, "mapped_prioritx");

namespace {
// --- has_type_name_member_v ---
static_assert(core::rtti::has_type_name_member_v<test_types::MemberNamedType>);
static_assert(core::rtti::has_type_name_member_v<test_types::MemberAndMappedType>);
static_assert(!core::rtti::has_type_name_member_v<test_types::MappedNamedType>);
static_assert(!core::rtti::has_type_name_member_v<test_types::PlainStruct>);

// --- has_type_name_provider_v ---
static_assert(core::rtti::has_type_name_provider_v<test_types::MappedNamedType>);
static_assert(core::rtti::has_type_name_provider_v<test_types::MemberAndMappedType>);
static_assert(!core::rtti::has_type_name_provider_v<test_types::MemberNamedType>);
static_assert(!core::rtti::has_type_name_provider_v<test_types::PlainStruct>);

// --- NamedType concept ---
static_assert(core::rtti::NamedRTTIType<test_types::MemberNamedType>);
static_assert(core::rtti::NamedRTTIType<test_types::MappedNamedType>);
static_assert(core::rtti::NamedRTTIType<test_types::MemberAndMappedType>);
static_assert(!core::rtti::NamedRTTIType<test_types::PlainStruct>);

// --- VectorType concept ---
static_assert(core::rtti::VectorType<core::Vector<int>>);
static_assert(core::rtti::VectorType<core::Vector<test_types::MappedNamedType>>);
static_assert(!core::rtti::VectorType<int>);
static_assert(!core::rtti::VectorType<test_types::MappedNamedType>);

// --- NamedVectorType concept ---
static_assert(core::rtti::NamedVectorType<core::Vector<test_types::MappedNamedType>>);
static_assert(core::rtti::NamedVectorType<core::Vector<test_types::MemberNamedType>>);
static_assert(!core::rtti::NamedVectorType<core::Vector<test_types::PlainStruct>>);
static_assert(!core::rtti::NamedVectorType<int>);
}  // namespace

TEST_CASE("Given a RTTITypeKind, when GetRTTIPrefix is requested, then each supported kind returns the expected prefix",
          "[rtti][type_name]") {
  REQUIRE(static_cast<std::string_view>(core::rtti::GetRTTIPrefix<core::rtti::RTTITypeKind::NAME>()) == "");
  REQUIRE(static_cast<std::string_view>(core::rtti::GetRTTIPrefix<core::rtti::RTTITypeKind::ARRAY>()) == "array:");
  REQUIRE(static_cast<std::string_view>(core::rtti::GetRTTIPrefix<core::rtti::RTTITypeKind::REF>()) == "ref:");
  REQUIRE(static_cast<std::string_view>(core::rtti::GetRTTIPrefix<core::rtti::RTTITypeKind::WEAK_REF>()) == "wref:");
}

TEST_CASE("Given a type with a NAME member only, when GetRTTIName is used, then the NAME value is returned",
          "[rtti][type_name]") {
  constexpr auto name = core::rtti::GetRTTIName<test_types::MemberNamedType>();

  REQUIRE(static_cast<std::string_view>(name) == "member_named");
}

TEST_CASE("Given a type with a TypeName mapping only, when GetRTTIName is used, then the mapped value is returned",
          "[rtti][type_name]") {
  constexpr auto name = core::rtti::GetRTTIName<test_types::MappedNamedType>();

  REQUIRE(static_cast<std::string_view>(name) == "mapped_named");
}

TEST_CASE(
    "Given a type with both a NAME member and a TypeName mapping, when GetRTTIName is used, then the NAME value is "
    "selected",
    "[rtti][type_name]") {
  constexpr auto name = core::rtti::GetRTTIName<test_types::MemberAndMappedType>();

  REQUIRE(static_cast<std::string_view>(name) == "member_priority");
}

TEST_CASE(
    "Given a compile-time type name, when GetPrefixedRTTIName is used, then the prefix and base name are concatenated",
    "[rtti][type_name]") {
  constexpr auto arrayName =
      core::rtti::GetPrefixedRTTIName<core::rtti::RTTITypeKind::ARRAY, test_types::MemberAndMappedType>();
  constexpr auto refName =
      core::rtti::GetPrefixedRTTIName<core::rtti::RTTITypeKind::REF, test_types::MappedNamedType>();

  REQUIRE(static_cast<std::string_view>(arrayName) == "array:member_priority");
  REQUIRE(static_cast<std::string_view>(refName) == "ref:mapped_named");
}

TEST_CASE(
    "Given a runtime base name, when the runtime GetPrefixedRTTIName overload is used, then the expected prefixed "
    "string is returned",
    "[rtti][type_name]") {
  const std::string name = core::rtti::GetPrefixedRTTIName<core::rtti::RTTITypeKind::WEAK_REF>("Entity");

  REQUIRE(name == "wref:Entity");
}

TEST_CASE(
    "Given a Vector of a named element type, when GetRTTIName is used, then the array prefix and element name "
    "are concatenated",
    "[rtti][type_name]") {
  constexpr auto mappedName = core::rtti::GetRTTIName<core::Vector<test_types::MappedNamedType>>();
  constexpr auto memberName = core::rtti::GetRTTIName<core::Vector<test_types::MemberNamedType>>();

  REQUIRE(static_cast<std::string_view>(mappedName) == "array:mapped_named");
  REQUIRE(static_cast<std::string_view>(memberName) == "array:member_named");
}
