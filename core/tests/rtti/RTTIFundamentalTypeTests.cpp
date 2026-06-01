#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <type_traits>

#include "CString.hpp"
#include "rtti/RTTIFundamentalType.hpp"
#include "rtti/RTTITypeName.hpp"

namespace {
using core::Name;
using core::rtti::RTTIFundamentalTType;
using core::rtti::RTTIFundamentalType;
using core::rtti::RTTITType;
using core::rtti::RTTIType;
using core::rtti::RTTITypeKind;

template <typename T>
class NamedFundamentalType final : public RTTITType<T, RTTIFundamentalType> {
 public:
  explicit NamedFundamentalType(const Name& name) : RTTITType<T, RTTIFundamentalType>(name) {}
};

template <typename T>
constexpr T valueA() {
  if constexpr (std::is_same_v<T, bool>) {
    return true;
  }
  if constexpr (std::is_floating_point_v<T>) {
    return static_cast<T>(7.25);
  }
  return static_cast<T>(42);
}

template <typename T>
constexpr T valueB() {
  if constexpr (std::is_same_v<T, bool>) {
    return false;
  }
  if constexpr (std::is_floating_point_v<T>) {
    return static_cast<T>(-3.5);
  }
  return static_cast<T>(13);
}

}  // namespace

TEMPLATE_TEST_CASE(
    "Given a concrete RTTIFundamentalType descriptor, when observed through RTTIType, then metadata matches the "
    "underlying fundamental C++ type",
    "[rtti][fundamental_type][metadata]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float, double,
    bool) {
  NamedFundamentalType<TestType> descriptor(Name("test_named_fundamental"));
  RTTIType* asType = &descriptor;
  RTTIFundamentalType* asFundamental = &descriptor;

  REQUIRE(asType != nullptr);
  REQUIRE(asFundamental != nullptr);
  REQUIRE(asType->kind() == RTTITypeKind::FUNDAMENTAL);
  REQUIRE(asType->size() == sizeof(TestType));
  REQUIRE(asType->alignment() == alignof(TestType));
  REQUIRE(asType->name() == Name("test_named_fundamental"));
}

TEMPLATE_TEST_CASE(
    "Given a RTTIFundamentalTType descriptor, when observed through RTTIFundamentalType and RTTIType, then metadata "
    "reflects the mapped built-in name and C++ type traits",
    "[rtti][fundamental_type][tfundamental_type][metadata]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
    float, double, bool) {
  RTTIFundamentalTType<TestType> descriptor;
  RTTIFundamentalType* asFundamental = &descriptor;
  const RTTIType& asType = *asFundamental;

  REQUIRE(asFundamental != nullptr);
  REQUIRE(asType.kind() == RTTITypeKind::FUNDAMENTAL);
  REQUIRE(asType.size() == sizeof(TestType));
  REQUIRE(asType.alignment() == alignof(TestType));
  REQUIRE(asType.name() == core::rtti::GetTypeName<TestType>());
}

TEMPLATE_TEST_CASE(
    "Given an RTTIFundamentalType descriptor and valid source/destination values, when assign is called through "
    "RTTIType, then destination receives source value",
    "[rtti][fundamental_type][operations][assign]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float,
    double, bool) {
  NamedFundamentalType<TestType> descriptor(Name("assign_type"));
  const RTTIType& type = descriptor;

  auto source = valueA<TestType>();
  auto destination = valueB<TestType>();

  type.assign(&destination, &source);
  REQUIRE(destination == source);
}

TEMPLATE_TEST_CASE(
    "Given an RTTIFundamentalType descriptor and a null destination, when assign is called through RTTIType, then "
    "operation is a no-op and existing values remain unchanged",
    "[rtti][fundamental_type][operations][assign][negative]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
    float, double, bool) {
  NamedFundamentalType<TestType> descriptor(Name("assign_null_destination_type"));
  const RTTIType& type = descriptor;

  auto source = valueA<TestType>();
  auto destination = valueB<TestType>();

  type.assign(nullptr, &source);
  REQUIRE(destination == valueB<TestType>());
}

