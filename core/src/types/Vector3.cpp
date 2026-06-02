#include "types/Vector3.hpp"

namespace core::rtti {
Vector3::Vector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}

Vector3::Vector3(const float x, const float y, const float z) noexcept : x(x), y(y), z(z) {}

bool Vector3::operator==(const Vector3& rhs) const noexcept = default;

bool Vector3::operator!=(const Vector3& rhs) const noexcept = default;
}  // namespace core::rtti
