#include "rtti/TypeSystem.hpp"

#include <algorithm>
#include <cstdint>

#include "rtti/ClassType.hpp"
#include "rtti/FundamentalType.hpp"
#include "rtti/INameType.hpp"
#include "types/Color.hpp"
#include "types/EulerAngles.hpp"
#include "types/Quaternion.hpp"

REGISTER_TYPE_NAME(int8_t, "int8");
REGISTER_TYPE_NAME(int32_t, "int32");
REGISTER_TYPE_NAME(int64_t, "int64");
REGISTER_TYPE_NAME(uint8_t, "uint8");
REGISTER_TYPE_NAME(uint32_t, "uint32");
REGISTER_TYPE_NAME(uint64_t, "uint64");
REGISTER_TYPE_NAME(float, "float");
REGISTER_TYPE_NAME(double, "double");
REGISTER_TYPE_NAME(bool, "bool");

namespace core::rtti {
TypeSystem& TypeSystem::get() noexcept {
  static TypeSystem typeSystem{};
  return typeSystem;
}

bool TypeSystem::initialize() {
  if (m_initialized) {
    return false;
  }

  std::ranges::for_each(m_declareFunctions, [](const auto& func) { func(); });
  std::ranges::for_each(m_defineFunctions, [](const auto& func) { func(); });

  m_initialized = true;
  return true;
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
  addDeclareCallback([this] { registerBuiltInTypes(); });
}

void TypeSystem::registerBuiltInTypes() noexcept {
  m_registry.registerType(std::make_unique<TFundamentalType<int8_t>>());
  m_registry.registerType(std::make_unique<TFundamentalType<int32_t>>());
  m_registry.registerType(std::make_unique<TFundamentalType<int64_t>>());
  m_registry.registerType(std::make_unique<TFundamentalType<uint8_t>>());
  m_registry.registerType(std::make_unique<TFundamentalType<uint32_t>>());
  m_registry.registerType(std::make_unique<TFundamentalType<uint64_t>>());
  m_registry.registerType(std::make_unique<TFundamentalType<float>>());
  m_registry.registerType(std::make_unique<TFundamentalType<double>>());
  m_registry.registerType(std::make_unique<TFundamentalType<bool>>());

  m_registry.registerType(std::make_unique<INameType>());
  m_registry.registerType(std::make_unique<TClassType<Quaternion>>());
  m_registry.registerType(std::make_unique<TClassType<EulerAngles>>());
  m_registry.registerType(std::make_unique<TClassType<Color>>());
}
}  // namespace core::rtti
