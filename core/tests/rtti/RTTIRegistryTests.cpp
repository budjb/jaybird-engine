#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <thread>

#include "Vector.hpp"
#include "rtti/RTTIClassType.hpp"
#include "rtti/RTTIGlobalFunction.hpp"
#include "rtti/RTTIRegistry.hpp"
#include "rtti/RTTISystem.hpp"
#include "rtti/RTTITypedFunction.hpp"
#include "types/CString.hpp"

using core::Vector;

// =============================================================================
// Test types — each struct has a unique TypeName so the auto-array name is
// deterministic and does not collide with other test files.
// =============================================================================

/** @brief Test struct A — used for basic class-type registration. */
struct RegTargetA {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetA&) const = default;
};

/** @brief Test struct B — used for auto-array companion verification. */
struct RegTargetB {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetB&) const = default;
};

/** @brief Test struct C — used for duplicate-rejection tests. */
struct RegTargetC {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetC&) const = default;
};

/** @brief Test struct D — used for Name hash equivalence lookup. */
struct RegTargetD {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetD&) const = default;
};

/** @brief Test struct E — used for getClass positive lookup on non-trivial class types. */
struct RegTargetE {
  // ReSharper disable once CppDeclaratorNeverUsed
  std::string v{};
  bool operator==(const RegTargetE&) const = default;
};

/** @brief Test struct F — used for unrelated-name negative lookup. */
struct RegTargetF {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetF&) const = default;
};

/** @brief Test struct G — used for concurrent-reader consistency. */
struct RegTargetG {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetG&) const = default;
};

/** @brief Test struct H — used for concurrent duplicate-writer test. */
struct RegTargetH {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetH&) const = default;
};

/** @brief Test struct I — used for array-kind does-not-re-register test. */
struct RegTargetI {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetI&) const = default;
};

/**
 * @brief This type is used exclusively to verify that the first @c registerType call returns @code true@endcode.
 *
 * No other test case may register this type. This constraint ensures the registration in that test case is
 * always the first, so the return value is guaranteed to be @code true@endcode.
 */
struct RegTargetA_Solo {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetA_Solo&) const = default;
};

/**
 * @brief This type is used exclusively to verify that companion-array auto-registration works correctly.
 *
 * No other test case may register this type. This constraint ensures the registration in that test case is
 * always the first, keeping the companion-array assertion order-independent.
 */
struct RegTargetB_Solo {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetB_Solo&) const = default;
};

/** @brief Test struct J is used exclusively for unregister-type tests. */
struct RegTargetJ_Solo {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetJ_Solo&) const = default;
};

/** @brief Test struct K is used exclusively to validate class companion unregister behavior. */
struct RegTargetK_Solo {
  // ReSharper disable once CppDeclaratorNeverUsed
  std::string v{};
  bool operator==(const RegTargetK_Solo&) const = default;
};

/** @brief Test struct L is used exclusively to validate unregistering only an auto-array companion. */
struct RegTargetL_Solo {
  // ReSharper disable once CppDeclaratorNeverUsed
  int v{0};
  bool operator==(const RegTargetL_Solo&) const = default;
};

REGISTER_TYPE_NAME(RegTargetA, "reg_target_a");
REGISTER_TYPE_NAME(RegTargetB, "reg_target_b");
REGISTER_TYPE_NAME(RegTargetC, "reg_target_c");
REGISTER_TYPE_NAME(RegTargetD, "reg_target_d");
REGISTER_TYPE_NAME(RegTargetE, "reg_target_e");
REGISTER_TYPE_NAME(RegTargetF, "reg_target_f");
REGISTER_TYPE_NAME(RegTargetG, "reg_target_g");
REGISTER_TYPE_NAME(RegTargetH, "reg_target_h");
REGISTER_TYPE_NAME(RegTargetI, "reg_target_i");
REGISTER_TYPE_NAME(RegTargetA_Solo, "reg_target_a_solo");
REGISTER_TYPE_NAME(RegTargetB_Solo, "reg_target_b_solo");
REGISTER_TYPE_NAME(RegTargetJ_Solo, "reg_target_j_solo");
REGISTER_TYPE_NAME(RegTargetK_Solo, "reg_target_k_solo");
REGISTER_TYPE_NAME(RegTargetL_Solo, "reg_target_l_solo");

