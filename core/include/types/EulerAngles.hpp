#pragma once

#include "Export.hpp"
#include "rtti/TypeName.hpp"

namespace core {
/**
 * @brief Represents Euler rotation angles in radians.
 */
class JAYBIRD_API EulerAngles {
 public:
  /**
   * @brief Constructs zero Euler angles.
   */
  EulerAngles() noexcept;

  /**
   * @brief Constructs Euler angles from explicit values.
   *
   * @param roll This parameter provides the roll angle.
   * @param pitch This parameter provides the pitch angle.
   * @param yaw This parameter provides the yaw angle.
   */
  EulerAngles(float roll, float pitch, float yaw) noexcept;

  /**
   * @brief Compares two Euler-angle values for exact component-wise equality.
   *
   * @return This function returns @c true when all three components are equal.
   */
  bool operator==(const EulerAngles&) const noexcept = default;

  /**
   * @brief Compares two Euler-angle values for inequality.
   *
   * @return This function returns @c true when at least one component differs.
   */
  bool operator!=(const EulerAngles&) const noexcept = default;

  /**
   * @brief This field stores the roll angle.
   */
  float roll;

  /**
   * @brief This field stores the pitch angle.
   */
  float pitch;

  /**
   * @brief This field stores the yaw angle.
   */
  float yaw;
};
}  // namespace core

REGISTER_TYPE_NAME(core::EulerAngles, "EulerAngles");
