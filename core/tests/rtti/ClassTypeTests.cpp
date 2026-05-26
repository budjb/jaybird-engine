#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <type_traits>

#include "rtti/ClassType.hpp"

/**
 * @brief A trivially copyable struct with a single int field.
 *
 * This struct exercises the fast @c memcpy path inside @code TType::assign@endcode and
 * serves as the simplest possible class type for RTTI descriptor tests.
 */
struct TrivialStruct {
  // ReSharper disable once CppDeclaratorNeverUsed
  int x{0};
  bool operator==(const TrivialStruct&) const = default;
};

/**
 * @brief A struct with an over-alignment requirement of 16 bytes.
 *
 * This struct verifies that @code TType::allocate@endcode and @code TType::deallocate@endcode
 * use the alignment-aware operator @c new / @c delete when the type requires non-default alignment.
 */
struct AlignedStruct {
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  alignas(16) float data[4]{};
  bool operator==(const AlignedStruct&) const = default;
};

/**
 * @brief A non-trivially copyable struct that wraps a @code std::string@endcode.
 *
 * This struct exercises the copy-assignment path inside @code TType::assign@endcode (bypassing
 * @c memcpy) and validates that @code construct@endcode and @code destruct@endcode correctly
 * initialize and tear down the contained string.
 */
struct NonTrivialStruct {
  std::string value{};
  bool operator==(const NonTrivialStruct&) const = default;
};

namespace {
using core::IName;
using core::rtti::IClassType;
using core::rtti::IType;
using core::rtti::TClassType;
using core::rtti::TType;
using core::rtti::TypeKind;

/**
 * @brief A concrete helper subtype that wires @code TType<T, IClassType>@endcode to a caller-supplied name.
 *
 * This mirrors the @c NamedFundamentalType helper used in @code FundamentalTypeTests@endcode and
 * lets each test choose an isolated name to avoid cross-test collisions.
 *
 * @tparam T The underlying class type described by this helper.
 */
template <typename T>
class NamedClassType final : public TType<T, IClassType> {
 public:
  /**
   * @brief Constructs a @code NamedClassType@endcode with the given name.
   *
   * @param name The name to assign to this type descriptor.
   */
  explicit NamedClassType(const IName& name) : TType<T, IClassType>(name) {}
};

/**
 * @brief Returns a "first" representative value of type @code T@endcode.
 *
 * @tparam T One of the three test struct types used in this file.
 * @return A value suitable for use as a source or left-hand side in tests.
 */
template <typename T>
T makeValueA() {
  if constexpr (std::is_same_v<T, TrivialStruct>) {
    return TrivialStruct{10};
  } else if constexpr (std::is_same_v<T, AlignedStruct>) {
    AlignedStruct s;
    s.data[0] = 1.0f;
    s.data[1] = 2.0f;
    s.data[2] = 3.0f;
    s.data[3] = 4.0f;
    return s;
  } else {
    return NonTrivialStruct{"hello"};
  }
}

/**
 * @brief Returns a "second" representative value of type @code T@endcode that differs from @code makeValueA@endcode.
 *
 * @tparam T One of the three test struct types used in this file.
 * @return A value that is not equal to the one returned by @code makeValueA<T>()@endcode.
 */
template <typename T>
T makeValueB() {
  if constexpr (std::is_same_v<T, TrivialStruct>) {
    return TrivialStruct{99};
  } else if constexpr (std::is_same_v<T, AlignedStruct>) {
    AlignedStruct s;
    s.data[0] = 5.0f;
    s.data[1] = 6.0f;
    s.data[2] = 7.0f;
    s.data[3] = 8.0f;
    return s;
  } else {
    return NonTrivialStruct{"world"};
  }
}
}  // namespace

// ============================================================================
// IClassType / NamedClassType metadata tests
// ============================================================================

TEMPLATE_TEST_CASE(
    "Given a concrete IClassType descriptor, when observed through IType, then metadata matches the underlying class "
    "type",
    "[rtti][class_type][metadata]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("test_named_class"));
  IType* asType = &descriptor;
  IClassType* asClass = &descriptor;

  REQUIRE(asType != nullptr);
  REQUIRE(asClass != nullptr);
  REQUIRE(asType->kind() == TypeKind::CLASS);
  REQUIRE(asType->size() == sizeof(TestType));
  REQUIRE(asType->alignment() == alignof(TestType));
  REQUIRE(asType->name() == IName("test_named_class"));
}

