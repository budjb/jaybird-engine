#pragma once

#include "rtti/RTTIName.hpp"
#include "rtti/RTTISystem.hpp"
#include "rtti/RTTIType.hpp"

namespace core::rtti {
template <typename T>
RTTIType* TypeResolver<T>::get() {
  static bool initialized = false;
  static RTTIType* type = nullptr;

  if (!initialized) {
    type = RTTISystem::get().registry().getType(GetRTTIName<T>());
    initialized = true;
  }

  return type;
}

template <typename T>
RTTIClassType* TypeResolver<T>::getClass() {
  if (auto* type = get(); type && type->kind() == RTTITypeKind::CLASS) {
    return reinterpret_cast<RTTIClassType*>(type);
  }

  return nullptr;
}
}  // namespace core::rtti
