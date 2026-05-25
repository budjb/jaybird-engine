#pragma once
#include "IType.hpp"
#include "JaybirdCoreExport.hpp"

namespace core::rtti {
class JAYBIRD_EXPORT IClassType : public IType {
 public:
  explicit IClassType(const IName& name, std::size_t size, std::size_t alignment) noexcept;
};
}  // namespace core::rtti
