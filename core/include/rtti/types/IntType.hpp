#pragma once
#include "rtti/RTTI.hpp"

namespace core::rtti {
class IntType : TType<int> {
 public:
  IntType(const IString& name, TypeKind kind);

  ~IntType() override = default;

  bool assign(const IType* srcType, void* dst, void* src) const override;

  bool construct(void* memory) const noexcept override;

  void destruct(void* memory) const noexcept override;

  bool assign(int* destination, const int* source) const override;
};
}  // namespace core::rtti
