#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <vector>

#include "rtti/RTTIName.hpp"
#include "rtti/RTTISystem.hpp"
#include "rtti/RTTITypeKind.hpp"

#define VALIDATE_RTTI_TYPE(registry, typeName, expectedKind)                                                        \
  REQUIRE(registry.hasType(typeName));                                                                              \
  REQUIRE(registry.getType(typeName)->kind() == expectedKind);                                                      \
  if (expectedKind == core::rtti::RTTITypeKind::CLASS) {                                                            \
    REQUIRE(registry.hasType(core::rtti::GetPrefixedRTTIName<core::rtti::RTTITypeKind::REF>(typeName)));            \
    REQUIRE(registry.hasType(core::rtti::GetPrefixedRTTIName<core::rtti::RTTITypeKind::WEAK_REF>(typeName)));       \
    REQUIRE(registry.hasType(core::rtti::GetPrefixedRTTIName<core::rtti::RTTITypeKind::ARRAY>(                      \
        core::rtti::GetPrefixedRTTIName<core::rtti::RTTITypeKind::REF>(typeName))));                                \
  } else {                                                                                                          \
    REQUIRE(registry.hasType(core::rtti::GetPrefixedRTTIName<core::rtti::RTTITypeKind::ARRAY>(typeName)));          \
    REQUIRE(registry.getType(core::rtti::GetPrefixedRTTIName<core::rtti::RTTITypeKind::ARRAY>(typeName))->kind() == \
            core::rtti::RTTITypeKind::ARRAY);                                                                       \
  }

TEST_CASE(
    "Given RTTISystem initialization callbacks, when initialize is called, then declare callbacks run before "
    "define callbacks and built-in types are registered",
    "[rtti][type_system][initialize]") {
  core::rtti::RTTISystem typeSystem;

  std::vector<int> callbackOrder;
  std::atomic declareCalls{0};
  std::atomic defineCalls{0};

  typeSystem.addDeclareCallback([&callbackOrder, &declareCalls]() {
    callbackOrder.push_back(1);
    declareCalls.fetch_add(1, std::memory_order_relaxed);
  });

  typeSystem.addDefineCallback([&callbackOrder, &defineCalls]() {
    callbackOrder.push_back(2);
    defineCalls.fetch_add(1, std::memory_order_relaxed);
  });

  const bool initializedNow = typeSystem.initialize();

  REQUIRE(initializedNow);
  REQUIRE(declareCalls.load(std::memory_order_relaxed) == 1);
  REQUIRE(defineCalls.load(std::memory_order_relaxed) == 1);
  REQUIRE(callbackOrder == std::vector<int>{1, 2});

  core::rtti::RTTIRegistry& registry = typeSystem.registry();

  VALIDATE_RTTI_TYPE(registry, "int8", core::rtti::RTTITypeKind::FUNDAMENTAL);
  VALIDATE_RTTI_TYPE(registry, "int32", core::rtti::RTTITypeKind::FUNDAMENTAL);
  VALIDATE_RTTI_TYPE(registry, "int64", core::rtti::RTTITypeKind::FUNDAMENTAL);
  VALIDATE_RTTI_TYPE(registry, "uint8", core::rtti::RTTITypeKind::FUNDAMENTAL);
  VALIDATE_RTTI_TYPE(registry, "uint32", core::rtti::RTTITypeKind::FUNDAMENTAL);
  VALIDATE_RTTI_TYPE(registry, "uint64", core::rtti::RTTITypeKind::FUNDAMENTAL);
  VALIDATE_RTTI_TYPE(registry, "float", core::rtti::RTTITypeKind::FUNDAMENTAL);
  VALIDATE_RTTI_TYPE(registry, "double", core::rtti::RTTITypeKind::FUNDAMENTAL);
  VALIDATE_RTTI_TYPE(registry, "bool", core::rtti::RTTITypeKind::FUNDAMENTAL);

  VALIDATE_RTTI_TYPE(registry, "Name", core::rtti::RTTITypeKind::NAME);
  VALIDATE_RTTI_TYPE(registry, "string", core::rtti::RTTITypeKind::STRING);

  VALIDATE_RTTI_TYPE(registry, "Quaternion", core::rtti::RTTITypeKind::SIMPLE);
  VALIDATE_RTTI_TYPE(registry, "EulerAngles", core::rtti::RTTITypeKind::SIMPLE);
  VALIDATE_RTTI_TYPE(registry, "Vector2", core::rtti::RTTITypeKind::SIMPLE);
  VALIDATE_RTTI_TYPE(registry, "Vector3", core::rtti::RTTITypeKind::SIMPLE);
  VALIDATE_RTTI_TYPE(registry, "Color", core::rtti::RTTITypeKind::SIMPLE);

  REQUIRE_FALSE(typeSystem.initialize());
}

TEST_CASE(
    "Given RTTISystem::addCallbacks, when initialize is called, then both callbacks run in the correct phase order",
    "[rtti][type_system][add_callbacks]") {
  core::rtti::RTTISystem typeSystem;

  std::vector<int> callbackOrder;
  std::atomic callCount{0};

  typeSystem.addCallbacks(
      [&callbackOrder, &callCount]() {
        callbackOrder.push_back(1);  // declare phase
        callCount.fetch_add(1, std::memory_order_relaxed);
      },
      [&callbackOrder, &callCount]() {
        callbackOrder.push_back(2);  // define phase
        callCount.fetch_add(1, std::memory_order_relaxed);
      });

  typeSystem.initialize();

  REQUIRE(callCount.load(std::memory_order_relaxed) == 2);
  REQUIRE(callbackOrder == std::vector<int>{1, 2});
}
