#pragma once

#include <cstdint>

#include "Export.hpp"
#include "rtti/RTTIName.hpp"

namespace core {
/**
 * @brief Represents a packed RGBA color using 8-bit channels.
 */
class JAYBIRD_API Color {
 public:
  /**
   * @brief Constructs a fully transparent black color.
   */
  Color() noexcept;

  /**
   * @brief Constructs a color from explicit channel values.
   *
   * @param r This parameter provides the red channel.
   * @param g This parameter provides the green channel.
   * @param b This parameter provides the blue channel.
   * @param a This parameter provides the alpha channel.
   */
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept;

  /**
   * @brief Compares two colors for exact channel-wise equality.
   *
   * @return This function returns @c true when all four channels are equal.
   */
  bool operator==(const Color&) const noexcept = default;

  /**
   * @brief Compares two colors for inequality.
   *
   * @return This function returns @c true when at least one channel differs.
   */
  bool operator!=(const Color&) const noexcept = default;

  /**
   * @brief This field stores the red channel.
   */
  uint8_t r;

  /**
   * @brief This field stores the green channel.
   */
  uint8_t g;

  /**
   * @brief This field stores the blue channel.
   */
  uint8_t b;

  /**
   * @brief This field stores the alpha channel.
   */
  uint8_t a;
};
}  // namespace core

REGISTER_TYPE_NAME(core::Color, "Color");
