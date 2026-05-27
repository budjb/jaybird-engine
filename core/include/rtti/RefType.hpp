#pragma once

#include <memory>

#include "ContainerType.hpp"
#include "TypeName.hpp"

namespace core::rtti {

/**
 * @brief Interface for reference-counted pointer type descriptors in the RTTI system.
 *
 * This class describes types whose C++ representation is @c std::shared_ptr<T>, allowing the RTTI system
 * to reason about shared ownership semantics through a common interface.
 */
class IRefType : public IContainerType {
 public:
  /**
   * @brief Constructs an @c IRefType with the given name, size, alignment, and inner type descriptor.
   *
   * @param name The interned string name of the reference type.
   * @param size The size of the smart pointer type in bytes.
   * @param alignment The alignment requirement of the smart pointer type in bytes.
   * @param inner A pointer to the @c IType descriptor for the referenced element type.
   */
  IRefType(const IName& name, const std::size_t size, const std::size_t alignment, const IType* inner) noexcept
      : IContainerType(name, size, alignment, inner) {}

  /**
   * @brief Virtual destructor for @code IRefType@endcode.
   */
  ~IRefType() override = default;
};

/**
 * @brief Concrete RTTI descriptor for @c std::shared_ptr<T> reference types.
 *
 * It implements @c IRefType over @c std::shared_ptr<T> and derives its type name from @c GetPrefixedTypeName,
 * producing a name such as @c "ref:MyType". The inner type descriptor must satisfy
 * @c TypedInnerDescriptorFor<InnerType, T>.
 *
 * @tparam T The element type held by the @c std::shared_ptr this descriptor represents.
 */
template <typename T>
class TRefType : public TType<std::shared_ptr<T>, IRefType> {
 public:
  /**
   * @brief Type alias for the underlying @c std::shared_ptr type described by this descriptor.
   */
  using Type = std::shared_ptr<T>;

  /**
   * @brief Constructs a @c TRefType with the given inner type descriptor.
   *
   * The type name is automatically derived from @c GetPrefixedTypeName with @c TypeKind::REF,
   * producing a name such as @c "ref:MyType".
   *
   * @tparam InnerType The concrete inner type descriptor, which must satisfy
   *   @c TypedInnerDescriptorFor<InnerType, T>.
   * @param inner A pointer to the @c IType descriptor for the element type @code T@endcode.
   */
  template <typename InnerType>
    requires TypedInnerDescriptorFor<InnerType, T>
  explicit TRefType(const InnerType* inner);
};

template <typename T>
template <typename InnerType>
  requires TypedInnerDescriptorFor<InnerType, T>
TRefType<T>::TRefType(const InnerType* inner)
    : TType<std::shared_ptr<T>, IRefType>(GetPrefixedTypeName<TypeKind::REF, T>(), static_cast<const IType*>(inner)) {}
}  // namespace core::rtti
