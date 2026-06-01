#pragma once

#include "Export.hpp"
#include "NamePool.hpp"
#include "RTTIName.hpp"
#include "RTTITType.hpp"
#include "RTTIType.hpp"

namespace core::rtti {

/**
 * @brief Polymorphic interface for fundamental type descriptors in the RTTI system.
 *
 * Concrete fundamental-type descriptors derive from this interface, while shared implementation lives in @code
 * TTypeImpl@endcode.
 */
class JAYBIRD_API RTTIFundamentalType : public RTTIType {
 public:
  /**
   * @brief Constructs an @c IFundamentalType with the given metadata.
   *
   * @param name The interned string name of the fundamental type.
   * @param size The size of the fundamental type in bytes.
   * @param alignment The alignment requirement of the fundamental type in bytes.
   */
  explicit RTTIFundamentalType(const Name& name, std::size_t size, std::size_t alignment) noexcept;

  /**
   * @brief Virtual destructor for the @c IFundamentalType class.
   */
  ~RTTIFundamentalType() override;
};

/**
 * @brief A template class representing a concrete fundamental type in the RTTI system.
 *
 * This class is intended for types that are considered fundamental and fulfill the following requirements:
 * - It must be a fundamental type, as determined by @code std::is_fundamental_v<T>@endcode.
 * - It must not be void, as determined by @code !std::is_void_v<T>@endcode.
 * - It must not be a pointer type, as determined by @code !std::is_pointer_v<T>@endcode.
 *
 * @tparam T The underlying type that this @c TFundamentalType represents.
 */
template <typename T>
  requires(std::is_fundamental_v<T> && !std::is_void_v<T> && !std::is_pointer_v<T>)
class RTTIFundamentalTType : public RTTITType<T, RTTIFundamentalType> {
 public:
  /**
   * @brief Defines a type alias for the underlying type @code T@endcode.
   */
  using Type = T;

  /**
   * @brief Constructs a @c TFundamentalType for the specified type @code T@endcode.
   *
   * The constructor initializes the base @c IFundamentalType with the type name obtained from the @c TypeName mapping
   * for type @c T and sets the type kind to @code RTTITypeKind::FUNDAMENTAL@endcode.
   */
  RTTIFundamentalTType() noexcept : RTTITType<T, RTTIFundamentalType>(NamePool::get().addName(GetRTTIName<T>())) {}
};
}  // namespace core::rtti
