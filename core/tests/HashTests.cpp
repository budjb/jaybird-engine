#include <array>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <string_view>

#include "Hash.hpp"

namespace {
using core::fnv1a_64;

constexpr core::hash_t kFnvOffsetBasis = 14695981039346656037ULL;

}  // namespace

TEST_CASE("Given an empty string, when hashed with fnv1a_64, then the FNV-1a offset basis is returned", "[hash]") {
  REQUIRE(fnv1a_64("") == kFnvOffsetBasis);
}

TEST_CASE(
    "Given equivalent string inputs, when hashed with fnv1a_64, then the resulting hash is identical regardless of "
    "string representation",
    "[hash]") {
  const char* textLiteral = "jaybird_hash_consistency";
  const std::string textString(textLiteral);
  const std::string_view textView(textLiteral);

  const core::hash_t fromLiteral = fnv1a_64(textLiteral);
  const core::hash_t fromString = fnv1a_64(textString);
  const core::hash_t fromView = fnv1a_64(textView);

  REQUIRE(fromLiteral == fromString);
  REQUIRE(fromString == fromView);
}

TEST_CASE(
    "Given two different strings, when hashed with fnv1a_64, then the resulting hashes differ for a one-byte "
    "difference",
    "[hash]") {
  REQUIRE(fnv1a_64("jaybird_hash_A") != fnv1a_64("jaybird_hash_B"));
}

TEST_CASE(
    "Given a string view containing embedded null bytes, when hashed with fnv1a_64, then all bytes in the view "
    "participate in the hash",
    "[hash]") {
  constexpr std::array bytes{'a', '\0', 'b'};
  const std::string_view withNull{bytes.data(), bytes.size()};

  REQUIRE(fnv1a_64(withNull) != fnv1a_64("a"));
}
