#include "RTTIPointerType.hpp"

namespace core::rtti {
RTTIPointerType::RTTIPointerType(const Name& name, const std::size_t size, const std::size_t alignment,
                                 const RTTIType* inner) noexcept
    : RTTIContainerType(name, size, alignment, inner, RTTITypeKind::POINTER) {}

RTTIPointerType::~RTTIPointerType() noexcept = default;
}  // namespace core::rtti