TEMPLATE_TEST_CASE(
    "Given an RTTIFundamentalType descriptor and two equal values, when equals is called through RTTIType, then it "
    "returns true",
    "[rtti][fundamental_type][operations][equals]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float,
    double, bool) {
  NamedFundamentalType<TestType> descriptor(Name("equals_equal_type"));
  const RTTIType& type = descriptor;

  auto lhs = valueA<TestType>();
  auto rhs = valueA<TestType>();

  REQUIRE(type.equals(&lhs, &rhs));
}

TEMPLATE_TEST_CASE(
    "Given an RTTIFundamentalType descriptor and two different values, when equals is called through RTTIType, then it "
    "returns false",
    "[rtti][fundamental_type][operations][equals][negative]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
    float, double, bool) {
  NamedFundamentalType<TestType> descriptor(Name("equals_not_equal_type"));
  const RTTIType& type = descriptor;

  auto lhs = valueA<TestType>();
  auto rhs = valueB<TestType>();

  REQUIRE_FALSE(type.equals(&lhs, &rhs));
}

TEMPLATE_TEST_CASE(
    "Given an RTTIFundamentalType descriptor, when equals receives null combinations, then null-null is true and "
    "mixed-null is false",
    "[rtti][fundamental_type][operations][equals][null]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
    float, double, bool) {
  NamedFundamentalType<TestType> descriptor(Name("equals_null_type"));
  const RTTIType& type = descriptor;

  auto value = valueA<TestType>();

  REQUIRE(type.equals(nullptr, nullptr));
  REQUIRE_FALSE(type.equals(&value, nullptr));
  REQUIRE_FALSE(type.equals(nullptr, &value));
}

TEMPLATE_TEST_CASE(
    "Given an RTTIFundamentalType descriptor, when allocate is called through RTTIType, then returned memory is "
    "non-null and aligned for the underlying type",
    "[rtti][fundamental_type][operations][allocate]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float,
    double, bool) {
  NamedFundamentalType<TestType> descriptor(Name("allocate_type"));
  const RTTIType& type = descriptor;

  void* allocated = type.allocate();
  REQUIRE(allocated != nullptr);
  REQUIRE(reinterpret_cast<std::uintptr_t>(allocated) % alignof(TestType) == 0);

  type.deallocate(allocated);
}

TEMPLATE_TEST_CASE(
    "Given an RTTIFundamentalType descriptor and allocated storage, when construct is called through RTTIType, then "
    "the object is default-initialized",
    "[rtti][fundamental_type][operations][construct]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float,
    double, bool) {
  NamedFundamentalType<TestType> descriptor(Name("construct_type"));
  const RTTIType& type = descriptor;

  void* allocated = type.allocate();
  type.construct(allocated);

  REQUIRE(*static_cast<TestType*>(allocated) == TestType{});

  type.destruct(allocated);
  type.deallocate(allocated);
}

TEMPLATE_TEST_CASE(
    "Given an RTTIFundamentalType descriptor and null storage, when construct is called through RTTIType, then "
    "operation is safe and does nothing",
    "[rtti][fundamental_type][operations][construct][negative]", std::int32_t, std::uint32_t, std::int64_t,
    std::uint64_t, float, double, bool) {
  NamedFundamentalType<TestType> descriptor(Name("construct_null_type"));
  const RTTIType& type = descriptor;

  type.construct(nullptr);
  SUCCEED();
}

TEMPLATE_TEST_CASE(
    "Given an RTTIFundamentalType descriptor, when create is called through RTTIType, then a default-constructed heap "
    "object is returned",
    "[rtti][fundamental_type][operations][create]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float,
    double, bool) {
  NamedFundamentalType<TestType> descriptor(Name("create_type"));
  const RTTIType& type = descriptor;

  void* created = type.create();
  REQUIRE(created != nullptr);
  REQUIRE(*static_cast<TestType*>(created) == TestType{});

  type.destroy(created);
}

TEMPLATE_TEST_CASE(
    "Given an RTTIFundamentalType descriptor and a null instance pointer, when destroy is called through RTTIType, "
    "then operation is safe and does nothing",
    "[rtti][fundamental_type][operations][destroy][negative]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
    float, double, bool) {
  NamedFundamentalType<TestType> descriptor(Name("destroy_null_type"));
  const RTTIType& type = descriptor;

  type.destroy(nullptr);
  SUCCEED();
}
