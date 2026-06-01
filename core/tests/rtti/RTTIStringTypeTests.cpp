#include <catch2/catch_test_macros.hpp>
#include <string>

#include "rtti/RTTIName.hpp"
#include "rtti/RTTIStringType.hpp"
#include "rtti/RTTITypeKind.hpp"

namespace {
using core::Name;
using core::rtti::RTTIStringType;
using core::rtti::RTTIType;
using core::rtti::RTTITypeKind;
}  // namespace

TEST_CASE(
    "Given an RTTIStringType descriptor, when inspected through RTTIType, then metadata matches the std::string type",
    "[rtti][string_type][metadata]") {
  const RTTIStringType descriptor;
  const RTTIType& asType = descriptor;

  REQUIRE(asType.kind() == RTTITypeKind::STRING);
  REQUIRE(asType.size() == sizeof(std::string));
  REQUIRE(asType.alignment() == alignof(std::string));
  REQUIRE(asType.name() == core::rtti::GetRTTIName<std::string>());
}

TEST_CASE("Given an RTTIStringType descriptor, when the name is queried, then it equals the canonical string type name",
          "[rtti][string_type][metadata]") {
  const RTTIStringType descriptor;

  REQUIRE(std::string(descriptor.name()) == "string");
}

TEST_CASE(
    "Given an RTTIStringType descriptor and two strings, when assign is called through RTTIType, then the "
    "destination receives the source value",
    "[rtti][string_type][operations][assign]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  const std::string source = "hello";
  std::string destination = "world";

  type.assign(&destination, &source);

  REQUIRE(destination == "hello");
}

TEST_CASE(
    "Given an RTTIStringType descriptor and a null destination, when assign is called through RTTIType, then the "
    "operation is a no-op and existing values remain unchanged",
    "[rtti][string_type][operations][assign][negative]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  const std::string source = "hello";

  type.assign(nullptr, &source);
  SUCCEED();
}

TEST_CASE(
    "Given an RTTIStringType descriptor and two equal strings, when equals is called through RTTIType, then it "
    "returns true",
    "[rtti][string_type][operations][equals]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  const std::string lhs = "jaybird";
  const std::string rhs = "jaybird";

  REQUIRE(type.equals(&lhs, &rhs));
}

TEST_CASE(
    "Given an RTTIStringType descriptor and two different strings, when equals is called through RTTIType, then it "
    "returns false",
    "[rtti][string_type][operations][equals][negative]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  const std::string lhs = "foo";
  const std::string rhs = "bar";

  REQUIRE_FALSE(type.equals(&lhs, &rhs));
}

TEST_CASE(
    "Given an RTTIStringType descriptor, when equals receives null combinations, then null-null is true and "
    "mixed-null is false",
    "[rtti][string_type][operations][equals][null]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  const std::string value = "test";

  REQUIRE(type.equals(nullptr, nullptr));
  REQUIRE_FALSE(type.equals(&value, nullptr));
  REQUIRE_FALSE(type.equals(nullptr, &value));
}

TEST_CASE(
    "Given an RTTIStringType descriptor, when allocate is called through RTTIType, then returned memory is "
    "non-null and aligned for std::string",
    "[rtti][string_type][operations][allocate]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  void* allocated = type.allocate();

  REQUIRE(allocated != nullptr);
  REQUIRE(reinterpret_cast<std::uintptr_t>(allocated) % alignof(std::string) == 0);

  type.deallocate(allocated);
}

TEST_CASE(
    "Given an RTTIStringType descriptor and allocated storage, when construct is called through RTTIType, then "
    "the string is default-initialized to empty",
    "[rtti][string_type][operations][construct]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  void* allocated = type.allocate();
  type.construct(allocated);

  REQUIRE(static_cast<std::string*>(allocated)->empty());

  type.destruct(allocated);
  type.deallocate(allocated);
}

TEST_CASE(
    "Given an RTTIStringType descriptor and null storage, when construct is called through RTTIType, then "
    "the operation is safe and does nothing",
    "[rtti][string_type][operations][construct][negative]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  type.construct(nullptr);
  SUCCEED();
}

TEST_CASE(
    "Given an RTTIStringType descriptor, when create is called through RTTIType, then a default-constructed "
    "heap string is returned",
    "[rtti][string_type][operations][create]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  void* created = type.create();

  REQUIRE(created != nullptr);
  REQUIRE(static_cast<std::string*>(created)->empty());

  type.destroy(created);
}

TEST_CASE(
    "Given an RTTIStringType descriptor, when create followed by assign is called, then the created instance "
    "holds the assigned value after assignment",
    "[rtti][string_type][operations][create][assign]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  void* created = type.create();
  REQUIRE(created != nullptr);

  const std::string source = "engine";
  type.assign(created, &source);

  REQUIRE(*static_cast<std::string*>(created) == "engine");

  type.destroy(created);
}

TEST_CASE(
    "Given an RTTIStringType descriptor and a null instance pointer, when destroy is called through RTTIType, "
    "then the operation is safe and does nothing",
    "[rtti][string_type][operations][destroy][negative]") {
  const RTTIStringType descriptor;
  const RTTIType& type = descriptor;

  type.destroy(nullptr);
  SUCCEED();
}

TEST_CASE(
    "Given an RTTIStringType descriptor, when two descriptors are constructed independently, then both report "
    "identical metadata",
    "[rtti][string_type][metadata]") {
  const RTTIStringType a;
  const RTTIStringType b;

  REQUIRE(a.kind() == b.kind());
  REQUIRE(a.size() == b.size());
  REQUIRE(a.alignment() == b.alignment());
  REQUIRE(a.name() == b.name());
}
