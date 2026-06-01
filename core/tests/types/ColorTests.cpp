#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <type_traits>

#include "rtti/RTTIName.hpp"
#include "types/Color.hpp"

TEST_CASE("Given a default Color, when constructed, then all channels are initialized to zero",
          "[types][color][construction]") {
  const core::Color color{};

  REQUIRE(color.r == 0u);
  REQUIRE(color.g == 0u);
  REQUIRE(color.b == 0u);
  REQUIRE(color.a == 0u);
}

TEST_CASE("Given two equal Color values, when operator== is used, then it returns true", "[types][color][equality]") {
  const core::Color a{255u, 128u, 64u, 32u};
  const core::Color b{255u, 128u, 64u, 32u};

  REQUIRE(a == b);
}

TEST_CASE("Given two different Color values, when operator== is used, then it returns false",
          "[types][color][equality][negative]") {
  const core::Color a{255u, 128u, 64u, 32u};
  const core::Color c{255u, 128u, 63u, 32u};

  REQUIRE_FALSE(a == c);
}

TEST_CASE("Given two equal Color values, when operator!= is used, then it returns false",
          "[types][color][inequality]") {
  const core::Color a{255u, 128u, 64u, 32u};
  const core::Color b{255u, 128u, 64u, 32u};

  REQUIRE_FALSE(a != b);
}

TEST_CASE("Given two different Color values, when operator!= is used, then it returns true",
          "[types][color][inequality][negative]") {
  const core::Color a{255u, 128u, 64u, 32u};
  const core::Color c{255u, 128u, 63u, 32u};

  REQUIRE(a != c);
}

TEST_CASE("Given Color type metadata, when GetRTTIName is queried, then the registered name is returned",
          "[types][color][type_name]") {
  constexpr auto typeName = core::rtti::GetRTTIName<core::Color>();
  REQUIRE(static_cast<std::string_view>(typeName) == "Color");
}

TEST_CASE("Given Color RTTI expectations, when trivial-copyability is checked, then it is trivially copyable",
          "[types][color][traits]") {
  REQUIRE(std::is_trivially_copyable_v<core::Color>);
}
