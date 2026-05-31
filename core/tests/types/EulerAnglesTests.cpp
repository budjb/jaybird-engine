#include <catch2/catch_test_macros.hpp>
#include <string_view>

#include "rtti/RTTITypeName.hpp"
#include "types/EulerAngles.hpp"

TEST_CASE("Given default EulerAngles, when constructed, then all angle components are zero",
          "[types][euler_angles][construction]") {
  const core::EulerAngles e{};

  REQUIRE(e.roll == 0.0f);
  REQUIRE(e.pitch == 0.0f);
  REQUIRE(e.yaw == 0.0f);
}

TEST_CASE("Given two equal EulerAngles values, when operator== is used, then it returns true",
          "[types][euler_angles][equality]") {
  const core::EulerAngles a{0.25f, 0.5f, 1.0f};
  const core::EulerAngles b{0.25f, 0.5f, 1.0f};

  REQUIRE(a == b);
}

TEST_CASE("Given two different EulerAngles values, when operator== is used, then it returns false",
          "[types][euler_angles][equality][negative]") {
  const core::EulerAngles a{0.25f, 0.5f, 1.0f};
  const core::EulerAngles c{1.0f, 0.5f, 0.25f};

  REQUIRE_FALSE(a == c);
}

TEST_CASE("Given two equal EulerAngles values, when operator!= is used, then it returns false",
          "[types][euler_angles][inequality]") {
  const core::EulerAngles a{0.25f, 0.5f, 1.0f};
  const core::EulerAngles b{0.25f, 0.5f, 1.0f};

  REQUIRE_FALSE(a != b);
}

TEST_CASE("Given two different EulerAngles values, when operator!= is used, then it returns true",
          "[types][euler_angles][inequality][negative]") {
  const core::EulerAngles a{0.25f, 0.5f, 1.0f};
  const core::EulerAngles c{1.0f, 0.5f, 0.25f};

  REQUIRE(a != c);
}

TEST_CASE("Given EulerAngles type metadata, when GetTypeName is queried, then the registered name is returned",
          "[types][euler_angles][type_name]") {
  constexpr auto typeName = core::rtti::GetTypeName<core::EulerAngles>();
  REQUIRE(static_cast<std::string_view>(typeName) == "EulerAngles");
}
