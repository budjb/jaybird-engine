#include "rtti/RTTIRegistry.hpp"

#include <mutex>

namespace core::rtti {
RTTIRegistry* RTTIRegistry::get() {
  static RTTIRegistry instance;
  return &instance;
}

IType* RTTIRegistry::getType(const IString& name) {
  std::shared_lock lock(m_mutex);

  if (m_types.contains(name)) {
    return m_types.at(name).get();
  }
  return nullptr;
}

IClass* RTTIRegistry::getClass(const IString& name) {
  if (auto* type = getType(name); type && type->kind() == TypeKind::CLASS) {
    return reinterpret_cast<IClass*>(type);
  }
  return nullptr;
}

bool RTTIRegistry::registerType(std::unique_ptr<IType>&& type) {
  std::unique_lock lock(m_mutex);
  if (m_types.contains(type->name())) {
    return false;
  }

  return m_types.insert({type->name(), std::move(type)}).second;
}

bool RTTIRegistry::hasType(const IString& name) noexcept {
  std::shared_lock lock(m_mutex);
  return m_types.contains(name);
}
}  // namespace core::rtti
