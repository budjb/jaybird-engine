#include "rtti/RTTIClassType.hpp"
#include "rtti/RTTIType.hpp"
#include "rtti/RTTITypeRegistry.hpp"

namespace core::rtti {
RTTIType* RTTITypeRegistry::getType(const IName& name) const {
  std::shared_lock lock(m_mutex);

  if (m_types.contains(name)) {
    return m_types.at(name).get();
  }
  return nullptr;
}

RTTIClassType* RTTITypeRegistry::getClass(const IName& name) const {
  if (auto* type = getType(name); type && type->kind() == RTTITypeKind::CLASS) {
    return reinterpret_cast<RTTIClassType*>(type);
  }
  return nullptr;
}

bool RTTITypeRegistry::unregisterType(const IName& name) {
  std::unique_lock lock(m_mutex);

  const auto it = m_types.find(name);

  if (it == m_types.end()) {
    return false;
  }

  const auto* type = it->second.get();

  if (type->kind() != RTTITypeKind::ARRAY) {
    unregisterType(GetPrefixedTypeName<RTTITypeKind::ARRAY>(name));
  }

  if (type->kind() != RTTITypeKind::REF) {
    unregisterType(GetPrefixedTypeName<RTTITypeKind::REF>(name));
  }

  if (type->kind() != RTTITypeKind::WEAK_REF) {
    unregisterType(GetPrefixedTypeName<RTTITypeKind::WEAK_REF>(name));
  }

  m_types.erase(it);
  return true;
}

bool RTTITypeRegistry::hasType(const IName& name) const noexcept {
  std::shared_lock lock(m_mutex);
  return m_types.contains(name);
}
}  // namespace core::rtti
