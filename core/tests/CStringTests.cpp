#include <catch2/catch_test_macros.hpp>
#include <string>
#include <string_view>

#include "CString.hpp"
#include "Hash.hpp"
#include "IName.hpp"

namespace {
using core::CString;
using core::IName;

static_assert(core::is_cstring_v<CString<4>>);
static_assert(core::is_cstring_v<const CString<4>&>);
static_assert(core::is_char_array_v<char[4]>);
static_assert(core::is_char_array_v<const char[4]>);
static_assert(core::is_cstring_convertible_v<CString<4>>);
static_assert(core::is_cstring_convertible_v<const char[4]>);

}  // namespace

TEST_CASE(
    "Given a CString created from a string literal, when querying size length and text view, then values match the "
    "literal",
    "[cstring]") {
  constexpr CString text("bird");

  REQUIRE(text.size() == 5);
  REQUIRE(text.length() == 4);
  REQUIRE_FALSE(text.empty());
  REQUIRE(static_cast<std::string_view>(text) == "bird");
  REQUIRE(std::string(text.c_str()) == "bird");
}

TEST_CASE("Given an empty CString, when converted to bool, then conversion follows the empty-state semantics",
          "[cstring]") {
  constexpr CString empty("");

  REQUIRE(empty.empty());
  REQUIRE(static_cast<bool>(empty));
}

TEST_CASE(
    "Given a non-empty CString, when converted to bool, then conversion returns false because bool mirrors empty()",
    "[cstring]") {
  constexpr CString nonEmpty("x");

  REQUIRE_FALSE(nonEmpty.empty());
  REQUIRE_FALSE(static_cast<bool>(nonEmpty));
}

TEST_CASE(
    "Given two CStrings, when append and operator+ are used, then a compile-time CString concatenation is produced",
    "[cstring]") {
  constexpr CString left("ref:");
  constexpr CString right("int");
  constexpr auto combined = left.append(right);
  constexpr auto plusCombined = left + right;

  REQUIRE(static_cast<std::string_view>(combined) == "ref:int");
  REQUIRE(static_cast<std::string_view>(plusCombined) == "ref:int");
  REQUIRE(combined.size() == 8);
}

TEST_CASE(
    "Given a CString and a runtime string_view, when append and operator+ are used, then the result is a std::string",
    "[cstring]") {
  constexpr CString prefix("array:");
  constexpr std::string_view suffix = "float";

  const std::string appended = prefix.append(suffix);
  const std::string plusAppended = prefix + suffix;

  REQUIRE(appended == "array:float");
  REQUIRE(plusAppended == "array:float");
}

TEST_CASE("Given a CString, when converted to IName, then the resulting hash matches fnv1a_64 of its text",
          "[cstring]") {
  constexpr CString text("convert_me");
  const IName name = text;

  REQUIRE(name.hash() == core::fnv1a_64("convert_me"));
}
