#include "rtti/RTTIClassType.hpp"

namespace core::rtti {
RTTIClassType::RTTIClassType(const Name& name, const std::size_t size, const std::size_t alignment,
                             const bool isTrivial) noexcept
    : RTTIType(name, size, alignment, isTrivial ? RTTITypeKind::SIMPLE : RTTITypeKind::CLASS), m_trivial(isTrivial) {}

bool RTTIClassType::isTrivial() const noexcept {
  return m_trivial;
}

void RTTIClassType::property(std::shared_ptr<RTTIProperty>&& property) noexcept {
  std::unique_lock lock(m_propertiesMutex);
  m_properties[property->name()] = std::move(property);
}

void RTTIClassType::property(const std::shared_ptr<RTTIProperty>& property) noexcept {
  std::unique_lock lock(m_propertiesMutex);
  m_properties[property->name()] = property;
}

std::shared_ptr<RTTIProperty> RTTIClassType::property(const Name& name) noexcept {
  std::shared_lock lock(m_propertiesMutex);
  const auto it = m_properties.find(name);
  return it != m_properties.end() ? it->second : nullptr;
}

void RTTIClassType::function(std::shared_ptr<RTTIClassFunction>&& function) noexcept {
  std::unique_lock lock(m_functionsMutex);
  m_functions[function->name()] = std::move(function);
}

void RTTIClassType::function(const std::shared_ptr<RTTIClassFunction>& function) noexcept {
  std::unique_lock lock(m_functionsMutex);
  m_functions[function->name()] = function;
}

std::shared_ptr<RTTIClassFunction> RTTIClassType::function(const Name& name) noexcept {
  std::shared_lock lock(m_functionsMutex);
  const auto it = m_functions.find(name);
  return it != m_functions.end() ? it->second : nullptr;
}
}  // namespace core::rtti
