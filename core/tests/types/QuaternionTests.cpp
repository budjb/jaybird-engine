#include <catch2/catch_test_macros.hpp>
#include <string_view>

#include "rtti/RTTIName.hpp"
#include "types/Quaternion.hpp"

TEST_CASE("Given a default Quaternion, when constructed, then it is initialized to identity values",
          "[types][quaternion][construction]") {
  const core::Quaternion q{};

  REQUIRE(q.x == 0.0f);
  REQUIRE(q.y == 0.0f);
  REQUIRE(q.z == 0.0f);
  REQUIRE(q.w == 1.0f);
}

TEST_CASE("Given two equal Quaternion values, when operator== is used, then it returns true",
          "[types][quaternion][equality]") {
  const core::Quaternion a{1.0f, 2.0f, 3.0f, 4.0f};
  const core::Quaternion b{1.0f, 2.0f, 3.0f, 4.0f};

  REQUIRE(a == b);
}

TEST_CASE("Given two different Quaternion values, when operator== is used, then it returns false",
          "[types][quaternion][equality][negative]") {
  const core::Quaternion a{1.0f, 2.0f, 3.0f, 4.0f};
  const core::Quaternion c{4.0f, 3.0f, 2.0f, 1.0f};

  REQUIRE_FALSE(a == c);
}

TEST_CASE("Given two equal Quaternion values, when operator!= is used, then it returns false",
          "[types][quaternion][inequality]") {
  const core::Quaternion a{1.0f, 2.0f, 3.0f, 4.0f};
  const core::Quaternion b{1.0f, 2.0f, 3.0f, 4.0f};

  REQUIRE_FALSE(a != b);
}

TEST_CASE("Given two different Quaternion values, when operator!= is used, then it returns true",
          "[types][quaternion][inequality][negative]") {
  const core::Quaternion a{1.0f, 2.0f, 3.0f, 4.0f};
  const core::Quaternion c{4.0f, 3.0f, 2.0f, 1.0f};

  REQUIRE(a != c);
}

TEST_CASE("Given Quaternion type metadata, when GetRTTIName is queried, then the registered name is returned",
          "[types][quaternion][type_name]") {
  constexpr auto typeName = core::rtti::GetRTTIName<core::Quaternion>();
  REQUIRE(static_cast<std::string_view>(typeName) == "Quaternion");
}
