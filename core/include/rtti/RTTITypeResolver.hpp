#pragma once

#include "rtti/RTTIType.hpp"

namespace core::rtti {
/**
 * @brief Helper struct that resolves a reflected type descriptor for a C++ type.
 *
 * This helper caches the registry lookup result after the first request so repeated
 * uses do not repeatedly query the type registry.
 *
 * @tparam T This type is the C++ type whose reflected descriptor is requested.
 */
template <typename T>
struct TypeResolver {
  /**
   * @brief Returns the reflected type descriptor for @c T.
   *
   * @return This function returns the reflected type descriptor for @c T, or @code nullptr@endcode when no descriptor
   * has been registered.
   */
  static RTTIType* get();

  /**
   * @brief Returns the reflected class descriptor for @c T when the resolved type is a class.
   *
   * @return This function returns the reflected class descriptor for @c T, or @code nullptr@endcode when the resolved
   * type is absent or not a class.
   */
  static RTTIClassType* getClass();
};
}  // namespace core::rtti

#include "RTTITypeResolver.inl"
