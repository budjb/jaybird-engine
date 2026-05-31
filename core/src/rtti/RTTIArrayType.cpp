#include "rtti/RTTIArrayType.hpp"

#include "rtti/RTTIIterator.hpp"

namespace core::rtti {
RTTIArrayType::RTTIArrayType(const IName& name, const std::size_t size, const std::size_t alignment,
                             const RTTIType* inner) noexcept
    : RTTIContainerType(name, size, alignment, inner, RTTITypeKind::ARRAY) {}

ReverseIterator<> RTTIArrayType::rbegin(void* array) noexcept {
  return ReverseIterator(end(array));
}

ReverseIterator<> RTTIArrayType::rend(void* array) noexcept {
  return ReverseIterator(begin(array));
}
}  // namespace core::rtti