namespace {
using core::Name;
using core::rtti::RTTIArrayType;
using core::rtti::RTTIClassType;
using core::rtti::RTTIRegistry;
using core::rtti::RTTISystem;
using core::rtti::RTTIType;
using core::rtti::RTTITypeKind;
using core::rtti::TypedRTTIArrayType;
using core::rtti::TypedRTTIClassType;

void registryPingFunction() {}

std::unique_ptr<core::rtti::RTTIGlobalFunction> makeFunctionDescriptor(const char* name) {
  using FunctionDescriptor = core::rtti::TypedRTTIGlobalFunction<decltype(&registryPingFunction)>;
  return std::make_unique<FunctionDescriptor>(name, &registryPingFunction);
}

template <typename T>
constexpr RTTITypeKind expectedClassRegistrationKind() {
  return std::is_trivially_copyable_v<T> ? RTTITypeKind::SIMPLE : RTTITypeKind::CLASS;
}
}  // namespace

// =============================================================================
// Singleton access
// =============================================================================

TEST_CASE(
    "Given RTTISystem::get, when registry() is called multiple times, then the same RTTIRegistry instance is "
    "returned",
    "[rtti][type_registry]") {
  RTTIRegistry& first = RTTISystem::get().registry();
  RTTIRegistry& second = RTTISystem::get().registry();

  REQUIRE(&first == &second);
}

// =============================================================================
// Unregistered name — lookup APIs
// =============================================================================

TEST_CASE(
    "Given an unregistered type name, when queried in RTTIRegistry, then hasType returns false and retrieval APIs "
    "return nullptr",
    "[rtti][type_registry]") {
  const RTTIRegistry& registry = RTTISystem::get().registry();
  const Name missing("registry_test_missing_xyz_1234");

  REQUIRE_FALSE(registry.hasType(missing));
  REQUIRE(registry.getType(missing) == nullptr);
  REQUIRE(registry.getClass(missing) == nullptr);
}

// =============================================================================
// Basic registration — class type
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIClassType descriptor, when registered in RTTIRegistry, then registerType returns a non-null "
    "pointer and the type is retrievable",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  auto* registered = registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetA_Solo>>());

  REQUIRE(registered != nullptr);
  REQUIRE(registry.hasType("reg_target_a_solo"));
  REQUIRE(registry.getType("reg_target_a_solo") == registered);
}

TEST_CASE(
    "Given a registered TypedRTTIClassType, when getType is called, then the returned descriptor kind matches trivial "
    "copyability",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetA>>());

  REQUIRE(registry.getType("reg_target_a")->kind() == expectedClassRegistrationKind<RegTargetA>());
}

TEST_CASE(
    "Given a registered non-trivial TypedRTTIClassType, when getClass is called, then it returns the same descriptor "
    "as getType",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetE>>());

  RTTIType* asType = registry.getType("reg_target_e");
  RTTIClassType* asClass = registry.getClass("reg_target_e");

  REQUIRE(asClass != nullptr);
  REQUIRE(asType->kind() == RTTITypeKind::CLASS);
  REQUIRE(static_cast<RTTIType*>(asClass) == asType);
}

TEST_CASE(
    "Given a registered non-trivial TypedRTTIClassType, when companion container descriptors are queried, then ref, "
    "pointer, weak ref, and array-of-ref descriptors are registered",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetE>>());

  RTTIType* baseType = registry.getType("reg_target_e");
  RTTIType* refType = registry.getType("ref:reg_target_e");
  RTTIType* pointerType = registry.getType("ptr:reg_target_e");
  RTTIType* weakRefType = registry.getType("wref:reg_target_e");
  RTTIType* refArrayType = registry.getType("array:ref:reg_target_e");

  REQUIRE(baseType != nullptr);
  REQUIRE(baseType->kind() == RTTITypeKind::CLASS);

  REQUIRE(refType != nullptr);
  REQUIRE(refType->kind() == RTTITypeKind::REF);

  REQUIRE(pointerType != nullptr);
  REQUIRE(pointerType->kind() == RTTITypeKind::POINTER);

  REQUIRE(weakRefType != nullptr);
  REQUIRE(weakRefType->kind() == RTTITypeKind::WEAK_REF);

  REQUIRE(refArrayType != nullptr);
  REQUIRE(refArrayType->kind() == RTTITypeKind::ARRAY);

  auto* refArrayDescriptor = reinterpret_cast<RTTIArrayType*>(refArrayType);
  REQUIRE(refArrayDescriptor != nullptr);
  REQUIRE(refArrayDescriptor->inner() == refType);
}

