#include "rtti/IContainerType.hpp"

namespace core::rtti {
IContainerType::IContainerType(const IString& name, const std::size_t size, const std::size_t alignment,
                       const IType* inner) noexcept
    : IType(name, size, alignment, TypeKind::ARRAY), m_inner(inner) {}

const IType* IContainerType::inner() const noexcept {
  return m_inner;
}

}  // namespace core::rtti
