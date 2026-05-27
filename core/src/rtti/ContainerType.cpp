#include "rtti/ContainerType.hpp"

namespace core::rtti {
IContainerType::IContainerType(const IName& name, const std::size_t size, const std::size_t alignment,
                               const IType* inner, const TypeKind kind) noexcept
    : IType(name, size, alignment, kind), m_inner(inner) {}

const IType* IContainerType::inner() const noexcept {
  return m_inner;
}
}  // namespace core::rtti
