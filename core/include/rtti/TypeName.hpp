#pragma once

#include <string>
#include <string_view>

#include "CString.hpp"
#include "rtti/TypeKind.hpp"

namespace core::rtti {
/**
 * @brief Returns the string prefix associated with a given @c TypeKind for use in type name construction.
 *
 * The base template returns an empty @c CString; specializations provide the appropriate prefix
 * for each kind (e.g., @c "array:" for @c TypeKind::ARRAY).
 *
 * @return The prefix @c CString for the given @c TypeKind, or an empty @c CString if none is defined.
 */
template <TypeKind>
constexpr auto TypePrefix() {
  return CString("");
}

/**
 * @brief Returns the prefix string for array type names.
 *
 * @return The string @c "array:" used as a prefix for array type names.
 */
template <>
constexpr auto TypePrefix<TypeKind::ARRAY>() {
  return CString("array:");
}

/**
 * @brief Returns the prefix string for reference type names.
 *
 * @return The string @c "ref:" used as a prefix for reference type names.
 */
template <>
constexpr auto TypePrefix<TypeKind::REF>() {
  return CString("ref:");
}

/**
 * @brief Returns the prefix string for weak reference type names.
 *
 * @return The string @c "wref:" used as a prefix for weak reference type names.
 */
template <>
constexpr auto TypePrefix<TypeKind::WEAK_REF>() {
  return CString("wref:");
}

/**
 * @brief Primary template for mapping a C++ type @c T to its RTTI string name.
 *
 * Specialize this struct for any type that cannot declare a @c static constexpr NAME member, providing a
 * @c static constexpr CString value member with the desired name.
 *
 * @tparam T The type for which to declare a name mapping.
 */
template <typename T>
struct TypeName;

/**
 * @brief Concept that is satisfied when @c T exposes a static @c NAME member convertible to @c CString or a
 * character array. It allows a type to declare its own RTTI name inline rather than through a @c TypeName
 * specialization.
 *
 * @tparam T The type to check for a static @c NAME member.
 */
template <typename T>
concept HasTypeNameMember = requires {
  { T::NAME } -> is_cstring_convertible_v;
};

/**
 * @brief Concept that is satisfied when a @c TypeName<T> specialization exists and defines a @c value member
 * convertible to a @c CString or character array.
 *
 * @tparam T The type to check for a @c TypeName mapping.
 */
template <typename T>
concept HasTypeNameMapping = requires {
  { TypeName<T>::value } -> is_cstring_convertible_v;
};

/**
 * @brief Concept that is satisfied when a type name can be resolved for @c T, either via a @c T::NAME member
 * or a @c TypeName<T> specialization.
 *
 * It combines @c HasTypeNameMember and @c HasTypeNameMapping so that callers can accept either approach.
 *
 * @tparam T The type to check for an available type name.
 */
template <typename T>
concept HasTypeName = HasTypeNameMember<T> || HasTypeNameMapping<T>;

/**
 * @brief Retrieves the type name for @c T as a compile-time @c CString.
 *
 * The name is resolved by checking @c T::NAME first (via @c HasTypeNameMember), then falling back to a
 * @c TypeName<T> specialization (via @c HasTypeNameMapping).
 *
 * @tparam T The type whose name is to be retrieved. It must satisfy @code HasTypeName@endcode.
 * @return The type name as a @code CString@endcode.
 */
template <typename T>
  requires HasTypeName<T>
constexpr auto GetTypeName() noexcept {
  if constexpr (HasTypeNameMember<T>) {
    return CString(T::NAME);
  }

  static_assert(HasTypeNameMapping<T>);

  return CString(TypeName<T>::value);
}

/**
 * @brief Returns the canonical prefixed type name for element type @c T at compile time.
 *
 * The result is the element type's name prefixed with the string corresponding to @c K (e.g., @c "array:int"
 * for @c TypeKind::ARRAY and @c int). The element type must satisfy @code HasTypeName@endcode.
 *
 * @tparam K The @c TypeKind whose prefix to prepend.
 * @tparam T The element type for which to produce the prefixed name. It must satisfy @code HasTypeName@endcode.
 * @return A @c CString containing the prefixed type name.
 */
template <TypeKind K, typename T>
  requires HasTypeName<T>
[[nodiscard]] constexpr auto GetPrefixedTypeName() {
  constexpr auto prefix = TypePrefix<K>();

  if constexpr (HasTypeNameMember<T>) {
    return prefix + CString(T::NAME);
  }

  static_assert(HasTypeNameMapping<T>);

  return prefix + CString(TypeName<T>::value);
}

/**
 * @brief Returns the canonical type name for a runtime string, prefixed with the string associated with @c K.
 *
 * @tparam K The @c TypeKind whose prefix to prepend.
 * @param name The base type name to prefix.
 * @return A @c std::string containing the concatenated prefix and base name.
 */
template <TypeKind K>
[[nodiscard]] std::string GetPrefixedTypeName(const std::string_view name) {
  return TypePrefix<K>().append(name);
}
}  // namespace core::rtti
