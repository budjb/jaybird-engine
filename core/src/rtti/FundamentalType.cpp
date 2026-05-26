#include "rtti/FundamentalType.hpp"

namespace core::rtti {
IFundamentalType::IFundamentalType(const IName& name, const std::size_t size, const std::size_t alignment) noexcept
    : IType(name, size, alignment, TypeKind::FUNDAMENTAL) {}

IFundamentalType::~IFundamentalType() = default;
}  // namespace core::rtti
