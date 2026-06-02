#pragma once

#include <type_traits>

#include "NamePool.hpp"
#include "RTTIContainerType.hpp"
#include "RTTIName.hpp"

namespace core::rtti {
/**
 * @brief Interface for raw pointer type descriptors in the RTTI system.
 *
 * This class describes C++ raw pointer types represented as @c T*. It stores metadata for the pointer
 * object itself and a pointer to the inner element descriptor returned by @code inner()@endcode.
 */
class JAYBIRD_API RTTIPointerType : public RTTIContainerType {
 public:
  /**
   * @brief Constructs an @c RTTIPointerType with the given metadata and inner descriptor.
   *
   * @param name This parameter provides the interned type name for the pointer descriptor.
   * @param size This parameter provides the size of the raw pointer type in bytes.
   * @param alignment This parameter provides the alignment requirement of the raw pointer type in bytes.
   * @param inner This parameter provides the descriptor for the pointee type.
   */
  RTTIPointerType(const Name& name, std::size_t size, std::size_t alignment, const RTTIType* inner) noexcept;

  /**
   * @brief Destroys the @code RTTIPointerType@endcode descriptor.
   */
  ~RTTIPointerType() noexcept override;
};

/**
 * @brief Concrete RTTI descriptor for raw pointer types.
 *
 * This template models pointer types as container descriptors whose inner type is the pointee.
 * The generated descriptor name uses the pointer prefix from @c RTTITypeKind::POINTER, producing
 * names such as @c "ptr:MyType".
 *
 * @tparam T This parameter specifies the raw pointer type represented by this descriptor.
 */
template <typename T>
  requires std::is_pointer_v<T>
class TypedRTTIPointerType : public TypedRTTIType<T, RTTIPointerType> {
 public:
  /**
   * @brief This alias names the represented C++ pointer type.
   */
  using Type = T;

  /**
   * @brief Constructs a typed raw-pointer descriptor bound to an inner element descriptor.
   *
   * @tparam InnerType This parameter specifies the concrete descriptor type for the pointee type.
   * @param inner This parameter provides the descriptor of the pointee type.
   */
  template <typename InnerType>
    requires is_same_element_v<InnerType, std::remove_pointer_t<T>>
  explicit TypedRTTIPointerType(const InnerType* inner)
      : TypedRTTIType<T, RTTIPointerType>(
            NamePool::get().addName(GetPrefixedRTTIName<RTTITypeKind::POINTER, std::remove_pointer_t<T>>()),
            static_cast<const RTTIType*>(inner)) {}
};
}  // namespace core::rtti
