#include "rtti/RTTIClassType.hpp"
#include "rtti/RTTIRegistry.hpp"
#include "rtti/RTTIType.hpp"

namespace core::rtti {
RTTIType* RTTIRegistry::getType(const Name& name) const {
  std::shared_lock lock(m_mutex);

  if (m_types.contains(name)) {
    return m_types.at(name).get();
  }
  return nullptr;
}

RTTIClassType* RTTIRegistry::getClass(const Name& name) const {
  if (auto* type = getType(name); type && type->kind() == RTTITypeKind::CLASS) {
    return reinterpret_cast<RTTIClassType*>(type);
  }
  return nullptr;
}

bool RTTIRegistry::unregisterType(const Name& name) {
  std::unique_lock lock(m_mutex);

  const auto it = m_types.find(name);

  if (it == m_types.end()) {
    return false;
  }

  const auto* type = it->second.get();

  if (type->kind() != RTTITypeKind::ARRAY) {
    unregisterType(GetPrefixedRTTIName<RTTITypeKind::ARRAY>(name));
  }

  if (type->kind() != RTTITypeKind::REF) {
    unregisterType(GetPrefixedRTTIName<RTTITypeKind::REF>(name));
  }

  if (type->kind() != RTTITypeKind::WEAK_REF) {
    unregisterType(GetPrefixedRTTIName<RTTITypeKind::WEAK_REF>(name));
  }

  m_types.erase(it);
  return true;
}

bool RTTIRegistry::hasType(const Name& name) const noexcept {
  std::shared_lock lock(m_mutex);
  return m_types.contains(name);
}
}  // namespace core::rtti
