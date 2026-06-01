#include <catch2/catch_test_macros.hpp>
#include <memory>

#include "rtti/RTTIClassType.hpp"
#include "rtti/RTTIName.hpp"
#include "rtti/RTTIRefType.hpp"
#include "types/CString.hpp"

/**
 * @brief A simple class type used as the managed element for RefType tests.
 */
struct RefTarget {
  int value{0};

  bool operator==(const RefTarget&) const = default;
};

template <>
struct core::rtti::RTTINameProvider<RefTarget> {
  static constexpr CString value{"ref_target"};
};  // namespace core::rtti

namespace {
using core::Name;
using core::rtti::RTTIContainerType;
using core::rtti::RTTIRefType;
using core::rtti::RTTIType;
using core::rtti::RTTITypeKind;
using core::rtti::TypedRTTIClassType;
using core::rtti::TypedRTTIRefType;
}  // namespace

// =============================================================================
// Metadata
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, when kind is queried through RTTIType, then it returns RTTITypeKind::REF",
    "[rtti][ref_type][metadata]") {
  const TypedRTTIClassType<RefTarget> inner;
  TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE(static_cast<RTTIType&>(descriptor).kind() == RTTITypeKind::REF);
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, when size is queried through RTTIType, then it equals sizeof std::shared_ptr",
    "[rtti][ref_type][metadata]") {
  const TypedRTTIClassType<RefTarget> inner;
  TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE(static_cast<RTTIType&>(descriptor).size() == sizeof(std::shared_ptr<RefTarget>));
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, when alignment is queried through RTTIType, then it equals alignof "
    "std::shared_ptr",
    "[rtti][ref_type][metadata]") {
  const TypedRTTIClassType<RefTarget> inner;
  TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE(static_cast<RTTIType&>(descriptor).alignment() == alignof(std::shared_ptr<RefTarget>));
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, when name is queried, then it is the inner type name prefixed with ref:",
    "[rtti][ref_type][metadata]") {
  const TypedRTTIClassType<RefTarget> inner;
  TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE(static_cast<RTTIType&>(descriptor).name() == Name("ref:ref_target"));
  REQUIRE(static_cast<RTTIType&>(descriptor).name().toString() == "ref:ref_target");
}

TEST_CASE("Given a TypedRTTIRefType descriptor, when asArray is called through RTTIType, then it returns nullptr",
          "[rtti][ref_type][metadata]") {
  const TypedRTTIClassType<RefTarget> inner;
  TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE(static_cast<RTTIType&>(descriptor).asArray() == nullptr);
}

// =============================================================================
// Hierarchy
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, when cast to RTTIRefType, RTTIContainerType, and RTTIType, then all casts "
    "are non-null",
    "[rtti][ref_type][hierarchy]") {
  const TypedRTTIClassType<RefTarget> inner;
  TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE(static_cast<RTTIRefType*>(&descriptor) != nullptr);
  REQUIRE(static_cast<RTTIContainerType*>(&descriptor) != nullptr);
  REQUIRE(static_cast<RTTIType*>(&descriptor) != nullptr);
}

// =============================================================================
// Inner descriptor
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, when inner is queried, then it returns a pointer equal to the inner "
    "descriptor",
    "[rtti][ref_type][inner]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE(descriptor.inner() == static_cast<const RTTIType*>(&inner));
}

// =============================================================================
// assign
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and valid source and destination shared_ptrs, when assign is called, then the "
    "destination shares the same managed object as the source",
    "[rtti][ref_type][operations][assign]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto source = std::make_shared<RefTarget>(RefTarget{42});
  std::shared_ptr<RefTarget> destination;

  descriptor.assign(&destination, &source);

  REQUIRE(destination.get() == source.get());
  REQUIRE(source.use_count() == 2);
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a null destination, when assign is called, then it is a no-op and the "
    "source use count remains one",
    "[rtti][ref_type][operations][assign][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto source = std::make_shared<RefTarget>(RefTarget{7});

  descriptor.assign(nullptr, &source);

  REQUIRE(source.use_count() == 1);
}

