#include "rtti/IType.hpp"

#include "rtti/IArrayType.hpp"
#include "rtti/TypeRegistry.hpp"

namespace core::rtti {
IType::IType(const IName& name, const std::size_t size, const TypeKind kind) noexcept
    : IType(name, size, alignof(std::max_align_t), kind) {}

IType::IType(const IName& name, const std::size_t size, const std::size_t alignment, const TypeKind kind) noexcept
    : m_name(name), m_size(size), m_alignment(alignment), m_kind(kind) {}

std::size_t IType::size() const noexcept {
  return m_size;
}

std::size_t IType::alignment() const noexcept {
  return m_alignment;
}

TypeKind IType::kind() const noexcept {
  return m_kind;
}

IName IType::name() const noexcept {
  return m_name;
}

IArrayType* IType::asArray() const noexcept {
  if (auto* type = TypeRegistry::get()->getType(typePrefix<TypeKind::ARRAY>(m_name));
      type && type->kind() == TypeKind::ARRAY) {
    return reinterpret_cast<IArrayType*>(type);
  }
  return nullptr;
}

bool IType::operator==(const IType& type) const noexcept {
  return m_name == type.m_name;
}

bool IType::operator!=(const IType& type) const noexcept {
  return m_name != type.m_name;
}
}  // namespace core::rtti
