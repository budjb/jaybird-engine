#include "types/Vector2.hpp"

namespace core::rtti {
Vector2::Vector2() noexcept : x(0), y(0) {}

Vector2::Vector2(const float x, const float y) noexcept : x(x), y(y) {}

bool Vector2::operator==(const Vector2& rhs) const noexcept = default;

bool Vector2::operator!=(const Vector2& rhs) const noexcept = default;
}  // namespace core::rtti
