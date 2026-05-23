#pragma once

#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "IName.hpp"
#include "RTTI.hpp"

namespace core::rtti {
class RTTIRegistry {
 public:
  static RTTIRegistry* get();

  IType* getType(const IName& name);

  IClassType* getClass(const IName& name);

  bool registerType(std::unique_ptr<IType>&& type);
  bool hasType(const IName& name) noexcept;

 private:
  RTTIRegistry() = default;

  std::shared_mutex m_mutex;
  std::unordered_map<IName, std::unique_ptr<IType>> m_types;
};
}  // namespace core::rtti