// =============================================================================
// Auto-array companion registration
// =============================================================================

TEST_CASE(
    "Given a TypedRTTIClassType registered in RTTIRegistry, when companion pointer and array names are queried, then "
    "both are registered automatically",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  REQUIRE(registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetB_Solo>>()) != nullptr);

  const Name pointerName("ptr:reg_target_b_solo");
  const Name arrayName("array:reg_target_b_solo");
  REQUIRE(registry.hasType(pointerName));
  REQUIRE(registry.getType(pointerName) != nullptr);
  REQUIRE(registry.hasType(arrayName));
  REQUIRE(registry.getType(arrayName) != nullptr);
}

TEST_CASE(
    "Given a TypedRTTIClassType registered in RTTIRegistry, when the companion array type is retrieved, then it has "
    "kind ARRAY and its inner descriptor matches the original type",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetB>>());
  RTTIType* baseType = registry.getType("reg_target_b");
  RTTIType* arrayType = registry.getType("array:reg_target_b");

  REQUIRE(arrayType->kind() == RTTITypeKind::ARRAY);
  // RTTIType::asArray() called on the base type resolves its companion array via registry lookup
  RTTIArrayType* companionArray = baseType->asArray();
  REQUIRE(companionArray != nullptr);
  REQUIRE(static_cast<RTTIType*>(companionArray) == arrayType);
  REQUIRE(companionArray->inner() == baseType);
}

TEST_CASE(
    "Given a TypedRTTIClassType registered in RTTIRegistry, when getClass is called for the companion array name, then "
    "it returns nullptr since an array type is not a class type",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetB>>());

  REQUIRE(registry.getClass(Name("array:reg_target_b")) == nullptr);
}

TEST_CASE(
    "Given an TypedRTTIArrayType registered directly in RTTIRegistry, when the registry is queried, then no "
    "second-order array is automatically created",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetI>>());
  RTTIType* inner = registry.getType("reg_target_i");
  REQUIRE(inner != nullptr);

  // The auto-array is already registered; re-registering it returns nullptr (duplicate) but does not crash.
  auto* reregistered = registry.registerType(
      std::make_unique<TypedRTTIArrayType<RegTargetI>>(reinterpret_cast<TypedRTTIClassType<RegTargetI>*>(inner)));
  REQUIRE(reregistered == nullptr);

  // No second-order array should ever exist.
  REQUIRE_FALSE(registry.hasType("array:array:reg_target_i"));
}

// =============================================================================
// Duplicate registration
// =============================================================================

TEST_CASE("Given a type already registered, when the same name is registered again, then registerType returns nullptr",
          "[rtti][type_registry][negative]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetC>>());

  REQUIRE(registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetC>>()) == nullptr);
}

TEST_CASE(
    "Given a type already registered, when a second descriptor with the same name is rejected, then the original "
    "descriptor remains retrievable",
    "[rtti][type_registry][negative]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetC>>());
  RTTIType* original = registry.getType("reg_target_c");
  REQUIRE(original != nullptr);

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetC>>());

  REQUIRE(registry.getType("reg_target_c") == original);
}

// =============================================================================
// Name hash equivalence
// =============================================================================

TEST_CASE(
    "Given a registered type, when looked up by a different Name instance with the same hash, then the descriptor "
    "is returned",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetD>>());

  const Name byHash(Name("reg_target_d").hash());

  REQUIRE(registry.hasType(byHash));
  REQUIRE(registry.getType(byHash) == registry.getType("reg_target_d"));
}

// =============================================================================
// Unrelated-name negative lookup
// =============================================================================

TEST_CASE(
    "Given a registered type, when an unrelated name is looked up, then hasType returns false and getType returns "
    "nullptr",
    "[rtti][type_registry][negative]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetF>>());

  const Name unrelated("totally_unrelated_name_xyz_9999");
  REQUIRE_FALSE(registry.hasType(unrelated));
  REQUIRE(registry.getType(unrelated) == nullptr);
  REQUIRE(registry.getClass(unrelated) == nullptr);
}

