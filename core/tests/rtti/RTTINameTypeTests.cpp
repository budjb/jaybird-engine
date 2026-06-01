#include <catch2/catch_test_macros.hpp>

#include "NamePool.hpp"
#include "rtti/RTTINameType.hpp"
#include "rtti/RTTITypeKind.hpp"

TEST_CASE(
    "Given an RTTINameType descriptor, when inspected through RTTIType, then metadata matches the Name runtime type",
    "[rtti][iname_type][metadata]") {
  const core::rtti::RTTINameType descriptor;
  const core::rtti::RTTIType& asType = descriptor;

  REQUIRE(asType.kind() == core::rtti::RTTITypeKind::NAME);
  REQUIRE(asType.size() == sizeof(core::Name));
  REQUIRE(asType.alignment() == alignof(core::Name));
  REQUIRE(asType.name() == core::Name("Name"));
}

TEST_CASE(
    "Given an RTTINameType descriptor, when assign and equals are called, then Name values follow value semantics",
    "[rtti][iname_type][operations]") {
  const core::rtti::RTTINameType descriptor;
  const core::rtti::RTTIType& asType = descriptor;

  const core::Name source = core::NamePool::get().addName("iname_type_source");
  core::Name destination = core::NamePool::get().addName("iname_type_destination");

  asType.assign(&destination, &source);
  REQUIRE(destination == source);
  REQUIRE(asType.equals(&destination, &source));

  const core::Name different = core::NamePool::get().addName("iname_type_different");
  REQUIRE_FALSE(asType.equals(&destination, &different));
}
