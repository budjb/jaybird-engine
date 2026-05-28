#include "types/Color.hpp"

namespace core {
Color::Color() noexcept : r(0), g(0), b(0), a(0) {}

Color::Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept : r(r), g(g), b(b), a(a) {}
}  // namespace core
