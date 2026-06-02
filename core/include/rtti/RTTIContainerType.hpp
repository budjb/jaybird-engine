#pragma once

#include "Export.hpp"
#include "rtti/RTTIType.hpp"

namespace core::rtti {
/**
 * @brief Concept that constrains inner type descriptors used with container types such as
 * @code TypedRTTIArrayType@endcode.
 *
 * It is satisfied when @c T1 derives from @c RTTIType and defines a nested @c Type alias that matches
 * @c T2 exactly. This ensures at compile time that the inner descriptor is compatible with the
 * container's element type, preventing mismatched descriptors from being passed to constructors.
 *
 * @tparam T1 The candidate inner type descriptor to validate.
 * @tparam T2 The expected element type that @c T1::Type must match.
 */
template <typename T1, typename T2>
constexpr bool is_same_element_v =
    std::derived_from<T1, RTTIType> && requires { typename T1::Type; } && std::same_as<typename T1::Type, T2>;

/**
 * @brief Interface for type descriptors representing containers that encapsulate other types (e.g., arrays, maps, smart
 * pointers, etc.).
 */
class JAYBIRD_API RTTIContainerType : public RTTIType {
 public:
  /**
   * @brief Constructs an @c RTTIContainerType with the given name, size, alignment, inner type descriptor, and kind.
   *
   * @param name The interned string name of the container type.
   * @param size The size of the container type in bytes.
   * @param alignment The alignment requirement of the container type in bytes.
   * @param inner A pointer to the RTTIType descriptor for the type contained within the container.
   * @param kind The @c RTTITypeKind value classifying this container (e.g., @c RTTITypeKind::ARRAY or
   * @c RTTITypeKind::REF).
   */
  explicit RTTIContainerType(const Name& name, std::size_t size, std::size_t alignment, const RTTIType* inner,
                             RTTITypeKind kind) noexcept;

  /**
   * @brief Returns a pointer to the inner type descriptor.
   *
   * The returned pointer is guaranteed to be valid for the lifetime of this container descriptor and should not be
   * modified by the caller.
   *
   * @return A pointer to the RTTIType descriptor for the type contained within the container.
   */
  [[nodiscard]] const RTTIType* inner() const noexcept;

 private:
  /**
   * @brief This pointer refers to the @c RTTIType descriptor for the type contained within the container.
   */
  const RTTIType* m_inner;
};
}  // namespace core::rtti
