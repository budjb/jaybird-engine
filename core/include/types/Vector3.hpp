#pragma once

#include "rtti/RTTIName.hpp"

namespace core::rtti {
/**
 * @brief Represents a 3D vector with three floating-point components.
 */
class JAYBIRD_API Vector3 {
 public:
  /**
   * @brief Constructs a zero vector (0, 0, 0).
   */
  Vector3() noexcept;

  /**
   * @brief Constructs a vector with specified component values.
   *
   * @param x The x component of the vector.
   * @param y The y component of the vector.
   * @param z The z component of the vector.
   */
  Vector3(float x, float y, float z) noexcept;

  /**
   * @brief Compares two vectors for exact component-wise equality.
   *
   * @param rhs The vector to compare against.
   * @return This function returns @c true when all three components are equal.
   */
  bool operator==(const Vector3& rhs) const noexcept;

  /**
   * @brief Compares two vectors for inequality.
   *
   * @param rhs The vector to compare against.
   * @return This function returns @c true when at least one component differs.
   */
  bool operator!=(const Vector3& rhs) const noexcept;

  /**
   * @brief The x component of the vector.
   */
  float x;

  /**
   * @brief The y component of the vector.
   */
  float y;

  /**
   * @brief The z component of the vector.
   */
  float z;
};
}  // namespace core::rtti

REGISTER_TYPE_NAME(core::rtti::Vector3, "Vector3");
