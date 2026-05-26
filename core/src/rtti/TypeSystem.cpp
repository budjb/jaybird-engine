#include "../../include/rtti/TypeSystem.hpp"

namespace core::rtti {
TypeSystem& TypeSystem::get() noexcept {
  static TypeSystem typeSystem{};
  return typeSystem;
}

bool TypeSystem::initialize() {
  if (m_initialized) {
    return false;
  }

  // TODO: actually initialize

  m_initialized = true;
  return m_initialized;
}

TypeRegistry& TypeSystem::registry() noexcept {
  return m_registry;
}

void TypeSystem::addDeclareCallback(const CallbackFunction& function) {
  m_declareFunctions.push_back(function);
}

void TypeSystem::addDefineCallback(const CallbackFunction& function) {
  m_defineFunctions.push_back(function);
}

void TypeSystem::addCallbacks(const CallbackFunction& declare, const CallbackFunction& define) {
  m_declareFunctions.push_back(declare);
  m_defineFunctions.push_back(define);
}

TypeSystem::TypeSystem() noexcept {
  // TODO: registrations of built-ins!
}
}  // namespace core::rtti
