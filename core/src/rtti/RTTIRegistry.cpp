#include "rtti/RTTIRegistry.hpp"

#include "rtti/RTTIClassType.hpp"
#include "rtti/RTTIGlobalFunction.hpp"
#include "rtti/RTTIType.hpp"

namespace core::rtti {
namespace {
bool unregisterTypeRecursive(std::unordered_map<Name, std::unique_ptr<RTTIType>>& types, const Name& name) {
  const auto it = types.find(name);

  if (it == types.end()) {
    return false;
  }

  const auto kind = it->second->kind();

  if (kind != RTTITypeKind::ARRAY) {
    unregisterTypeRecursive(types, GetPrefixedRTTIName<RTTITypeKind::ARRAY>(name));
  }

  if (kind != RTTITypeKind::REF) {
    unregisterTypeRecursive(types, GetPrefixedRTTIName<RTTITypeKind::REF>(name));
  }

  if (kind != RTTITypeKind::WEAK_REF) {
    unregisterTypeRecursive(types, GetPrefixedRTTIName<RTTITypeKind::WEAK_REF>(name));
  }

  if (kind != RTTITypeKind::POINTER) {
    unregisterTypeRecursive(types, GetPrefixedRTTIName<RTTITypeKind::POINTER>(name));
  }

  types.erase(it);
  return true;
}
}  // namespace

RTTIRegistry::~RTTIRegistry() = default;

bool RTTIRegistry::hasType(const Name& name) const noexcept {
  std::shared_lock lock(m_typesMutex);
  return m_types.contains(name);
}

RTTIType* RTTIRegistry::getType(const Name& name) const {
  std::shared_lock lock(m_typesMutex);

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

#ifdef TESTING_ENABLED
bool RTTIRegistry::unregisterType(const Name& name) {
  std::unique_lock lock(m_typesMutex);
  return unregisterTypeRecursive(m_types, name);
}
#endif

bool RTTIRegistry::hasFunction(const Name& name) const noexcept {
  std::shared_lock lock(m_functionsMutex);
  return m_functions.contains(name);
}

RTTIGlobalFunction* RTTIRegistry::getFunction(const Name& name) const noexcept {
  std::shared_lock lock(m_functionsMutex);

  if (m_functions.contains(name)) {
    return m_functions.at(name).get();
  }

  return nullptr;
}

RTTIGlobalFunction* RTTIRegistry::registerFunction(std::unique_ptr<RTTIGlobalFunction>&& function) noexcept {
  std::unique_lock lock(m_functionsMutex);
  if (auto [it, success] = m_functions.emplace(function->name(), std::move(function)); success) {
    return it->second.get();
  }
  return nullptr;
}

#ifdef TESTING_ENABLED
bool RTTIRegistry::unregisterFunction(const Name& name) noexcept {
  std::unique_lock lock(m_functionsMutex);

  const auto it = m_functions.find(name);

  if (it == m_functions.end()) {
    return false;
  }

  m_functions.erase(it);
  return true;
}
#endif
}  // namespace core::rtti
