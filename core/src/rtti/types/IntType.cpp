#include "rtti/types/IntType.hpp"

namespace core::rtti {
IntType::IntType(const IName& name) : TType(name, TypeKind::FUNDAMENTAL) {}
IntArrayType::IntArrayType(const IName& name, const TType<int>* inner) : TArrayType(name, inner) {}
}  // namespace core::rtti
