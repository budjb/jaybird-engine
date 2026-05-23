#pragma once
#include "rtti/RTTI.hpp"

namespace core::rtti {
class IntType : public TType<int> {
 public:
  explicit IntType(const IName& name);
};

class IntArrayType : public TArrayType<int> {
 public:
  explicit IntArrayType(const IName& name, const TType<int>* inner);
};
}  // namespace core::rtti