// =============================================================================
// equals
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and two shared_ptrs managing the same object, when equals is called, then it "
    "returns true",
    "[rtti][ref_type][operations][equals]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto a = std::make_shared<RefTarget>(RefTarget{1});
  const auto b = a;

  REQUIRE(descriptor.equals(&a, &b));
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and two shared_ptrs managing distinct objects with the same value, when "
    "equals is called, then it returns false",
    "[rtti][ref_type][operations][equals][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto a = std::make_shared<RefTarget>(RefTarget{1});
  const auto b = std::make_shared<RefTarget>(RefTarget{1});

  REQUIRE_FALSE(descriptor.equals(&a, &b));
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and two default-constructed empty shared_ptrs, when equals is called, then it "
    "returns true",
    "[rtti][ref_type][operations][equals]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const std::shared_ptr<RefTarget> a;
  const std::shared_ptr<RefTarget> b;

  REQUIRE(descriptor.equals(&a, &b));
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, when equals is called with both void pointer arguments null, then it returns "
    "true",
    "[rtti][ref_type][operations][equals]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE(descriptor.equals(nullptr, nullptr));
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a valid shared_ptr, when equals is called with a null left void pointer, "
    "then it returns false",
    "[rtti][ref_type][operations][equals][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto a = std::make_shared<RefTarget>();

  REQUIRE_FALSE(descriptor.equals(nullptr, &a));
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a valid shared_ptr, when equals is called with a null right void pointer, "
    "then it returns false",
    "[rtti][ref_type][operations][equals][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto a = std::make_shared<RefTarget>();

  REQUIRE_FALSE(descriptor.equals(&a, nullptr));
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, a non-empty shared_ptr, and an empty shared_ptr, when equals is called, then "
    "it returns false",
    "[rtti][ref_type][operations][equals][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto populated = std::make_shared<RefTarget>(RefTarget{5});
  const std::shared_ptr<RefTarget> empty;

  REQUIRE_FALSE(descriptor.equals(&populated, &empty));
}

// =============================================================================
// allocate / deallocate
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, when allocate is called, then the returned pointer is non-null and correctly "
    "aligned for std::shared_ptr",
    "[rtti][ref_type][operations][allocate]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  void* mem = descriptor.allocate();
  REQUIRE(mem != nullptr);
  REQUIRE(reinterpret_cast<std::uintptr_t>(mem) % alignof(std::shared_ptr<RefTarget>) == 0);

  descriptor.deallocate(mem);
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and memory obtained from allocate, when deallocate is called, then it "
    "completes without error",
    "[rtti][ref_type][operations][allocate]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  void* mem = descriptor.allocate();
  REQUIRE_NOTHROW(descriptor.deallocate(mem));
}

// =============================================================================
// construct / destruct
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and pre-allocated storage, when construct is called, then the resulting "
    "shared_ptr is empty",
    "[rtti][ref_type][operations][construct]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  void* mem = descriptor.allocate();
  descriptor.construct(mem);

  REQUIRE(*static_cast<std::shared_ptr<RefTarget>*>(mem) == nullptr);

  descriptor.destruct(mem);
  descriptor.deallocate(mem);
}

TEST_CASE("Given a TypedRTTIRefType descriptor, when construct is called with nullptr, then it is a safe no-op",
          "[rtti][ref_type][operations][construct][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE_NOTHROW(descriptor.construct(nullptr));
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and allocated and constructed storage, when destruct is called, then it "
    "completes without error",
    "[rtti][ref_type][operations][destruct]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  void* mem = descriptor.allocate();
  descriptor.construct(mem);
  REQUIRE_NOTHROW(descriptor.destruct(mem));
  descriptor.deallocate(mem);
}

