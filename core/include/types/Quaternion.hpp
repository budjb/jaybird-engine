#pragma once

#include "Export.hpp"
#include "rtti/RTTIName.hpp"

namespace core {
/**
 * @brief Represents a quaternion with four floating-point components.
 */
class JAYBIRD_API Quaternion {
 public:
  /**
   * @brief Constructs an identity quaternion.
   */
  Quaternion() noexcept;

  /**
   * @brief Constructs a quaternion from explicit component values.
   *
   * @param x This parameter provides the @c x component.
   * @param y This parameter provides the @c y component.
   * @param z This parameter provides the @c z component.
   * @param w This parameter provides the @c w component.
   */
  Quaternion(float x, float y, float z, float w) noexcept;

  /**
   * @brief Compares two quaternions for exact component-wise equality.
   *
   * @return This function returns @c true when all four components match exactly.
   */
  bool operator==(const Quaternion&) const noexcept = default;

  /**
   * @brief Compares two quaternions for inequality.
   *
   * @return This function returns @c true when at least one component differs.
   */
  bool operator!=(const Quaternion&) const noexcept = default;

  /**
   * @brief This field stores the @c x component.
   */
  float x;

  /**
   * @brief This field stores the @c y component.
   */
  float y;

  /**
   * @brief This field stores the @c z component.
   */
  float z;

  /**
   * @brief This field stores the @c w component.
   */
  float w;
};
}  // namespace core

REGISTER_TYPE_NAME(core::Quaternion, "Quaternion");
