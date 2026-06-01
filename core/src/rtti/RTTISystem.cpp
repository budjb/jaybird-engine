#include "rtti/RTTISystem.hpp"

#include <algorithm>
#include <cstdint>

#include "rtti/RTTIClassType.hpp"
#include "rtti/RTTIFundamentalType.hpp"
#include "rtti/RTTINameType.hpp"
#include "rtti/RTTIStringType.hpp"
#include "types/Color.hpp"
#include "types/EulerAngles.hpp"
#include "types/Quaternion.hpp"

namespace core::rtti {
RTTISystem& RTTISystem::get() noexcept {
  static RTTISystem typeSystem{};
  return typeSystem;
}

bool RTTISystem::initialize() {
  if (m_initialized) {
    return false;
  }

  std::ranges::for_each(m_declareFunctions, [](const auto& func) { func(); });
  std::ranges::for_each(m_defineFunctions, [](const auto& func) { func(); });

  m_initialized = true;
  return true;
}

RTTIRegistry& RTTISystem::registry() noexcept {
  return m_registry;
}

void RTTISystem::addDeclareCallback(const CallbackFunction& function) {
  m_declareFunctions.push_back(function);
}

void RTTISystem::addDefineCallback(const CallbackFunction& function) {
  m_defineFunctions.push_back(function);
}

void RTTISystem::addCallbacks(const CallbackFunction& declare, const CallbackFunction& define) {
  m_declareFunctions.push_back(declare);
  m_defineFunctions.push_back(define);
}

RTTISystem::RTTISystem() noexcept {
  addDeclareCallback([this] { registerBuiltInTypes(); });
}

void RTTISystem::registerBuiltInTypes() noexcept {
  m_registry.registerType(std::make_unique<TypedRTTIFundamentalType<int8_t>>());
  m_registry.registerType(std::make_unique<TypedRTTIFundamentalType<int32_t>>());
  m_registry.registerType(std::make_unique<TypedRTTIFundamentalType<int64_t>>());
  m_registry.registerType(std::make_unique<TypedRTTIFundamentalType<uint8_t>>());
  m_registry.registerType(std::make_unique<TypedRTTIFundamentalType<uint32_t>>());
  m_registry.registerType(std::make_unique<TypedRTTIFundamentalType<uint64_t>>());
  m_registry.registerType(std::make_unique<TypedRTTIFundamentalType<float>>());
  m_registry.registerType(std::make_unique<TypedRTTIFundamentalType<double>>());
  m_registry.registerType(std::make_unique<TypedRTTIFundamentalType<bool>>());

  m_registry.registerType(std::make_unique<RTTINameType>());
  m_registry.registerType(std::make_unique<RTTIStringType>());
  m_registry.registerType(std::make_unique<TypedRTTIClassType<Quaternion>>());
  m_registry.registerType(std::make_unique<TypedRTTIClassType<EulerAngles>>());
  m_registry.registerType(std::make_unique<TypedRTTIClassType<Color>>());
}
}  // namespace core::rtti