// =============================================================================
// create / destroy
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, when create is called, then the returned pointer is non-null and points to "
    "an empty shared_ptr",
    "[rtti][ref_type][operations][create]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  void* instance = descriptor.create();
  REQUIRE(instance != nullptr);
  REQUIRE(*static_cast<std::shared_ptr<RefTarget>*>(instance) == nullptr);

  descriptor.destroy(instance);
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and an instance obtained from create, when destroy is called, then it "
    "completes without error",
    "[rtti][ref_type][operations][destroy]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  void* instance = descriptor.create();
  REQUIRE_NOTHROW(descriptor.destroy(instance));
}

TEST_CASE("Given a TypedRTTIRefType descriptor, when destroy is called with nullptr, then it is a safe no-op",
          "[rtti][ref_type][operations][destroy][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE_NOTHROW(descriptor.destroy(nullptr));
}

// =============================================================================
// Shared ownership / reference counting
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a shared_ptr with use count one, when assign copies it into a second "
    "descriptor-managed slot and then that slot is destructed, then the original use count returns to one",
    "[rtti][ref_type][shared_ownership]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto original = std::make_shared<RefTarget>(RefTarget{99});
  REQUIRE(original.use_count() == 1);

  void* mem = descriptor.allocate();
  descriptor.construct(mem);
  descriptor.assign(mem, &original);

  REQUIRE(original.use_count() == 2);

  descriptor.destruct(mem);
  descriptor.deallocate(mem);

  REQUIRE(original.use_count() == 1);
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a shared_ptr whose original owner goes out of scope, when the "
    "descriptor-managed copy still holds a reference, then the managed object remains alive",
    "[rtti][ref_type][shared_ownership]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  void* instance = descriptor.create();
  auto* slot = static_cast<std::shared_ptr<RefTarget>*>(instance);

  {
    const auto temp = std::make_shared<RefTarget>(RefTarget{77});
    descriptor.assign(instance, &temp);
    REQUIRE(temp.use_count() == 2);
  }

  // temp is destroyed; the descriptor slot is the sole remaining owner
  REQUIRE(*slot != nullptr);
  REQUIRE((*slot)->value == 77);
  REQUIRE(slot->use_count() == 1);

  descriptor.destroy(instance);
}

// =============================================================================
// get
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a non-empty shared_ptr, when get is called, then the returned void "
    "pointer equals the raw pointer managed by the shared_ptr",
    "[rtti][ref_type][operations][get]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  auto shared = std::make_shared<RefTarget>(RefTarget{10});
  REQUIRE(descriptor.get(&shared) == shared.get());
}

TEST_CASE("Given a TypedRTTIRefType descriptor and an empty shared_ptr, when get is called, then it returns nullptr",
          "[rtti][ref_type][operations][get]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  std::shared_ptr<RefTarget> empty;
  REQUIRE(descriptor.get(&empty) == nullptr);
}

TEST_CASE("Given a TypedRTTIRefType descriptor, when get is called with a null instance, then it returns nullptr",
          "[rtti][ref_type][operations][get][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE(descriptor.get(nullptr) == nullptr);
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a non-empty shared_ptr, when typed get<RefTarget> is called, then the "
    "returned pointer is correctly typed and equals the managed raw pointer",
    "[rtti][ref_type][operations][get]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);
  const RTTIRefType* poly = &descriptor;

  auto shared = std::make_shared<RefTarget>(RefTarget{20});
  auto* raw = poly->get<RefTarget>(&shared);

  REQUIRE(raw != nullptr);
  REQUIRE(raw == shared.get());
  REQUIRE(raw->value == 20);
}

// =============================================================================
// reset
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a non-empty shared_ptr, when reset is called, then the shared_ptr becomes "
    "empty",
    "[rtti][ref_type][operations][reset]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  auto shared = std::make_shared<RefTarget>(RefTarget{5});
  descriptor.reset(&shared);

  REQUIRE(shared == nullptr);
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a shared_ptr with two owners, when reset is called on one, then the other "
    "owner's use count drops to one",
    "[rtti][ref_type][operations][reset]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto original = std::make_shared<RefTarget>(RefTarget{9});
  auto copy = original;
  REQUIRE(original.use_count() == 2);

  descriptor.reset(&copy);

  REQUIRE(original.use_count() == 1);
}

