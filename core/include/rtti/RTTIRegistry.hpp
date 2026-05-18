#pragma once

#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "IString.hpp"
#include "RTTI.hpp"

namespace core::rtti {
class RTTIRegistry {
 public:
  static RTTIRegistry* get();

  IType* getType(const IString& name);

  IClass* getClass(const IString& name);

  bool registerType(std::unique_ptr<IType>&& type);
  bool hasType(const IString& name) noexcept;

 private:
  RTTIRegistry() = default;

  std::shared_mutex m_mutex;
  std::unordered_map<IString, std::unique_ptr<IType>> m_types;
};
}  // namespace core::rtti
