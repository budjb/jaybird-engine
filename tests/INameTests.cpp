#include <catch2/catch_test_macros.hpp>
#include <string>

#include "Hash.hpp"
#include "IName.hpp"
#include "INamePool.hpp"

namespace {
using core::fnv1a_64;
using core::IName;
using core::INamePool;

std::string uniqueNameText(const char* prefix) {
  static std::size_t counter = 0;
  ++counter;
  return std::string(prefix) + "_" + std::to_string(counter);
}

}  // namespace

TEST_CASE("Given a default-constructed IName, when queried, then it reports empty invalid and zero hash", "[iname]") {
  constexpr IName name;

  REQUIRE(name.empty());
  REQUIRE_FALSE(static_cast<bool>(name));
  REQUIRE(name.hash() == 0);
  REQUIRE(static_cast<core::hash_t>(name) == 0);
}

TEST_CASE(
    "Given an IName constructed from string text, when queried, then it exposes the expected FNV-1a hash and valid "
    "state",
    "[iname]") {
  const std::string text = uniqueNameText("iname_hash");
  const IName name(text);

  REQUIRE_FALSE(name.empty());
  REQUIRE(static_cast<bool>(name));
  REQUIRE(name.hash() == fnv1a_64(text));
}

TEST_CASE(
    "Given IName instances built from equal and different hashes, when compared, then equality follows hash identity",
    "[iname]") {
  const std::string text = uniqueNameText("iname_compare");
  const IName fromString(text);
  const IName fromHash(fromString.hash());
  const IName different(uniqueNameText("iname_compare_other"));

  REQUIRE(fromString == fromHash);
  REQUIRE(fromString != different);
}

TEST_CASE(
    "Given an IName created from text but not interned, when checked in INamePool, then the pool reports it as missing",
    "[iname]") {
  const std::string text = uniqueNameText("iname_not_in_pool");
  const IName name(text);

  const INamePool& pool = INamePool::get();
  REQUIRE_FALSE(pool.hasName(name));
}

TEST_CASE("Given an interned IName, when converted to string representations, then the pooled text is returned",
          "[iname]") {
  INamePool& pool = INamePool::get();
  const std::string text = uniqueNameText("iname_to_string");
  const IName name = pool.addName(text);

  REQUIRE(name.toString() == text);
  REQUIRE(static_cast<std::string_view>(name) == text);
}

TEST_CASE(
    "Given non-explicit IName constructors, when assigned from string literal and std::string, then the same hash is "
    "produced",
    "[iname]") {
  const std::string text = uniqueNameText("iname_implicit_ctor");

  const IName fromLiteral = text.c_str();
  const IName fromString = std::string(text);

  REQUIRE(fromLiteral.hash() == fnv1a_64(text));
  REQUIRE(fromString.hash() == fnv1a_64(text));
  REQUIRE(fromLiteral == fromString);
}

TEST_CASE(
    "Given non-explicit IName constructors, when passed to a function by value, then implicit conversion works for "
    "string literal and std::string",
    "[iname]") {
  const std::string text = uniqueNameText("iname_implicit_param");

  auto hashOf = [](const IName name) { return name.hash(); };

  REQUIRE(hashOf(text.c_str()) == fnv1a_64(text));
  REQUIRE(hashOf(std::string(text)) == fnv1a_64(text));
}
