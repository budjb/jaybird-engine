#include "rtti/TypeRegistry.hpp"

#include <mutex>

#include "rtti/IType.hpp"

namespace core::rtti {
TypeRegistry* TypeRegistry::get() {
  static TypeRegistry instance;
  return &instance;
}

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

IType* TypeRegistry::registerType(std::unique_ptr<IType>&& type) {
  std::unique_lock lock(m_mutex);
  if (m_types.contains(type->name())) {
    // TODO: log? throw?
    return nullptr;
  }

  if (auto [it, success] = m_types.insert({type->name(), std::move(type)}); success) {
    return it->second.get();
  }

  return nullptr;
}

bool TypeRegistry::hasType(const IName& name) const noexcept {
  std::shared_lock lock(m_mutex);
  return m_types.contains(name);
}
}  // namespace core::rtti