TEST_CASE("Given a TypedRTTIRefType descriptor, when reset is called with a null instance, then it is a safe no-op",
          "[rtti][ref_type][operations][reset][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE_NOTHROW(descriptor.reset(nullptr));
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and an already-empty shared_ptr, when reset is called, then it remains empty",
    "[rtti][ref_type][operations][reset]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  std::shared_ptr<RefTarget> empty;
  descriptor.reset(&empty);

  REQUIRE(empty == nullptr);
}

// =============================================================================
// useCount
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a sole-owner shared_ptr, when useCount is called, then it returns one",
    "[rtti][ref_type][operations][use_count]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto shared = std::make_shared<RefTarget>();
  REQUIRE(descriptor.useCount(&shared) == 1);
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and a shared_ptr shared between two owners, when useCount is called, then it "
    "returns two",
    "[rtti][ref_type][operations][use_count]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const auto original = std::make_shared<RefTarget>();
  const auto copy = original;
  REQUIRE(descriptor.useCount(&original) == 2);
}

TEST_CASE("Given a TypedRTTIRefType descriptor and an empty shared_ptr, when useCount is called, then it returns zero",
          "[rtti][ref_type][operations][use_count]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  const std::shared_ptr<RefTarget> empty;
  REQUIRE(descriptor.useCount(&empty) == 0);
}

TEST_CASE("Given a TypedRTTIRefType descriptor, when useCount is called with a null instance, then it returns zero",
          "[rtti][ref_type][operations][use_count][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE(descriptor.useCount(nullptr) == 0);
}

// =============================================================================
// swap
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIRefType descriptor and two non-empty shared_ptrs, when swap is called, then their managed "
    "objects are exchanged",
    "[rtti][ref_type][operations][swap]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  auto a = std::make_shared<RefTarget>(RefTarget{1});
  auto b = std::make_shared<RefTarget>(RefTarget{2});
  RefTarget* rawA = a.get();
  RefTarget* rawB = b.get();

  descriptor.swap(&a, &b);

  REQUIRE(a.get() == rawB);
  REQUIRE(b.get() == rawA);
}

TEST_CASE("Given a TypedRTTIRefType descriptor and two empty shared_ptrs, when swap is called, then both remain empty",
          "[rtti][ref_type][operations][swap]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  std::shared_ptr<RefTarget> a;
  std::shared_ptr<RefTarget> b;
  descriptor.swap(&a, &b);

  REQUIRE(a == nullptr);
  REQUIRE(b == nullptr);
}

TEST_CASE("Given a TypedRTTIRefType descriptor, when swap is called with a null lhs, then rhs is unchanged",
          "[rtti][ref_type][operations][swap][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  auto b = std::make_shared<RefTarget>(RefTarget{7});
  RefTarget* rawB = b.get();

  descriptor.swap(nullptr, &b);

  REQUIRE(b.get() == rawB);
}

TEST_CASE("Given a TypedRTTIRefType descriptor, when swap is called with a null rhs, then lhs is unchanged",
          "[rtti][ref_type][operations][swap][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  auto a = std::make_shared<RefTarget>(RefTarget{8});
  RefTarget* rawA = a.get();

  descriptor.swap(&a, nullptr);

  REQUIRE(a.get() == rawA);
}

TEST_CASE(
    "Given a TypedRTTIRefType descriptor, when swap is called with both lhs and rhs null, then it completes without "
    "error",
    "[rtti][ref_type][operations][swap][negative]") {
  const TypedRTTIClassType<RefTarget> inner;
  const TypedRTTIRefType<RefTarget> descriptor(&inner);

  REQUIRE_NOTHROW(descriptor.swap(nullptr, nullptr));
}
