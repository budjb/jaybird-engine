#include <catch2/catch_test_macros.hpp>

#include "INamePool.hpp"
#include "rtti/RTTIINameType.hpp"
#include "rtti/RTTITypeKind.hpp"

TEST_CASE(
    "Given an RTTIINameType descriptor, when inspected through RTTIType, then metadata matches the IName runtime type",
    "[rtti][iname_type][metadata]") {
  const core::rtti::RTTIINameType descriptor;
  const core::rtti::RTTIType& asType = descriptor;

  REQUIRE(asType.kind() == core::rtti::RTTITypeKind::NAME);
  REQUIRE(asType.size() == sizeof(core::IName));
  REQUIRE(asType.alignment() == alignof(core::IName));
  REQUIRE(asType.name() == core::IName("IName"));
}

TEST_CASE(
    "Given an RTTIINameType descriptor, when assign and equals are called, then IName values follow value semantics",
    "[rtti][iname_type][operations]") {
  const core::rtti::RTTIINameType descriptor;
  const core::rtti::RTTIType& asType = descriptor;

  const core::IName source = core::INamePool::get().addName("iname_type_source");
  core::IName destination = core::INamePool::get().addName("iname_type_destination");

  asType.assign(&destination, &source);
  REQUIRE(destination == source);
  REQUIRE(asType.equals(&destination, &source));

  const core::IName different = core::INamePool::get().addName("iname_type_different");
  REQUIRE_FALSE(asType.equals(&destination, &different));
}
