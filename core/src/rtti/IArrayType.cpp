#include "rtti/IArrayType.hpp"

#include <vector>

#include "rtti/Iterator.hpp"

namespace core::rtti {
IArrayType::IArrayType(const IName& name, const std::size_t size, const std::size_t alignment,
                       const IType* inner) noexcept
    : IContainerType(name, size, alignment, inner) {}

ReverseIterator<> IArrayType::rbegin(void* array) noexcept {
  return ReverseIterator(end(array));
}

ReverseIterator<> IArrayType::rend(void* array) noexcept {
  return ReverseIterator(begin(array));
}
}  // namespace core::rtti
