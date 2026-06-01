#include "rtti/RTTIType.hpp"

#include "rtti/RTTIArrayType.hpp"
#include "rtti/RTTIName.hpp"
#include "rtti/RTTIRegistry.hpp"
#include "rtti/RTTISystem.hpp"

namespace core::rtti {
RTTIType::RTTIType(const Name& name, const std::size_t size, const RTTITypeKind kind) noexcept
    : RTTIType(name, size, alignof(std::max_align_t), kind) {}

RTTIType::RTTIType(const Name& name, const std::size_t size, const std::size_t alignment,
                   const RTTITypeKind kind) noexcept
    : m_name(name), m_size(size), m_alignment(alignment), m_kind(kind) {}

std::size_t RTTIType::size() const noexcept {
  return m_size;
}

std::size_t RTTIType::alignment() const noexcept {
  return m_alignment;
}

RTTITypeKind RTTIType::kind() const noexcept {
  return m_kind;
}

Name RTTIType::name() const noexcept {
  return m_name;
}

RTTIType* RTTIType::parent() const noexcept {
  return m_parent;
}

void RTTIType::parent(RTTIType* parent) noexcept {
  m_parent = parent;
}

RTTIArrayType* RTTIType::asArray() const noexcept {
  if (auto* type = RTTISystem::get().registry().getType(GetPrefixedRTTIName<RTTITypeKind::ARRAY>(m_name));
      type && type->kind() == RTTITypeKind::ARRAY) {
    return reinterpret_cast<RTTIArrayType*>(type);
  }
  return nullptr;
}

bool RTTIType::operator==(const RTTIType& type) const noexcept {
  return m_name == type.m_name;
}

bool RTTIType::operator!=(const RTTIType& type) const noexcept {
  return m_name != type.m_name;
}
}  // namespace core::rtti
