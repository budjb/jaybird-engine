#include "rtti/RTTIFundamentalType.hpp"

namespace core::rtti {
RTTIFundamentalType::RTTIFundamentalType(const IName& name, const std::size_t size,
                                         const std::size_t alignment) noexcept
    : RTTIType(name, size, alignment, RTTITypeKind::FUNDAMENTAL) {}

RTTIFundamentalType::~RTTIFundamentalType() = default;
}  // namespace core::rtti
