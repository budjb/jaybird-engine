#include "rtti/RTTIClassType.hpp"

namespace core::rtti {
RTTIClassType::RTTIClassType(const IName& name, const std::size_t size, const std::size_t alignment) noexcept
    : RTTIType(name, size, alignment, RTTITypeKind::CLASS) {}
}  // namespace core::rtti
