#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <type_traits>

#include "CString.hpp"
#include "rtti/FundamentalType.hpp"
#include "rtti/TypeName.hpp"

namespace core::rtti {
template <>
struct TypeName<std::int32_t> {
  static constexpr CString value{"int32"};
};

template <>
struct TypeName<std::uint32_t> {
  static constexpr CString value{"uint32"};
};

template <>
struct TypeName<std::int64_t> {
  static constexpr CString value{"int64"};
};

template <>
struct TypeName<std::uint64_t> {
  static constexpr CString value{"uint64"};
};

template <>
struct TypeName<float> {
  static constexpr CString value{"float"};
};

template <>
struct TypeName<double> {
  static constexpr CString value{"double"};
};

template <>
struct TypeName<bool> {
  static constexpr CString value{"bool"};
};
}  // namespace core::rtti

namespace {
using core::IName;
using core::rtti::IFundamentalType;
using core::rtti::IType;
using core::rtti::TFundamentalType;
using core::rtti::TType;
using core::rtti::TypeKind;

template <typename T>
class NamedFundamentalType final : public TType<T, IFundamentalType> {
 public:
  explicit NamedFundamentalType(const IName& name) : TType<T, IFundamentalType>(name) {}
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
    "Given a concrete IFundamentalType descriptor, when observed through IType, then metadata matches the underlying "
    "fundamental C++ type",
    "[rtti][fundamental_type][metadata]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float, double,
    bool) {
  NamedFundamentalType<TestType> descriptor(IName("test_named_fundamental"));
  IType* asType = &descriptor;
  IFundamentalType* asFundamental = &descriptor;

  REQUIRE(asType != nullptr);
  REQUIRE(asFundamental != nullptr);
  REQUIRE(asType->kind() == TypeKind::FUNDAMENTAL);
  REQUIRE(asType->size() == sizeof(TestType));
  REQUIRE(asType->alignment() == alignof(TestType));
  REQUIRE(asType->name() == IName("test_named_fundamental"));
}

TEMPLATE_TEST_CASE(
    "Given a TFundamentalType descriptor, when observed through IFundamentalType and IType, then metadata reflects the "
    "mapped built-in name and C++ type traits",
    "[rtti][fundamental_type][tfundamental_type][metadata]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
    float, double, bool) {
  TFundamentalType<TestType> descriptor;
  IFundamentalType* asFundamental = &descriptor;
  const IType& asType = *asFundamental;

  REQUIRE(asFundamental != nullptr);
  REQUIRE(asType.kind() == TypeKind::FUNDAMENTAL);
  REQUIRE(asType.size() == sizeof(TestType));
  REQUIRE(asType.alignment() == alignof(TestType));
  REQUIRE(asType.name() == IName(core::rtti::GetTypeName<TestType>()));
}

TEMPLATE_TEST_CASE(
    "Given an IFundamentalType descriptor and valid source/destination values, when assign is called through IType, "
    "then "
    "destination receives source value",
    "[rtti][fundamental_type][operations][assign]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float,
    double, bool) {
  NamedFundamentalType<TestType> descriptor(IName("assign_type"));
  IType& type = descriptor;

  auto source = valueA<TestType>();
  auto destination = valueB<TestType>();

  type.assign(&destination, &source);
  REQUIRE(destination == source);
}

TEMPLATE_TEST_CASE(
    "Given an IFundamentalType descriptor and a null destination, when assign is called through IType, then operation "
    "is "
    "a no-op and existing values remain unchanged",
    "[rtti][fundamental_type][operations][assign][negative]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
    float, double, bool) {
  NamedFundamentalType<TestType> descriptor(IName("assign_null_destination_type"));
  IType& type = descriptor;

  auto source = valueA<TestType>();
  auto destination = valueB<TestType>();

  type.assign(nullptr, &source);
  REQUIRE(destination == valueB<TestType>());
}

TEMPLATE_TEST_CASE(
    "Given an IFundamentalType descriptor and two equal values, when equals is called through IType, then it returns "
    "true",
    "[rtti][fundamental_type][operations][equals]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float,
    double, bool) {
  NamedFundamentalType<TestType> descriptor(IName("equals_equal_type"));
  const IType& type = descriptor;

  auto lhs = valueA<TestType>();
  auto rhs = valueA<TestType>();

  REQUIRE(type.equals(&lhs, &rhs));
}

TEMPLATE_TEST_CASE(
    "Given an IFundamentalType descriptor and two different values, when equals is called through IType, then it "
    "returns false",
    "[rtti][fundamental_type][operations][equals][negative]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
    float, double, bool) {
  NamedFundamentalType<TestType> descriptor(IName("equals_not_equal_type"));
  const IType& type = descriptor;

  auto lhs = valueA<TestType>();
  auto rhs = valueB<TestType>();

  REQUIRE_FALSE(type.equals(&lhs, &rhs));
}

TEMPLATE_TEST_CASE(
    "Given an IFundamentalType descriptor, when equals receives null combinations, then null-null is true and "
    "mixed-null "
    "is false",
    "[rtti][fundamental_type][operations][equals][null]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
    float, double, bool) {
  NamedFundamentalType<TestType> descriptor(IName("equals_null_type"));
  const IType& type = descriptor;

  auto value = valueA<TestType>();

  REQUIRE(type.equals(nullptr, nullptr));
  REQUIRE_FALSE(type.equals(&value, nullptr));
  REQUIRE_FALSE(type.equals(nullptr, &value));
}

TEMPLATE_TEST_CASE(
    "Given an IFundamentalType descriptor, when allocate is called through IType, then returned memory is non-null and "
    "aligned for the underlying type",
    "[rtti][fundamental_type][operations][allocate]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float,
    double, bool) {
  NamedFundamentalType<TestType> descriptor(IName("allocate_type"));
  IType& type = descriptor;

  void* allocated = type.allocate();
  REQUIRE(allocated != nullptr);
  REQUIRE(reinterpret_cast<std::uintptr_t>(allocated) % alignof(TestType) == 0);

  type.deallocate(allocated);
}

TEMPLATE_TEST_CASE(
    "Given an IFundamentalType descriptor and allocated storage, when construct is called through IType, then the "
    "object "
    "is default-initialized",
    "[rtti][fundamental_type][operations][construct]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float,
    double, bool) {
  NamedFundamentalType<TestType> descriptor(IName("construct_type"));
  IType& type = descriptor;

  void* allocated = type.allocate();
  type.construct(allocated);

  REQUIRE(*static_cast<TestType*>(allocated) == TestType{});

  type.destruct(allocated);
  type.deallocate(allocated);
}

TEMPLATE_TEST_CASE(
    "Given an IFundamentalType descriptor and null storage, when construct is called through IType, then operation is "
    "safe and does nothing",
    "[rtti][fundamental_type][operations][construct][negative]", std::int32_t, std::uint32_t, std::int64_t,
    std::uint64_t, float, double, bool) {
  NamedFundamentalType<TestType> descriptor(IName("construct_null_type"));
  IType& type = descriptor;

  type.construct(nullptr);
  SUCCEED();
}

TEMPLATE_TEST_CASE(
    "Given an IFundamentalType descriptor, when create is called through IType, then a default-constructed heap object "
    "is "
    "returned",
    "[rtti][fundamental_type][operations][create]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t, float,
    double, bool) {
  NamedFundamentalType<TestType> descriptor(IName("create_type"));
  IType& type = descriptor;

  void* created = type.create();
  REQUIRE(created != nullptr);
  REQUIRE(*static_cast<TestType*>(created) == TestType{});

  type.destroy(created);
}

TEMPLATE_TEST_CASE(
    "Given an IFundamentalType descriptor and a null instance pointer, when destroy is called through IType, then "
    "operation is safe and does nothing",
    "[rtti][fundamental_type][operations][destroy][negative]", std::int32_t, std::uint32_t, std::int64_t, std::uint64_t,
    float, double, bool) {
  NamedFundamentalType<TestType> descriptor(IName("destroy_null_type"));
  IType& type = descriptor;

  type.destroy(nullptr);
  SUCCEED();
}
