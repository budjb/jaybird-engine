#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <type_traits>

#include "rtti/RTTIName.hpp"
#include "types/Vector3.hpp"

TEST_CASE("Given a default Vector3, when constructed, then all components are initialized to zero",
          "[types][vector3][construction]") {
  const core::rtti::Vector3 vector{};

  REQUIRE(vector.x == 0.0f);
  REQUIRE(vector.y == 0.0f);
  REQUIRE(vector.z == 0.0f);
}

TEST_CASE("Given two equal Vector3 values, when operator== is used, then it returns true",
          "[types][vector3][equality]") {
  const core::rtti::Vector3 a{1.25f, -2.5f, 5.0f};
  const core::rtti::Vector3 b{1.25f, -2.5f, 5.0f};

  REQUIRE(a == b);
}

TEST_CASE("Given two different Vector3 values, when operator== is used, then it returns false",
          "[types][vector3][equality][negative]") {
  const core::rtti::Vector3 a{1.25f, -2.5f, 5.0f};
  const core::rtti::Vector3 c{5.0f, -2.5f, 1.25f};

  REQUIRE_FALSE(a == c);
}

TEST_CASE("Given two equal Vector3 values, when operator!= is used, then it returns false",
          "[types][vector3][inequality]") {
  const core::rtti::Vector3 a{1.25f, -2.5f, 5.0f};
  const core::rtti::Vector3 b{1.25f, -2.5f, 5.0f};

  REQUIRE_FALSE(a != b);
}

TEST_CASE("Given two different Vector3 values, when operator!= is used, then it returns true",
          "[types][vector3][inequality][negative]") {
  const core::rtti::Vector3 a{1.25f, -2.5f, 5.0f};
  const core::rtti::Vector3 c{5.0f, -2.5f, 1.25f};

  REQUIRE(a != c);
}

TEST_CASE("Given Vector3 type metadata, when GetRTTIName is queried, then the registered name is returned",
          "[types][vector3][type_name]") {
  constexpr auto typeName = core::rtti::GetRTTIName<core::rtti::Vector3>();
  REQUIRE(static_cast<std::string_view>(typeName) == "Vector3");
}

TEST_CASE("Given Vector3 RTTI expectations, when trivial-copyability is checked, then it is trivially copyable",
          "[types][vector3][traits]") {
  REQUIRE(std::is_trivially_copyable_v<core::rtti::Vector3>);
}