// ============================================================================
// TClassType metadata tests
// ============================================================================

TEMPLATE_TEST_CASE(
    "Given a TClassType descriptor, when observed through IClassType and IType, then metadata reflects the name passed "
    "to the constructor",
    "[rtti][class_type][tclass_type][metadata]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  TClassType<TestType> descriptor(IName("my_class_type"));
  IClassType* asClass = &descriptor;
  const IType& asType = *asClass;

  REQUIRE(asClass != nullptr);
  REQUIRE(asType.kind() == TypeKind::CLASS);
  REQUIRE(asType.size() == sizeof(TestType));
  REQUIRE(asType.alignment() == alignof(TestType));
  REQUIRE(asType.name() == IName("my_class_type"));
}

// ============================================================================
// IType::operator== and operator!= tests
// ============================================================================

TEMPLATE_TEST_CASE(
    "Given two IClassType descriptors with the same name, when compared with operator==, then they are considered "
    "equal",
    "[rtti][class_type][equality]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> a(IName("same_name"));
  NamedClassType<TestType> b(IName("same_name"));

  REQUIRE(static_cast<IType&>(a) == static_cast<IType&>(b));
  REQUIRE_FALSE(static_cast<IType&>(a) != static_cast<IType&>(b));
}

TEMPLATE_TEST_CASE(
    "Given two IClassType descriptors with different names, when compared with operator!=, then they are not equal",
    "[rtti][class_type][equality][negative]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> a(IName("name_a"));
  NamedClassType<TestType> b(IName("name_b"));

  REQUIRE(static_cast<IType&>(a) != static_cast<IType&>(b));
  REQUIRE_FALSE(static_cast<IType&>(a) == static_cast<IType&>(b));
}

// ============================================================================
// assign tests
// ============================================================================

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor and valid source/destination values, when assign is called through IType, then "
    "destination receives source value",
    "[rtti][class_type][operations][assign]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("assign_class_type"));
  IType& type = descriptor;

  auto source = makeValueA<TestType>();
  auto destination = makeValueB<TestType>();

  type.assign(&destination, &source);
  REQUIRE(destination == source);
}

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor and a null destination, when assign is called through IType, then the operation is "
    "a no-op and the source value is unchanged",
    "[rtti][class_type][operations][assign][negative]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("assign_null_dst_class_type"));
  IType& type = descriptor;

  auto source = makeValueA<TestType>();

  type.assign(nullptr, &source);
  REQUIRE(source == makeValueA<TestType>());
}

// ============================================================================
// equals tests
// ============================================================================

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor and two equal values, when equals is called through IType, then it returns true",
    "[rtti][class_type][operations][equals]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("equals_equal_class_type"));
  const IType& type = descriptor;

  auto lhs = makeValueA<TestType>();
  auto rhs = makeValueA<TestType>();

  REQUIRE(type.equals(&lhs, &rhs));
}

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor and two different values, when equals is called through IType, then it returns "
    "false",
    "[rtti][class_type][operations][equals][negative]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("equals_not_equal_class_type"));
  const IType& type = descriptor;

  auto lhs = makeValueA<TestType>();
  auto rhs = makeValueB<TestType>();

  REQUIRE_FALSE(type.equals(&lhs, &rhs));
}

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor, when equals receives null combinations, then null-null is true and mixed-null is "
    "false",
    "[rtti][class_type][operations][equals][null]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("equals_null_class_type"));
  const IType& type = descriptor;

  auto value = makeValueA<TestType>();

  REQUIRE(type.equals(nullptr, nullptr));
  REQUIRE_FALSE(type.equals(&value, nullptr));
  REQUIRE_FALSE(type.equals(nullptr, &value));
}

// ============================================================================
// allocate / deallocate tests
// ============================================================================

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor, when allocate is called through IType, then returned memory is non-null and "
    "aligned for the underlying type",
    "[rtti][class_type][operations][allocate]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("allocate_class_type"));
  IType& type = descriptor;

  void* allocated = type.allocate();
  REQUIRE(allocated != nullptr);
  REQUIRE(reinterpret_cast<std::uintptr_t>(allocated) % alignof(TestType) == 0);

  type.deallocate(allocated);
}

