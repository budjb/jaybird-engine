#include "rtti/RTTI.hpp"

namespace core::rtti {
IType::IType(const IString& name, const std::size_t size, const TypeKind kind) noexcept
    : IType(name, size, alignof(std::max_align_t), kind) {}

IType::IType(const IString& name, const std::size_t size, const std::size_t alignment, const TypeKind kind) noexcept
    : m_name(name), m_size(size), m_alignment(alignment), m_kind(kind) {}

std::size_t IType::size() const noexcept { return m_size; }

std::size_t IType::alignment() const noexcept { return m_alignment; }

TypeKind IType::kind() const noexcept { return m_kind; }

IString IType::name() const noexcept { return m_name; }

void* IType::create() const {
  void* memory = operator new(m_size, static_cast<std::align_val_t>(m_alignment));

  if (!construct(memory)) {
    operator delete(memory, static_cast<std::align_val_t>(m_alignment));
    return nullptr;
  }

  return memory;
}

void IType::destroy(void* memory) const noexcept {
  destruct(memory);
  operator delete(memory, static_cast<std::align_val_t>(m_alignment));
}

bool IType::operator==(const IType& type) const noexcept { return m_name == type.m_name; }

bool IType::operator!=(const IType& type) const noexcept { return m_name != type.m_name; }

IClass::IClass(const IString& name, const std::size_t size) noexcept : IType(name, size, TypeKind::CLASS) {}

IClass::IClass(const IString& name, const std::size_t size, const std::size_t alignment) noexcept
    : IType(name, size, alignment, TypeKind::CLASS) {}
}  // namespace core::rtti
