#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <vector>

#include "rtti/RTTISystem.hpp"
#include "rtti/RTTITypeKind.hpp"

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

  REQUIRE(registry.hasType("int8"));
  REQUIRE(registry.hasType("int32"));
  REQUIRE(registry.hasType("int64"));
  REQUIRE(registry.hasType("uint8"));
  REQUIRE(registry.hasType("uint32"));
  REQUIRE(registry.hasType("uint64"));
  REQUIRE(registry.hasType("float"));
  REQUIRE(registry.hasType("double"));
  REQUIRE(registry.hasType("bool"));

  REQUIRE(registry.hasType("Name"));
  REQUIRE(registry.getType("Name")->kind() == core::rtti::RTTITypeKind::NAME);

  REQUIRE(registry.hasType("Quaternion"));
  REQUIRE(registry.hasType("EulerAngles"));
  REQUIRE(registry.hasType("Color"));

  REQUIRE(registry.hasType("array:Quaternion"));
  REQUIRE(registry.hasType("array:EulerAngles"));
  REQUIRE(registry.hasType("array:Color"));

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