// =============================================================================
// Thread safety — concurrent duplicate writers
// =============================================================================

TEST_CASE(
    "Given many concurrent threads all trying to register the same type name, when all threads complete, then "
    "exactly one registration succeeds and the auto-array companion is also present",
    "[rtti][type_registry][thread_safety]") {
  RTTIRegistry& registry = RTTISystem::get().registry();
  constexpr int writerCount = 16;

  Vector successes(writerCount, 0);
  Vector<std::thread> threads;
  threads.reserve(writerCount);

  for (int i = 0; i < writerCount; ++i) {
    threads.emplaceBack([&registry, &successes, i]() {
      auto* registered = registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetH>>());
      successes[static_cast<std::size_t>(i)] = registered != nullptr ? 1 : 0;
    });
  }

  for (auto& t : threads) {
    t.join();
  }

  int successCount = 0;
  for (const int s : successes) {
    successCount += s;
  }

  REQUIRE(successCount == 1);
  REQUIRE(registry.hasType("reg_target_h"));
  REQUIRE(registry.hasType("ptr:reg_target_h"));
  REQUIRE(registry.hasType("array:reg_target_h"));
}

// =============================================================================
// Thread safety — concurrent readers
// =============================================================================

TEST_CASE(
    "Given a registered type, when many concurrent readers repeatedly query RTTIRegistry, then all reads remain "
    "consistent and return the expected descriptor",
    "[rtti][type_registry][thread_safety]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetG>>());
  RTTIType* expected = registry.getType("reg_target_g");
  RTTIClassType* expectedClass = registry.getClass(Name("reg_target_g"));
  REQUIRE(expected != nullptr);

  constexpr int readerCount = 8;
  constexpr int iterationsPerReader = 5000;
  std::atomic inconsistentRead{false};

  Vector<std::thread> readers;
  readers.reserve(readerCount);

  for (int i = 0; i < readerCount; ++i) {
    readers.emplaceBack([&registry, expected, expectedClass, &inconsistentRead]() {
      for (int j = 0; j < iterationsPerReader; ++j) {
        if (!registry.hasType("reg_target_g")) {
          inconsistentRead.store(true, std::memory_order_relaxed);
          break;
        }
        if (registry.getType("reg_target_g") != expected) {
          inconsistentRead.store(true, std::memory_order_relaxed);
          break;
        }
        if (registry.getClass(Name("reg_target_g")) != expectedClass) {
          inconsistentRead.store(true, std::memory_order_relaxed);
          break;
        }
      }
    });
  }

  for (auto& r : readers) {
    r.join();
  }

  REQUIRE_FALSE(inconsistentRead.load(std::memory_order_relaxed));
}

// =============================================================================
// Global function registration and retrieval
// =============================================================================

TEST_CASE(
    "Given an unregistered global function name, when queried in RTTIRegistry, then hasFunction returns false and "
    "getFunction returns nullptr",
    "[rtti][type_registry]") {
  const RTTIRegistry& registry = RTTISystem::get().registry();
  const Name missing("registry_test_missing_function_xyz_1234");

  REQUIRE_FALSE(registry.hasFunction(missing));
  REQUIRE(registry.getFunction(missing) == nullptr);
}

TEST_CASE(
    "Given a reflected global function descriptor, when registered in RTTIRegistry, then it is retrievable by name",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  auto* registered = registry.registerFunction(makeFunctionDescriptor("registry_ping_function_a"));

  REQUIRE(registered != nullptr);
  REQUIRE(registry.hasFunction("registry_ping_function_a"));
  REQUIRE(registry.getFunction("registry_ping_function_a") == registered);
}

TEST_CASE(
    "Given a global function already registered, when a second descriptor with the same name is registered, then "
    "registerFunction returns nullptr and the original remains retrievable",
    "[rtti][type_registry][negative]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  auto* first = registry.registerFunction(makeFunctionDescriptor("registry_ping_function_b"));
  auto* second = registry.registerFunction(makeFunctionDescriptor("registry_ping_function_b"));

  REQUIRE(first != nullptr);
  REQUIRE(second == nullptr);
  REQUIRE(registry.getFunction("registry_ping_function_b") == first);
}

