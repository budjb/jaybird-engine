#include <catch2/catch_test_macros.hpp>

#include "IName.hpp"
#include "rtti/TypeRegistry.hpp"
#include "rtti/registration/TypeRegistrar.hpp"

TEST_CASE(
    "Given fundamental types from Fundamentals.hpp, when the DLL is loaded, then they are auto-registered without "
    "explicit registerTypes call",
    "[rtti][registration][auto_registration][fundamentals]") {
  core::rtti::TypeRegistrar::registerTypes();
  auto* registry = core::rtti::TypeRegistry::get();

  REQUIRE(registry->hasType(core::IName("int32")));
  REQUIRE(registry->hasType(core::IName("uint32")));
  REQUIRE(registry->hasType(core::IName("int64")));
  REQUIRE(registry->hasType(core::IName("uint64")));
  REQUIRE(registry->hasType(core::IName("float")));
  REQUIRE(registry->hasType(core::IName("double")));
  REQUIRE(registry->hasType(core::IName("bool")));

  REQUIRE(registry->getType(core::IName("int32")) != nullptr);
  REQUIRE(registry->getType(core::IName("float")) != nullptr);
  REQUIRE(registry->getType(core::IName("bool")) != nullptr);
}
