#pragma once
#include "JaybirdCoreExport.hpp"
#include "rtti/RTTI.hpp"

namespace core::rtti {
class JAYBIRD_EXPORT IntType : public TType<int> {
 public:
  explicit IntType(const IName& name);
};

class JAYBIRD_EXPORT IntArrayType : public TArrayType<int> {
 public:
  explicit IntArrayType(const IName& name, const TType<int>* inner);
};
}  // namespace core::rtti