// ============================================================================
// construct / destruct tests
// ============================================================================

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor and allocated storage, when construct is called through IType, then the object is "
    "default-initialized",
    "[rtti][class_type][operations][construct]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("construct_class_type"));
  IType& type = descriptor;

  void* allocated = type.allocate();
  type.construct(allocated);

  REQUIRE(*static_cast<TestType*>(allocated) == TestType{});

  type.destruct(allocated);
  type.deallocate(allocated);
}

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor and null storage, when construct is called through IType, then the operation is "
    "safe and does nothing",
    "[rtti][class_type][operations][construct][negative]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("construct_null_class_type"));
  IType& type = descriptor;

  type.construct(nullptr);
  SUCCEED();
}

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor, when construct then destruct is called, then the full object lifecycle completes "
    "without error",
    "[rtti][class_type][operations][construct][destruct]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("lifecycle_class_type"));
  IType& type = descriptor;

  void* memory = type.allocate();
  REQUIRE(memory != nullptr);

  type.construct(memory);
  auto& instance = *static_cast<TestType*>(memory);
  instance = makeValueA<TestType>();
  REQUIRE(instance == makeValueA<TestType>());

  type.destruct(memory);
  type.deallocate(memory);
  SUCCEED();
}

// ============================================================================
// create / destroy tests
// ============================================================================

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor, when create is called through IType, then a default-constructed heap object is "
    "returned",
    "[rtti][class_type][operations][create]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("create_class_type"));
  IType& type = descriptor;

  void* created = type.create();
  REQUIRE(created != nullptr);
  REQUIRE(*static_cast<TestType*>(created) == TestType{});

  type.destroy(created);
}

TEMPLATE_TEST_CASE(
    "Given an IClassType descriptor and a null instance pointer, when destroy is called through IType, then the "
    "operation is safe and does nothing",
    "[rtti][class_type][operations][destroy][negative]", TrivialStruct, AlignedStruct, NonTrivialStruct) {
  NamedClassType<TestType> descriptor(IName("destroy_null_class_type"));
  IType& type = descriptor;

  type.destroy(nullptr);
  SUCCEED();
}

// ============================================================================
// Non-trivial type specific tests
// ============================================================================

TEST_CASE(
    "Given a TClassType over a non-trivial type, when assign is called, then copy semantics apply and the source is "
    "not mutated",
    "[rtti][class_type][operations][assign][non_trivial]") {
  TClassType<NonTrivialStruct> descriptor(IName("non_trivial_assign"));
  IType& type = descriptor;

  auto source = NonTrivialStruct{"original"};
  auto destination = NonTrivialStruct{"placeholder"};

  type.assign(&destination, &source);

  REQUIRE(destination.value == "original");
  REQUIRE(source.value == "original");
}

TEST_CASE(
    "Given a TClassType over a non-trivial type, when create is called then destroy is called, then the object "
    "lifetime is managed correctly without leaking resources",
    "[rtti][class_type][operations][create][destroy][non_trivial]") {
  TClassType<NonTrivialStruct> descriptor(IName("non_trivial_create_destroy"));
  IType& type = descriptor;

  void* created = type.create();
  REQUIRE(created != nullptr);

  auto* instance = static_cast<NonTrivialStruct*>(created);
  instance->value = "lifecycle_test";
  REQUIRE(instance->value == "lifecycle_test");

  type.destroy(created);
  SUCCEED();
}

// ============================================================================
// Over-aligned type specific tests
// ============================================================================

TEST_CASE(
    "Given a TClassType over an over-aligned type, when allocate is called, then returned address satisfies the "
    "required alignment",
    "[rtti][class_type][operations][allocate][aligned]") {
  TClassType<AlignedStruct> descriptor(IName("aligned_allocate"));
  IType& type = descriptor;

  void* allocated = type.allocate();
  REQUIRE(allocated != nullptr);
  REQUIRE(reinterpret_cast<std::uintptr_t>(allocated) % alignof(AlignedStruct) == 0);
  REQUIRE(reinterpret_cast<std::uintptr_t>(allocated) % 16 == 0);

  type.deallocate(allocated);
}
