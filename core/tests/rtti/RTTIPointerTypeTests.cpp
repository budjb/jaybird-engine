#include <catch2/catch_test_macros.hpp>

#include "rtti/RTTIClassType.hpp"
#include "rtti/RTTIContainerType.hpp"
#include "rtti/RTTIName.hpp"
#include "rtti/RTTIPointerType.hpp"
#include "types/CString.hpp"

/**
 * @brief A simple class type used as the pointee element for pointer descriptor tests.
 */
struct PointerTarget {
  // ReSharper disable once CppDeclaratorNeverUsed
  int value{0};

  bool operator==(const PointerTarget&) const = default;
};

template <>
struct core::rtti::RTTINameProvider<PointerTarget> {
  static constexpr CString value{"pointer_target"};
};

namespace {
using core::Name;
using core::rtti::RTTIContainerType;
using core::rtti::RTTIPointerType;
using core::rtti::RTTIType;
using core::rtti::RTTITypeKind;
using core::rtti::TypedRTTIClassType;
using core::rtti::TypedRTTIPointerType;
}  // namespace

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor, when kind is queried through RTTIType, then it returns "
    "RTTITypeKind::POINTER",
    "[rtti][pointer_type][metadata]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  REQUIRE(static_cast<const RTTIType&>(descriptor).kind() == RTTITypeKind::POINTER);
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor, when size and alignment are queried, then they match the represented "
    "raw pointer type",
    "[rtti][pointer_type][metadata]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  REQUIRE(static_cast<const RTTIType&>(descriptor).size() == sizeof(PointerTarget*));
  REQUIRE(static_cast<const RTTIType&>(descriptor).alignment() == alignof(PointerTarget*));
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor, when name is queried, then it is the inner type name prefixed with ptr:",
    "[rtti][pointer_type][metadata]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  REQUIRE(static_cast<const RTTIType&>(descriptor).name() == Name("ptr:pointer_target"));
  REQUIRE(static_cast<const RTTIType&>(descriptor).name().toString() == "ptr:pointer_target");
}

TEST_CASE("Given a TypedRTTIPointerType descriptor, when asArray is called through RTTIType, then it returns nullptr",
          "[rtti][pointer_type][metadata]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  REQUIRE(static_cast<const RTTIType&>(descriptor).asArray() == nullptr);
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor, when cast to RTTIPointerType, RTTIContainerType, and RTTIType, then "
    "all casts are non-null",
    "[rtti][pointer_type][hierarchy]") {
  const TypedRTTIClassType<PointerTarget> inner;
  TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  REQUIRE(static_cast<RTTIPointerType*>(&descriptor) != nullptr);
  REQUIRE(static_cast<RTTIContainerType*>(&descriptor) != nullptr);
  REQUIRE(static_cast<RTTIType*>(&descriptor) != nullptr);
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor, when inner is queried, then it returns a pointer equal to the inner "
    "descriptor",
    "[rtti][pointer_type][inner]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  REQUIRE(descriptor.inner() == static_cast<const RTTIType*>(&inner));
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor and valid source and destination pointers, when assign is called, then "
    "the destination pointer value matches the source pointer value",
    "[rtti][pointer_type][operations][assign]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  PointerTarget sourceObject{11};
  PointerTarget destinationObject{22};
  PointerTarget* source = &sourceObject;
  PointerTarget* destination = &destinationObject;

  descriptor.assign(&destination, &source);

  REQUIRE(destination == source);
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor and a null destination, when assign is called, then it is a safe no-op",
    "[rtti][pointer_type][operations][assign][negative]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  PointerTarget sourceObject{7};
  PointerTarget* source = &sourceObject;

  REQUIRE_NOTHROW(descriptor.assign(nullptr, &source));
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor and two equal pointer values, when equals is called, then it returns true",
    "[rtti][pointer_type][operations][equals]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  PointerTarget object{1};
  PointerTarget* a = &object;
  PointerTarget* b = &object;

  REQUIRE(descriptor.equals(&a, &b));
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor and two different pointer values, when equals is called, then it returns "
    "false",
    "[rtti][pointer_type][operations][equals][negative]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  PointerTarget aObject{1};
  PointerTarget bObject{1};
  PointerTarget* a = &aObject;
  PointerTarget* b = &bObject;

  REQUIRE_FALSE(descriptor.equals(&a, &b));
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor, when equals is called with both void pointer arguments null, then it "
    "returns true",
    "[rtti][pointer_type][operations][equals]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  REQUIRE(descriptor.equals(nullptr, nullptr));
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor and a valid pointer value, when equals is called with one null argument, "
    "then it returns false",
    "[rtti][pointer_type][operations][equals][negative]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  PointerTarget object{3};
  PointerTarget* value = &object;

  REQUIRE_FALSE(descriptor.equals(nullptr, &value));
  REQUIRE_FALSE(descriptor.equals(&value, nullptr));
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor, when allocate is called, then the returned pointer is non-null and "
    "correctly aligned for the represented pointer type",
    "[rtti][pointer_type][operations][allocate]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  void* memory = descriptor.allocate();
  REQUIRE(memory != nullptr);
  REQUIRE(reinterpret_cast<std::uintptr_t>(memory) % alignof(PointerTarget*) == 0);

  descriptor.deallocate(memory);
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor and memory obtained from allocate, when construct is called, then the "
    "resulting pointer value is null",
    "[rtti][pointer_type][operations][construct]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  void* memory = descriptor.allocate();
  descriptor.construct(memory);

  REQUIRE(*static_cast<PointerTarget**>(memory) == nullptr);

  descriptor.destruct(memory);
  descriptor.deallocate(memory);
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor and storage from allocate and construct, when destruct is called, then it "
    "completes without error",
    "[rtti][pointer_type][operations][destruct]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  void* memory = descriptor.allocate();
  descriptor.construct(memory);

  REQUIRE_NOTHROW(descriptor.destruct(memory));

  descriptor.deallocate(memory);
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor, when create is called, then the returned pointer is non-null and points "
    "to a null raw-pointer value",
    "[rtti][pointer_type][operations][create]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  void* instance = descriptor.create();
  REQUIRE(instance != nullptr);
  REQUIRE(*static_cast<PointerTarget**>(instance) == nullptr);

  descriptor.destroy(instance);
}

TEST_CASE(
    "Given a TypedRTTIPointerType descriptor and an instance from create, when destroy is called, then it completes "
    "without error",
    "[rtti][pointer_type][operations][destroy]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  void* instance = descriptor.create();

  REQUIRE_NOTHROW(descriptor.destroy(instance));
}

TEST_CASE("Given a TypedRTTIPointerType descriptor, when destroy is called with nullptr, then it is a safe no-op",
          "[rtti][pointer_type][operations][destroy][negative]") {
  const TypedRTTIClassType<PointerTarget> inner;
  const TypedRTTIPointerType<PointerTarget*> descriptor(&inner);

  REQUIRE_NOTHROW(descriptor.destroy(nullptr));
}
