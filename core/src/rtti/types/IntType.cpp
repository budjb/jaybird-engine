#include "rtti/types/IntType.hpp"

namespace core::rtti {
IntType::IntType(const IString& name) : TType(name, TypeKind::SIMPLE) {}
IntArrayType::IntArrayType(const IString& name, const TType<int>* inner) : TArrayType(name, inner) {}
}  // namespace core::rtti
