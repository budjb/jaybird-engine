#pragma once

#include <cstddef>

namespace core::rtti {

/**
 * @brief Classification of types in the RTTI system.
 *
 * This enumeration describes the runtime descriptor category for each registered @code RTTIType@endcode.
 */
enum class RTTITypeKind : std::size_t {
  /**
   * @brief Identifies the @c Name class.
   */
  NAME,

  /**
   * @brief Identifies trivial classes.
   */
  SIMPLE,

  /**
   * @brief Identifies non-trivial classes.
   */
  CLASS,

  /**
   * @brief Identifies the @c core::Vector<T> class.
   */
  ARRAY,

  /**
   * @brief Identifies the @c std::string class.
   */
  STRING,

  /**
   * @brief Identifies primitive types (e.g, @c int32_t, @c float, @c bool, etc.).
   */
  FUNDAMENTAL,

  /**
   * @brief Identifies reference-counted smart pointers (i.e. @c std::shared_ptr<T>).
   */
  REF,

  /**
   * @brief Identifies non-owning, weak-reference smart pointers (i.e. @c std::weak_ptr<T>).
   */
  WEAK_REF,

  /**
   * @brief Identifies raw pointer types (i.e. @c T*).
   */
  POINTER,
};

}  // namespace core::rtti
