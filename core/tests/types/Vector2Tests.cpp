#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <type_traits>

#include "rtti/RTTIName.hpp"
#include "types/Vector2.hpp"

TEST_CASE("Given a default Vector2, when constructed, then both components are initialized to zero",
          "[types][vector2][construction]") {
  const core::rtti::Vector2 vector{};

  REQUIRE(vector.x == 0.0f);
  REQUIRE(vector.y == 0.0f);
}

TEST_CASE("Given two equal Vector2 values, when operator== is used, then it returns true",
          "[types][vector2][equality]") {
  const core::rtti::Vector2 a{1.25f, -2.5f};
  const core::rtti::Vector2 b{1.25f, -2.5f};

  REQUIRE(a == b);
}

TEST_CASE("Given two different Vector2 values, when operator== is used, then it returns false",
          "[types][vector2][equality][negative]") {
  const core::rtti::Vector2 a{1.25f, -2.5f};
  const core::rtti::Vector2 c{-2.5f, 1.25f};

  REQUIRE_FALSE(a == c);
}

TEST_CASE("Given two equal Vector2 values, when operator!= is used, then it returns false",
          "[types][vector2][inequality]") {
  const core::rtti::Vector2 a{1.25f, -2.5f};
  const core::rtti::Vector2 b{1.25f, -2.5f};

  REQUIRE_FALSE(a != b);
}

TEST_CASE("Given two different Vector2 values, when operator!= is used, then it returns true",
          "[types][vector2][inequality][negative]") {
  const core::rtti::Vector2 a{1.25f, -2.5f};
  const core::rtti::Vector2 c{-2.5f, 1.25f};

  REQUIRE(a != c);
}

TEST_CASE("Given Vector2 type metadata, when GetRTTIName is queried, then the registered name is returned",
          "[types][vector2][type_name]") {
  constexpr auto typeName = core::rtti::GetRTTIName<core::rtti::Vector2>();
  REQUIRE(static_cast<std::string_view>(typeName) == "Vector2");
}

TEST_CASE("Given Vector2 RTTI expectations, when trivial-copyability is checked, then it is trivially copyable",
          "[types][vector2][traits]") {
  REQUIRE(std::is_trivially_copyable_v<core::rtti::Vector2>);
}
