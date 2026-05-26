#include "rtti/ClassType.hpp"

namespace core::rtti {
IClassType::IClassType(const IName& name, const std::size_t size, const std::size_t alignment) noexcept
    : IType(name, size, alignment, TypeKind::CLASS) {}
}  // namespace core::rtti
