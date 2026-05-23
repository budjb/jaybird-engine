#pragma once
#include "IType.hpp"

namespace core::rtti {
class IClassType : public IType {
 public:
  explicit IClassType(const IName& name, std::size_t size, std::size_t alignment) noexcept;
};
}  // namespace core::rtti
