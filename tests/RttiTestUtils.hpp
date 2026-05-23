#pragma once

#include "rtti/types/IntType.hpp"

namespace tests::rtti {

inline core::rtti::IntType& intElementType() {
  static core::rtti::IntType type(core::IName("int"));
  return type;
}

inline core::rtti::IntArrayType& intArrayType() {
  static core::rtti::IntArrayType type(core::IName("int[]"), &intElementType());
  return type;
}

inline core::rtti::IType* asType(core::rtti::IntArrayType& value) {
  return &value;
}

inline core::rtti::IArrayType* asArrayType(core::rtti::IntArrayType& value) {
  return &value;
}

}  // namespace tests::rtti
