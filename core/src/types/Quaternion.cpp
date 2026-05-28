#include "types/Quaternion.hpp"

namespace core {
Quaternion::Quaternion() noexcept : x(0), y(0), z(0), w(1) {}

Quaternion::Quaternion(const float x, const float y, const float z, const float w) noexcept : x(x), y(y), z(z), w(w) {}
}  // namespace core
