#include "rtti/RTTIFundamentalType.hpp"

namespace core::rtti {
RTTIFundamentalType::RTTIFundamentalType(const Name& name, const std::size_t size, const std::size_t alignment) noexcept
    : RTTIType(name, size, alignment, RTTITypeKind::FUNDAMENTAL) {}

RTTIFundamentalType::~RTTIFundamentalType() = default;
}  // namespace core::rtti
