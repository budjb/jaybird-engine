#include "types/EulerAngles.hpp"

namespace core {
EulerAngles::EulerAngles() noexcept : roll(0), pitch(0), yaw(0) {}

EulerAngles::EulerAngles(const float roll, const float pitch, const float yaw) noexcept
    : roll(roll), pitch(pitch), yaw(yaw) {}
}  // namespace core
