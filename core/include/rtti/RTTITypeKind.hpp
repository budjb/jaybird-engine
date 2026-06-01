#pragma once

namespace core::rtti {

/**
 * @brief Classification of types in the RTTI system.
 *
 * This enumeration describes the runtime descriptor category for each registered @code RTTIType@endcode.
 */
enum class RTTITypeKind : std::size_t {
  /**
   * @brief This value identifies the interned string @code Name@endcode descriptor category.
   */
  NAME,

  /**
   * @brief This value identifies class or struct descriptors.
   */
  CLASS,

  /**
   * @brief This value identifies dynamic array descriptors such as @code core::Vector<T>@endcode.
   */
  ARRAY,

  /**
   * @brief This value identifies string descriptors.
   */
  STRING,

  /**
   * @brief This value identifies primitive descriptors such as @code int@endcode, @code float@endcode, and @code
   * bool@endcode.
   */
  FUNDAMENTAL,

  /**
   * @brief This value identifies reference-counted smart pointer descriptors such as
   * @code std::shared_ptr<T>@endcode.
   */
  REF,

  /**
   * @brief This value identifies non-owning weak-reference descriptors such as @code std::weak_ptr<T>@endcode.
   */
  WEAK_REF,
};

}  // namespace core::rtti
