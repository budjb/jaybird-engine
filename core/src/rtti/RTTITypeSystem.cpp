#include "rtti/RTTITypeSystem.hpp"

#include <algorithm>
#include <cstdint>

#include "rtti/RTTIClassType.hpp"
#include "rtti/RTTIFundamentalType.hpp"
#include "rtti/RTTIINameType.hpp"
#include "types/Color.hpp"
#include "types/EulerAngles.hpp"
#include "types/Quaternion.hpp"

namespace core::rtti {
RTTITypeSystem& RTTITypeSystem::get() noexcept {
  static RTTITypeSystem typeSystem{};
  return typeSystem;
}

bool RTTITypeSystem::initialize() {
  if (m_initialized) {
    return false;
  }

  std::ranges::for_each(m_declareFunctions, [](const auto& func) { func(); });
  std::ranges::for_each(m_defineFunctions, [](const auto& func) { func(); });

  m_initialized = true;
  return true;
}

RTTITypeRegistry& RTTITypeSystem::registry() noexcept {
  return m_registry;
}

void RTTITypeSystem::addDeclareCallback(const CallbackFunction& function) {
  m_declareFunctions.push_back(function);
}

void RTTITypeSystem::addDefineCallback(const CallbackFunction& function) {
  m_defineFunctions.push_back(function);
}

void RTTITypeSystem::addCallbacks(const CallbackFunction& declare, const CallbackFunction& define) {
  m_declareFunctions.push_back(declare);
  m_defineFunctions.push_back(define);
}

RTTITypeSystem::RTTITypeSystem() noexcept {
  addDeclareCallback([this] { registerBuiltInTypes(); });
}

void RTTITypeSystem::registerBuiltInTypes() noexcept {
  m_registry.registerType(std::make_unique<RTTIFundamentalTType<int8_t>>());
  m_registry.registerType(std::make_unique<RTTIFundamentalTType<int32_t>>());
  m_registry.registerType(std::make_unique<RTTIFundamentalTType<int64_t>>());
  m_registry.registerType(std::make_unique<RTTIFundamentalTType<uint8_t>>());
  m_registry.registerType(std::make_unique<RTTIFundamentalTType<uint32_t>>());
  m_registry.registerType(std::make_unique<RTTIFundamentalTType<uint64_t>>());
  m_registry.registerType(std::make_unique<RTTIFundamentalTType<float>>());
  m_registry.registerType(std::make_unique<RTTIFundamentalTType<double>>());
  m_registry.registerType(std::make_unique<RTTIFundamentalTType<bool>>());

  m_registry.registerType(std::make_unique<RTTIINameType>());
  m_registry.registerType(std::make_unique<RTTIClassTType<Quaternion>>());
  m_registry.registerType(std::make_unique<RTTIClassTType<EulerAngles>>());
  m_registry.registerType(std::make_unique<RTTIClassTType<Color>>());
}
}  // namespace core::rtti
