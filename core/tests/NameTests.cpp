#include <catch2/catch_test_macros.hpp>
#include <string>

#include "Hash.hpp"
#include "NamePool.hpp"
#include "Name.hpp"

namespace {
using core::fnv1a_64;
using core::Name;
using core::NamePool;

std::string uniqueNameText(const char* prefix) {
  static std::size_t counter = 0;
  ++counter;
  return std::string(prefix) + "_" + std::to_string(counter);
}

}  // namespace

TEST_CASE("Given a default-constructed Name, when queried, then it reports empty invalid and zero hash", "[iname]") {
  constexpr Name name;

  REQUIRE(name.empty());
  REQUIRE_FALSE(static_cast<bool>(name));
  REQUIRE(name.hash() == 0);
  REQUIRE(static_cast<core::hash_t>(name) == 0);
}

TEST_CASE(
    "Given an Name constructed from string text, when queried, then it exposes the expected FNV-1a hash and valid "
    "state",
    "[iname]") {
  const std::string text = uniqueNameText("iname_hash");
  const Name name(text);

  REQUIRE_FALSE(name.empty());
  REQUIRE(static_cast<bool>(name));
  REQUIRE(name.hash() == fnv1a_64(text));
}

TEST_CASE(
    "Given Name instances built from equal and different hashes, when compared, then equality follows hash identity",
    "[iname]") {
  const std::string text = uniqueNameText("iname_compare");
  const Name fromString(text);
  const Name fromHash(fromString.hash());
  const Name different(uniqueNameText("iname_compare_other"));

  REQUIRE(fromString == fromHash);
  REQUIRE(fromString != different);
}

TEST_CASE(
    "Given an Name created from text but not interned, when checked in NamePool, then the pool reports it as missing",
    "[iname]") {
  const std::string text = uniqueNameText("iname_not_in_pool");
  const Name name(text);

  const NamePool& pool = NamePool::get();
  REQUIRE_FALSE(pool.hasName(name));
}

TEST_CASE("Given an interned Name, when converted to string representations, then the pooled text is returned",
          "[iname]") {
  NamePool& pool = NamePool::get();
  const std::string text = uniqueNameText("iname_to_string");
  const Name name = pool.addName(text);

  REQUIRE(name.toString() == text);
  REQUIRE(static_cast<std::string_view>(name) == text);
}

TEST_CASE(
    "Given non-explicit Name constructors, when assigned from string literal and std::string, then the same hash is "
    "produced",
    "[iname]") {
  const std::string text = uniqueNameText("iname_implicit_ctor");

  const Name fromLiteral = text.c_str();
  const Name fromString = std::string(text);

  REQUIRE(fromLiteral.hash() == fnv1a_64(text));
  REQUIRE(fromString.hash() == fnv1a_64(text));
  REQUIRE(fromLiteral == fromString);
}

TEST_CASE(
    "Given non-explicit Name constructors, when passed to a function by value, then implicit conversion works for "
    "string literal and std::string",
    "[iname]") {
  const std::string text = uniqueNameText("iname_implicit_param");

  auto hashOf = [](const Name name) { return name.hash(); };

  REQUIRE(hashOf(text.c_str()) == fnv1a_64(text));
  REQUIRE(hashOf(std::string(text)) == fnv1a_64(text));
}
