#include "rtti/types/IntType.hpp"

namespace core::rtti {
IntType::IntType(const IString& name, const TypeKind kind) : TType(name, kind) {}

bool IntType::assign(const IType* srcType, void* dst, void* src) const {
  if (*srcType != *this) {
    return false;
  }
  *static_cast<int*>(dst) = *static_cast<const int*>(src);
  return true;
}

bool IntType::construct(void* memory) const noexcept {
  *static_cast<int*>(memory) = 0;
  return true;
}

void IntType::destruct(void* memory) const noexcept { *static_cast<int*>(memory) = 0; }

bool IntType::assign(int* destination, const int* source) const {
  *destination = *source;
  return true;
}
}  // namespace core::rtti
