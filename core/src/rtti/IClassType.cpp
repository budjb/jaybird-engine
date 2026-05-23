#include "rtti/IClassType.hpp"

namespace core::rtti {
IClassType::IClassType(const IString& name, const std::size_t size, const std::size_t alignment) noexcept
    : IType(name, size, alignment, TypeKind::CLASS) {}
}  // namespace core::rtti
