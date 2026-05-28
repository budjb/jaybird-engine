#include <catch2/catch_test_macros.hpp>

#include "INamePool.hpp"
#include "rtti/INameType.hpp"
#include "rtti/TypeKind.hpp"

TEST_CASE("Given an INameType descriptor, when inspected through IType, then metadata matches the IName runtime type",
          "[rtti][iname_type][metadata]") {
  const core::rtti::INameType descriptor;
  const core::rtti::IType& asType = descriptor;

  REQUIRE(asType.kind() == core::rtti::TypeKind::NAME);
  REQUIRE(asType.size() == sizeof(core::IName));
  REQUIRE(asType.alignment() == alignof(core::IName));
  REQUIRE(asType.name() == core::IName("IName"));
}

TEST_CASE("Given an INameType descriptor, when assign and equals are called, then IName values follow value semantics",
          "[rtti][iname_type][operations]") {
  core::rtti::INameType descriptor;
  core::rtti::IType& asType = descriptor;

  const core::IName source = core::INamePool::get().addName("iname_type_source");
  core::IName destination = core::INamePool::get().addName("iname_type_destination");

  asType.assign(&destination, &source);
  REQUIRE(destination == source);
  REQUIRE(asType.equals(&destination, &source));

  const core::IName different = core::INamePool::get().addName("iname_type_different");
  REQUIRE_FALSE(asType.equals(&destination, &different));
}
