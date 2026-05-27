#include <catch2/catch_test_macros.hpp>
#include <memory>

#include "CString.hpp"
#include "rtti/ClassType.hpp"
#include "rtti/TypeName.hpp"
#include "rtti/WeakRefType.hpp"

/**
 * @brief A simple class type used as the managed element for WeakRefType tests.
 */
struct WeakTarget {
  // ReSharper disable once CppDeclaratorNeverUsed
  int value{0};

  bool operator==(const WeakTarget&) const = default;
};

template <>
struct core::rtti::TypeName<WeakTarget> {
  static constexpr CString value{"weak_target"};
};  // namespace core::rtti

namespace {
using core::IName;
using core::rtti::IContainerType;
using core::rtti::IType;
using core::rtti::IWeakRefType;
using core::rtti::TClassType;
using core::rtti::TWeakRefType;
using core::rtti::TypeKind;
}  // namespace

// =============================================================================
// Metadata
// =============================================================================

TEST_CASE("Given a TWeakRefType descriptor, when kind is queried through IType, then it returns TypeKind::WEAK_REF",
          "[rtti][weak_ref_type][metadata]") {
  const TClassType<WeakTarget> inner(IName("wkind_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE(static_cast<IType&>(descriptor).kind() == TypeKind::WEAK_REF);
}

TEST_CASE("Given a TWeakRefType descriptor, when size is queried through IType, then it equals sizeof std::weak_ptr",
          "[rtti][weak_ref_type][metadata]") {
  const TClassType<WeakTarget> inner(IName("wsize_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE(static_cast<IType&>(descriptor).size() == sizeof(std::weak_ptr<WeakTarget>));
}

TEST_CASE(
    "Given a TWeakRefType descriptor, when alignment is queried through IType, then it equals alignof std::weak_ptr",
    "[rtti][weak_ref_type][metadata]") {
  const TClassType<WeakTarget> inner(IName("walign_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE(static_cast<IType&>(descriptor).alignment() == alignof(std::weak_ptr<WeakTarget>));
}

TEST_CASE("Given a TWeakRefType descriptor, when name is queried, then it is the inner type name prefixed with wref:",
          "[rtti][weak_ref_type][metadata]") {
  const TClassType<WeakTarget> inner(IName("wname_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE(static_cast<IType&>(descriptor).name() == IName("wref:weak_target"));
}

TEST_CASE("Given a TWeakRefType descriptor, when asArray is called through IType, then it returns nullptr",
          "[rtti][weak_ref_type][metadata]") {
  const TClassType<WeakTarget> inner(IName("was_array_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE(static_cast<IType&>(descriptor).asArray() == nullptr);
}

// =============================================================================
// Hierarchy
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor, when cast to IWeakRefType, IContainerType, and IType, then all casts are "
    "non-null",
    "[rtti][weak_ref_type][hierarchy]") {
  const TClassType<WeakTarget> inter(IName("whier_inner"));
  TWeakRefType<WeakTarget> descriptor(&inter);

  REQUIRE(static_cast<IWeakRefType*>(&descriptor) != nullptr);
  REQUIRE(static_cast<IContainerType*>(&descriptor) != nullptr);
  REQUIRE(static_cast<IType*>(&descriptor) != nullptr);
}

// =============================================================================
// Inner descriptor
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor, when inner is queried, then it returns a pointer equal to the inner descriptor",
    "[rtti][weak_ref_type][inner]") {
  const TClassType<WeakTarget> inner(IName("winner_query_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE(descriptor.inner() == static_cast<const IType*>(&inner));
}

// =============================================================================
// assign
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor and valid source and destination weak_ptrs, when assign is called, then the "
    "destination refers to the same managed object as the source",
    "[rtti][weak_ref_type][operations][assign]") {
  const TClassType<WeakTarget> inner(IName("wassign_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  const auto shared = std::make_shared<WeakTarget>(WeakTarget{42});
  const std::weak_ptr source{shared};
  std::weak_ptr<WeakTarget> destination;

  descriptor.assign(&destination, &source);

  REQUIRE(destination.lock().get() == source.lock().get());
}

TEST_CASE("Given a TWeakRefType descriptor and a null destination, when assign is called, then it is a no-op",
          "[rtti][weak_ref_type][operations][assign][negative]") {
  const TClassType<WeakTarget> inner(IName("wassign_null_dst_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  const auto shared = std::make_shared<WeakTarget>(WeakTarget{7});
  const std::weak_ptr source{shared};

  descriptor.assign(nullptr, &source);

  REQUIRE(shared.use_count() == 1);
}

TEST_CASE(
    "Given a TWeakRefType descriptor and a valid source weak_ptr, when assign is called, then the strong use count "
    "of the managed object is unchanged",
    "[rtti][weak_ref_type][operations][assign]") {
  const TClassType<WeakTarget> inner(IName("wassign_refcount_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  const auto shared = std::make_shared<WeakTarget>(WeakTarget{99});
  const std::weak_ptr source{shared};
  std::weak_ptr<WeakTarget> destination;

  REQUIRE(shared.use_count() == 1);
  descriptor.assign(&destination, &source);
  REQUIRE(shared.use_count() == 1);
}

// =============================================================================
// equals
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor and two weak_ptrs from the same shared_ptr, when equals is called, then it "
    "returns true",
    "[rtti][weak_ref_type][operations][equals]") {
  const TClassType<WeakTarget> inner(IName("wequals_same_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto shared = std::make_shared<WeakTarget>(WeakTarget{1});
  const std::weak_ptr a{shared};
  const std::weak_ptr b{shared};

  REQUIRE(descriptor.equals(&a, &b));
}

TEST_CASE(
    "Given a TWeakRefType descriptor and two weak_ptrs from distinct shared_ptrs, when equals is called, then it "
    "returns false",
    "[rtti][weak_ref_type][operations][equals][negative]") {
  const TClassType<WeakTarget> inner(IName("wequals_diff_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto sa = std::make_shared<WeakTarget>(WeakTarget{1});
  const auto sb = std::make_shared<WeakTarget>(WeakTarget{1});
  const std::weak_ptr a{sa};
  const std::weak_ptr b{sb};

  REQUIRE_FALSE(descriptor.equals(&a, &b));
}

TEST_CASE(
    "Given a TWeakRefType descriptor and two default-constructed (expired) weak_ptrs, when equals is called, then it "
    "returns true",
    "[rtti][weak_ref_type][operations][equals]") {
  const TClassType<WeakTarget> inner(IName("wequals_expired_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const std::weak_ptr<WeakTarget> a;
  const std::weak_ptr<WeakTarget> b;

  REQUIRE(descriptor.equals(&a, &b));
}

TEST_CASE(
    "Given a TWeakRefType descriptor, when equals is called with both void pointer arguments null, then it returns "
    "true",
    "[rtti][weak_ref_type][operations][equals]") {
  const TClassType<WeakTarget> inner(IName("wequals_null_ptrs_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE(descriptor.equals(nullptr, nullptr));
}

TEST_CASE(
    "Given a TWeakRefType descriptor and a valid weak_ptr, when equals is called with a null left void pointer, "
    "then it returns false",
    "[rtti][weak_ref_type][operations][equals][negative]") {
  const TClassType<WeakTarget> inner(IName("wequals_lhs_null_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto shared = std::make_shared<WeakTarget>();
  const std::weak_ptr a{shared};

  REQUIRE_FALSE(descriptor.equals(nullptr, &a));
}

TEST_CASE(
    "Given a TWeakRefType descriptor and a valid weak_ptr, when equals is called with a null right void pointer, "
    "then it returns false",
    "[rtti][weak_ref_type][operations][equals][negative]") {
  const TClassType<WeakTarget> inner(IName("wequals_rhs_null_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto shared = std::make_shared<WeakTarget>();
  const std::weak_ptr a{shared};

  REQUIRE_FALSE(descriptor.equals(&a, nullptr));
}

TEST_CASE(
    "Given a TWeakRefType descriptor, an expired weak_ptr, and a non-expired weak_ptr, when equals is called, then "
    "it returns false",
    "[rtti][weak_ref_type][operations][equals][negative]") {
  const TClassType<WeakTarget> inner(IName("wequals_expired_vs_live_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto shared = std::make_shared<WeakTarget>(WeakTarget{5});
  const std::weak_ptr live{shared};
  const std::weak_ptr<WeakTarget> expired;

  REQUIRE_FALSE(descriptor.equals(&live, &expired));
}

// =============================================================================
// allocate / deallocate
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor, when allocate is called, then the returned pointer is non-null and correctly "
    "aligned for std::weak_ptr",
    "[rtti][weak_ref_type][operations][allocate]") {
  const TClassType<WeakTarget> inner(IName("walloc_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  void* mem = descriptor.allocate();
  REQUIRE(mem != nullptr);
  REQUIRE(reinterpret_cast<std::uintptr_t>(mem) % alignof(std::weak_ptr<WeakTarget>) == 0);

  descriptor.deallocate(mem);
}

TEST_CASE(
    "Given a TWeakRefType descriptor and memory obtained from allocate, when deallocate is called, then it completes "
    "without error",
    "[rtti][weak_ref_type][operations][allocate]") {
  const TClassType<WeakTarget> inner(IName("wdealloc_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  void* mem = descriptor.allocate();
  REQUIRE_NOTHROW(descriptor.deallocate(mem));
}

// =============================================================================
// construct / destruct
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor and pre-allocated storage, when construct is called, then the resulting "
    "weak_ptr is expired",
    "[rtti][weak_ref_type][operations][construct]") {
  const TClassType<WeakTarget> inner(IName("wconstruct_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  void* mem = descriptor.allocate();
  descriptor.construct(mem);

  REQUIRE(static_cast<std::weak_ptr<WeakTarget>*>(mem)->expired());

  descriptor.destruct(mem);
  descriptor.deallocate(mem);
}

TEST_CASE("Given a TWeakRefType descriptor, when construct is called with nullptr, then it is a safe no-op",
          "[rtti][weak_ref_type][operations][construct][negative]") {
  const TClassType<WeakTarget> inner(IName("wconstruct_null_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE_NOTHROW(descriptor.construct(nullptr));
}

TEST_CASE(
    "Given a TWeakRefType descriptor and allocated and constructed storage, when destruct is called, then it "
    "completes without error",
    "[rtti][weak_ref_type][operations][destruct]") {
  const TClassType<WeakTarget> inner(IName("wdestruct_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  void* mem = descriptor.allocate();
  descriptor.construct(mem);
  REQUIRE_NOTHROW(descriptor.destruct(mem));
  descriptor.deallocate(mem);
}

// =============================================================================
// create / destroy
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor, when create is called, then the returned pointer is non-null and points to an "
    "expired weak_ptr",
    "[rtti][weak_ref_type][operations][create]") {
  const TClassType<WeakTarget> inner(IName("wcreate_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  void* instance = descriptor.create();
  REQUIRE(instance != nullptr);
  REQUIRE(static_cast<std::weak_ptr<WeakTarget>*>(instance)->expired());

  descriptor.destroy(instance);
}

TEST_CASE(
    "Given a TWeakRefType descriptor and an instance obtained from create, when destroy is called, then it completes "
    "without error",
    "[rtti][weak_ref_type][operations][destroy]") {
  const TClassType<WeakTarget> inner(IName("wdestroy_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  void* instance = descriptor.create();
  REQUIRE_NOTHROW(descriptor.destroy(instance));
}

TEST_CASE("Given a TWeakRefType descriptor, when destroy is called with nullptr, then it is a safe no-op",
          "[rtti][weak_ref_type][operations][destroy][negative]") {
  const TClassType<WeakTarget> inner(IName("wdestroy_null_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE_NOTHROW(descriptor.destroy(nullptr));
}

// =============================================================================
// Weak reference semantics
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor and a weak_ptr assigned from a shared_ptr, when the shared_ptr is destroyed, "
    "then the weak_ptr becomes expired",
    "[rtti][weak_ref_type][weak_semantics]") {
  const TClassType<WeakTarget> inner(IName("wexpiry_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  void* instance = descriptor.create();
  const auto* slot = static_cast<std::weak_ptr<WeakTarget>*>(instance);

  {
    const auto shared = std::make_shared<WeakTarget>(WeakTarget{55});
    const std::weak_ptr weak{shared};
    descriptor.assign(instance, &weak);
    REQUIRE_FALSE(slot->expired());
  }

  // shared is destroyed; no strong owners remain
  REQUIRE(slot->expired());

  descriptor.destroy(instance);
}

TEST_CASE(
    "Given a TWeakRefType descriptor and a weak_ptr copied via assign, when the original shared_ptr still exists, "
    "then both weak_ptrs lock to the same object and the strong use count remains one",
    "[rtti][weak_ref_type][weak_semantics]") {
  const TClassType<WeakTarget> inner(IName("wshared_ownership_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  const auto shared = std::make_shared<WeakTarget>(WeakTarget{77});
  const std::weak_ptr source{shared};

  void* mem = descriptor.allocate();
  descriptor.construct(mem);
  descriptor.assign(mem, &source);

  const auto* copy = static_cast<std::weak_ptr<WeakTarget>*>(mem);
  REQUIRE(copy->lock().get() == shared.get());
  REQUIRE(shared.use_count() == 1);

  descriptor.destruct(mem);
  descriptor.deallocate(mem);
}

// =============================================================================
// reset
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor and a non-expired weak_ptr, when reset is called, then the weak_ptr becomes "
    "expired",
    "[rtti][weak_ref_type][operations][reset]") {
  const TClassType<WeakTarget> inner(IName("wreset_nonempty_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  auto shared = std::make_shared<WeakTarget>(WeakTarget{5});
  std::weak_ptr weak{shared};
  REQUIRE_FALSE(weak.expired());

  descriptor.reset(&weak);

  REQUIRE(weak.expired());
}

TEST_CASE("Given a TWeakRefType descriptor, when reset is called with a null instance, then it is a safe no-op",
          "[rtti][weak_ref_type][operations][reset][negative]") {
  const TClassType<WeakTarget> inner(IName("wreset_null_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE_NOTHROW(descriptor.reset(nullptr));
}

TEST_CASE(
    "Given a TWeakRefType descriptor and an already-expired weak_ptr, when reset is called, then it remains expired",
    "[rtti][weak_ref_type][operations][reset]") {
  const TClassType<WeakTarget> inner(IName("wreset_already_expired_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  std::weak_ptr<WeakTarget> expired;
  descriptor.reset(&expired);

  REQUIRE(expired.expired());
}

TEST_CASE(
    "Given a TWeakRefType descriptor and a non-expired weak_ptr, when reset is called, then the original shared_ptr "
    "use count is unchanged",
    "[rtti][weak_ref_type][operations][reset]") {
  const TClassType<WeakTarget> inner(IName("wreset_usecount_inner"));
  TWeakRefType<WeakTarget> descriptor(&inner);

  auto shared = std::make_shared<WeakTarget>(WeakTarget{7});
  std::weak_ptr weak{shared};
  REQUIRE(shared.use_count() == 1);

  descriptor.reset(&weak);

  REQUIRE(shared.use_count() == 1);
}

// =============================================================================
// swap
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor and two non-expired weak_ptrs from different shared_ptrs, when swap is called, "
    "then their managed object references are exchanged",
    "[rtti][weak_ref_type][operations][swap]") {
  const TClassType<WeakTarget> inner(IName("wswap_exchange_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto a_shared = std::make_shared<WeakTarget>(WeakTarget{1});
  const auto b_shared = std::make_shared<WeakTarget>(WeakTarget{2});
  std::weak_ptr a{a_shared};
  std::weak_ptr b{b_shared};

  descriptor.swap(&a, &b);

  REQUIRE(a.lock().get() == b_shared.get());
  REQUIRE(b.lock().get() == a_shared.get());
}

TEST_CASE("Given a TWeakRefType descriptor and two expired weak_ptrs, when swap is called, then both remain expired",
          "[rtti][weak_ref_type][operations][swap]") {
  const TClassType<WeakTarget> inner(IName("wswap_both_expired_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  std::weak_ptr<WeakTarget> a;
  std::weak_ptr<WeakTarget> b;
  descriptor.swap(&a, &b);

  REQUIRE(a.expired());
  REQUIRE(b.expired());
}

TEST_CASE("Given a TWeakRefType descriptor, when swap is called with a null lhs, then rhs is unchanged",
          "[rtti][weak_ref_type][operations][swap][negative]") {
  const TClassType<WeakTarget> inner(IName("wswap_null_lhs_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto b_shared = std::make_shared<WeakTarget>(WeakTarget{7});
  std::weak_ptr b{b_shared};
  WeakTarget* rawB = b.lock().get();

  descriptor.swap(nullptr, &b);

  REQUIRE(b.lock().get() == rawB);
}

TEST_CASE("Given a TWeakRefType descriptor, when swap is called with a null rhs, then lhs is unchanged",
          "[rtti][weak_ref_type][operations][swap][negative]") {
  const TClassType<WeakTarget> inner(IName("wswap_null_rhs_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto a_shared = std::make_shared<WeakTarget>(WeakTarget{8});
  std::weak_ptr a{a_shared};
  WeakTarget* rawA = a.lock().get();

  descriptor.swap(&a, nullptr);

  REQUIRE(a.lock().get() == rawA);
}

TEST_CASE(
    "Given a TWeakRefType descriptor, when swap is called with both lhs and rhs null, then it completes without "
    "error",
    "[rtti][weak_ref_type][operations][swap][negative]") {
  const TClassType<WeakTarget> inner(IName("wswap_both_null_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE_NOTHROW(descriptor.swap(nullptr, nullptr));
}

// =============================================================================
// expired
// =============================================================================

TEST_CASE("Given a TWeakRefType descriptor and a non-expired weak_ptr, when expired is called, then it returns false",
          "[rtti][weak_ref_type][operations][expired]") {
  const TClassType<WeakTarget> inner(IName("wexpired_live_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto shared = std::make_shared<WeakTarget>();
  const std::weak_ptr weak{shared};

  REQUIRE_FALSE(descriptor.expired(&weak));
}

TEST_CASE(
    "Given a TWeakRefType descriptor and its shared_ptr owner exiting scope, when expired is called on the weak_ptr, "
    "then it returns true",
    "[rtti][weak_ref_type][operations][expired]") {
  const TClassType<WeakTarget> inner(IName("wexpired_dead_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  std::weak_ptr<WeakTarget> weak;
  {
    const auto shared = std::make_shared<WeakTarget>();
    weak = shared;
    REQUIRE_FALSE(descriptor.expired(&weak));
  }
  REQUIRE(descriptor.expired(&weak));
}

TEST_CASE("Given a TWeakRefType descriptor, when expired is called with a null instance, then it returns true",
          "[rtti][weak_ref_type][operations][expired][negative]") {
  const TClassType<WeakTarget> inner(IName("wexpired_null_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  REQUIRE(descriptor.expired(nullptr));
}

TEST_CASE(
    "Given a TWeakRefType descriptor and a default-constructed (never-assigned) weak_ptr, when expired is called, "
    "then it returns true",
    "[rtti][weak_ref_type][operations][expired]") {
  const TClassType<WeakTarget> inner(IName("wexpired_default_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const std::weak_ptr<WeakTarget> unassigned;
  REQUIRE(descriptor.expired(&unassigned));
}

// =============================================================================
// lock
// =============================================================================

TEST_CASE(
    "Given a TWeakRefType descriptor and a non-expired weak_ptr, when lock is called, then the output shared_ptr "
    "manages the same object",
    "[rtti][weak_ref_type][operations][lock]") {
  const TClassType<WeakTarget> inner(IName("wlock_success_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto original = std::make_shared<WeakTarget>(WeakTarget{42});
  const std::weak_ptr weak{original};

  std::shared_ptr<WeakTarget> locked;
  descriptor.lock(&weak, &locked);

  REQUIRE(locked.get() == original.get());
}

TEST_CASE(
    "Given a TWeakRefType descriptor and an expired weak_ptr, when lock is called, then the output shared_ptr is "
    "empty",
    "[rtti][weak_ref_type][operations][lock]") {
  const TClassType<WeakTarget> inner(IName("wlock_expired_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const std::weak_ptr<WeakTarget> expired;
  std::shared_ptr<WeakTarget> locked;

  descriptor.lock(&expired, &locked);

  REQUIRE(locked == nullptr);
}

TEST_CASE(
    "Given a TWeakRefType descriptor, when lock is called with a null weak_ptr, then the output shared_ptr remains "
    "unchanged",
    "[rtti][weak_ref_type][operations][lock][negative]") {
  const TClassType<WeakTarget> inner(IName("wlock_null_weak_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  auto original = std::make_shared<WeakTarget>(WeakTarget{7});
  std::shared_ptr<WeakTarget> locked = original;

  descriptor.lock(nullptr, &locked);

  REQUIRE(locked == original);
}

TEST_CASE(
    "Given a TWeakRefType descriptor, when lock is called with a null output shared_ptr, then it completes without "
    "error",
    "[rtti][weak_ref_type][operations][lock][negative]") {
  const TClassType<WeakTarget> inner(IName("wlock_null_output_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto shared = std::make_shared<WeakTarget>();
  const std::weak_ptr weak{shared};

  REQUIRE_NOTHROW(descriptor.lock(&weak, nullptr));
}

TEST_CASE(
    "Given a TWeakRefType descriptor and a non-expired weak_ptr, when lock is called, then the output shared_ptr "
    "increments the use count",
    "[rtti][weak_ref_type][operations][lock]") {
  const TClassType<WeakTarget> inner(IName("wlock_usecount_inner"));
  const TWeakRefType<WeakTarget> descriptor(&inner);

  const auto original = std::make_shared<WeakTarget>(WeakTarget{99});
  REQUIRE(original.use_count() == 1);

  const std::weak_ptr weak{original};
  std::shared_ptr<WeakTarget> locked;
  descriptor.lock(&weak, &locked);

  REQUIRE(locked.use_count() == 2);
  REQUIRE(original.use_count() == 2);
}
