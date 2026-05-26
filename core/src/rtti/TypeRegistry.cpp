#include "rtti/TypeRegistry.hpp"

#include <mutex>

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

IType* TypeRegistry::registerType(std::unique_ptr<IType>&& type) {
  std::unique_lock lock(m_mutex);

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
