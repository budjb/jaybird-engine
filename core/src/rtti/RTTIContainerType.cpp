#include "rtti/RTTIContainerType.hpp"

namespace core::rtti {
RTTIContainerType::RTTIContainerType(const IName& name, const std::size_t size, const std::size_t alignment,
                                     const RTTIType* inner, const RTTITypeKind kind) noexcept
    : RTTIType(name, size, alignment, kind), m_inner(inner) {}

const RTTIType* RTTIContainerType::inner() const noexcept {
  return m_inner;
}
}  // namespace core::rtti
