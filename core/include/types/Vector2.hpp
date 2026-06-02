#pragma once

#include "rtti/RTTIName.hpp"

namespace core::rtti {
/**
 * @brief Represents a 2D vector with two floating-point components.
 */
class JAYBIRD_API Vector2 {
 public:
  /**
   * @brief Constructs a zero vector (0, 0).
   */
  Vector2() noexcept;

  /**
   * @brief Constructs a vector with specified component values.
   *
   * @param x The x component of the vector.
   * @param y The y component of the vector.
   */
  Vector2(float x, float y) noexcept;

  /**
   * @brief Compares two vectors for exact component-wise equality.
   *
   * @param rhs The vector to compare against.
   * @return This function returns @c true when all three components are equal.
   */
  bool operator==(const Vector2& rhs) const noexcept;

  /**
   * @brief Compares two vectors for inequality.
   *
   * @param rhs The vector to compare against.
   * @return This function returns @c true when at least one component differs.
   */
  bool operator!=(const Vector2& rhs) const noexcept;

  /**
   * @brief The x component of the vector.
   */
  float x;

  /**
   * @brief The y component of the vector.
   */
  float y;
};
}  // namespace core::rtti

REGISTER_TYPE_NAME(core::rtti::Vector2, "Vector2");
