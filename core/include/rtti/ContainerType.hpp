#pragma once

#include "Export.hpp"
#include "IType.hpp"

namespace core::rtti {
/**
 * @brief Concept that constrains inner type descriptors used with container types such as @code TArrayType@endcode.
 *
 * It is satisfied when @c InnerType derives from @c IType and defines a nested @c Type alias that matches
 * @c ElementType exactly. This ensures at compile time that the inner descriptor is compatible with the
 * container's element type, preventing mismatched descriptors from being passed to constructors.
 *
 * @tparam InnerType The candidate inner type descriptor to validate.
 * @tparam ElementType The expected element type that @c InnerType::Type must match.
 */
template <typename InnerType, typename ElementType>
concept TypedInnerDescriptorFor = std::derived_from<InnerType, IType> && requires { typename InnerType::Type; } &&
                                  std::same_as<typename InnerType::Type, ElementType>;

/**
 * @brief Interface for type descriptors representing containers that encapsulate other types (e.g., arrays, maps, smart
 * pointers, etc.).
 */
class JAYBIRD_API IContainerType : public IType {
 public:
  /**
   * @brief Constructs an @c IContainerType with the given name, size, alignment, inner type descriptor, and kind.
   *
   * @param name The interned string name of the container type.
   * @param size The size of the container type in bytes.
   * @param alignment The alignment requirement of the container type in bytes.
   * @param inner A pointer to the IType descriptor for the type contained within the container.
   * @param kind The @c TypeKind value classifying this container (e.g., @c TypeKind::ARRAY or @c TypeKind::REF).
   */
  explicit IContainerType(const IName& name, std::size_t size, std::size_t alignment, const IType* inner,
                          TypeKind kind) noexcept;

  /**
   * @brief Returns a pointer to the inner type descriptor.
   *
   * The returned pointer is guaranteed to be valid for the lifetime of this container descriptor and should not be
   * modified by the caller.
   *
   * @return A pointer to the IType descriptor for the type contained within the container.
   */
  [[nodiscard]] const IType* inner() const noexcept;

 private:
  /**
   * @brief A pointer to the @c IType descriptor for the type contained within the container.
   */
  const IType* m_inner;
};
}  // namespace core::rtti
