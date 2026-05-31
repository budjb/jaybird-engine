#include "rtti/TypeRegistry.hpp"

#include "rtti/ClassType.hpp"
#include "rtti/IType.hpp"

namespace core::rtti {
IType* TypeRegistry::getType(const IName& name) const {
  std::shared_lock lock(m_mutex);

  if (m_types.contains(name)) {
    return m_types.at(name).get();
  }
  return nullptr;
}

IClassType* TypeRegistry::getClass(const IName& name) const {
  if (auto* type = getType(name); type && type->kind() == TypeKind::CLASS) {
    return reinterpret_cast<IClassType*>(type);
  }
  return nullptr;
}

bool TypeRegistry::unregisterType(const IName& name) {
  std::unique_lock lock(m_mutex);

  const auto it = m_types.find(name);

  if (it == m_types.end()) {
    return false;
  }

  const auto* type = it->second.get();

  if (type->kind() != TypeKind::ARRAY) {
    unregisterType(GetPrefixedTypeName<TypeKind::ARRAY>(name));
  }

  if (type->kind() != TypeKind::REF) {
    unregisterType(GetPrefixedTypeName<TypeKind::REF>(name));
  }

  if (type->kind() != TypeKind::WEAK_REF) {
    unregisterType(GetPrefixedTypeName<TypeKind::WEAK_REF>(name));
  }

  m_types.erase(it);
  return true;
}

bool TypeRegistry::hasType(const IName& name) const noexcept {
  std::shared_lock lock(m_mutex);
  return m_types.contains(name);
}
}  // namespace core::rtti