#ifdef TESTING_ENABLED
// =============================================================================
// Unregister APIs (testing-only)
// =============================================================================

TEST_CASE(
    "Given a registered type, when unregisterType is called in tests, then the type and its pointer and array "
    "companions are removed",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  REQUIRE(registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetJ_Solo>>()) != nullptr);
  REQUIRE(registry.hasType("reg_target_j_solo"));
  REQUIRE(registry.hasType("ptr:reg_target_j_solo"));
  REQUIRE(registry.hasType("array:reg_target_j_solo"));

  REQUIRE(registry.unregisterType("reg_target_j_solo"));
  REQUIRE_FALSE(registry.hasType("reg_target_j_solo"));
  REQUIRE_FALSE(registry.hasType("ptr:reg_target_j_solo"));
  REQUIRE_FALSE(registry.hasType("array:reg_target_j_solo"));
}

TEST_CASE(
    "Given a registered non-trivial class type, when unregisterType is called in tests, then class pointer, ref, "
    "weak-ref, and array-of-ref companions are removed",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  REQUIRE(registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetK_Solo>>()) != nullptr);
  REQUIRE(registry.hasType("reg_target_k_solo"));
  REQUIRE(registry.hasType("ptr:reg_target_k_solo"));
  REQUIRE(registry.hasType("ref:reg_target_k_solo"));
  REQUIRE(registry.hasType("wref:reg_target_k_solo"));
  REQUIRE(registry.hasType("array:ref:reg_target_k_solo"));

  REQUIRE(registry.unregisterType("reg_target_k_solo"));
  REQUIRE_FALSE(registry.hasType("reg_target_k_solo"));
  REQUIRE_FALSE(registry.hasType("ptr:reg_target_k_solo"));
  REQUIRE_FALSE(registry.hasType("ref:reg_target_k_solo"));
  REQUIRE_FALSE(registry.hasType("wref:reg_target_k_solo"));
  REQUIRE_FALSE(registry.hasType("array:ref:reg_target_k_solo"));
}

TEST_CASE(
    "Given a registered type with pointer and auto-array companions, when unregisterType is called for only the array "
    "name in tests, then the base and pointer types remain registered",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  REQUIRE(registry.registerType(std::make_unique<TypedRTTIClassType<RegTargetL_Solo>>()) != nullptr);
  REQUIRE(registry.hasType("reg_target_l_solo"));
  REQUIRE(registry.hasType("ptr:reg_target_l_solo"));
  REQUIRE(registry.hasType("array:reg_target_l_solo"));

  REQUIRE(registry.unregisterType("array:reg_target_l_solo"));
  REQUIRE(registry.hasType("reg_target_l_solo"));
  REQUIRE(registry.hasType("ptr:reg_target_l_solo"));
  REQUIRE_FALSE(registry.hasType("array:reg_target_l_solo"));
}

TEST_CASE("Given a type name that is not registered, when unregisterType is called in tests, then it returns false",
          "[rtti][type_registry][negative]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  REQUIRE_FALSE(registry.unregisterType("registry_test_missing_unreg_type_xyz_1234"));
}

TEST_CASE(
    "Given a registered global function, when unregisterFunction is called in tests, then the function is removed",
    "[rtti][type_registry]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  REQUIRE(registry.registerFunction(makeFunctionDescriptor("registry_ping_function_c")) != nullptr);
  REQUIRE(registry.hasFunction("registry_ping_function_c"));

  REQUIRE(registry.unregisterFunction("registry_ping_function_c"));
  REQUIRE_FALSE(registry.hasFunction("registry_ping_function_c"));
  REQUIRE(registry.getFunction("registry_ping_function_c") == nullptr);
}

TEST_CASE(
    "Given a global function that was already removed, when unregisterFunction is called again in tests, then it "
    "returns false",
    "[rtti][type_registry][negative]") {
  RTTIRegistry& registry = RTTISystem::get().registry();

  REQUIRE(registry.registerFunction(makeFunctionDescriptor("registry_ping_function_d")) != nullptr);
  REQUIRE(registry.unregisterFunction("registry_ping_function_d"));
  REQUIRE_FALSE(registry.unregisterFunction("registry_ping_function_d"));
}
#endif
