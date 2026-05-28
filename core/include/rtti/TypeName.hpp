#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <vector>

#include "CString.hpp"
#include "rtti/TypeKind.hpp"

namespace core::rtti {
/**
 * @brief Returns the string prefix associated with a given @c TypeKind for use in type name construction.
 *
 * The base template returns an empty @c CString; specializations provide the appropriate prefix
 * for each kind (e.g., @c "array:" for @code TypeKind::ARRAY@endcode).
 *
 * @return The prefix @c CString for the given @c TypeKind, or an empty @c CString if none is defined.
 */
template <TypeKind>
constexpr auto GetTypePrefix() {
  return CString("");
}

/**
 * @brief Returns the prefix string for array type names.
 *
 * @return The string @c "array:" used as a prefix for array type names.
 */
template <>
constexpr auto GetTypePrefix<TypeKind::ARRAY>() {
  return CString("array:");
}

/**
 * @brief Returns the prefix string for reference type names.
 *
 * @return The string @c "ref:" used as a prefix for reference type names.
 */
template <>
constexpr auto GetTypePrefix<TypeKind::REF>() {
  return CString("ref:");
}

/**
 * @brief Returns the prefix string for weak reference type names.
 *
 * @return The string @c "wref:" used as a prefix for weak reference type names.
 */
template <>
constexpr auto GetTypePrefix<TypeKind::WEAK_REF>() {
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
 * @brief Variable template that is @c true when @c T exposes a static @c NAME member convertible to a
 * @c CString or character array.
 *
 * It allows a type to declare its own RTTI name inline rather than through a @c TypeName specialization.
 *
 * @tparam T The type to check for a static @c NAME member.
 */
template <typename T>
constexpr bool has_type_name_member_v = requires {
  { T::NAME } -> CStringConvertible;
};

/**
 * @brief Variable template that is @c true when a @c TypeName<T> specialization exists and defines a @c value member
 * convertible to a @c CString or character array.
 *
 * @tparam T The type to check for a @c TypeName mapping.
 */
template <typename T>
constexpr bool has_type_name_mapping_v = requires {
  { TypeName<T>::value } -> CStringConvertible;
};

/**
 * @brief Concept that is satisfied when a type name can be resolved for @c T, either via a @c T::NAME member
 * or a @c TypeName<T> specialization.
 *
 * @tparam T The type to check for an available type name.
 */
template <typename T>
concept NamedType = has_type_name_member_v<T> || has_type_name_mapping_v<T>;

/**
 * @brief Retrieves the type name for @c T as a compile-time @code CString@endcode.
 *
 * The name is resolved by checking @c T::NAME first (via @code has_type_name_member_v@endcode), then falling back to
 * a @c TypeName<T> specialization (via @code has_type_name_mapping_v@endcode). The @c NamedType constraint
 * guarantees that at least one of the two paths is available.
 *
 * @tparam T The type whose name is to be retrieved. It must satisfy @code NamedType@endcode.
 * @return The type name as a @code CString@endcode.
 */
template <NamedType T>
constexpr auto GetTypeName() noexcept {
  if constexpr (has_type_name_member_v<T>) {
    return CString(T::NAME);
  } else if constexpr (has_type_name_mapping_v<T>) {
    return CString(TypeName<T>::value);
  }
}

/**
 * @brief Returns the canonical prefixed type name for element type @c T at compile time.
 *
 * The result is the element type's name prefixed with the string corresponding to @c K (e.g., @c "array:int"
 * for @c TypeKind::ARRAY and @code int@endcode). @c T::NAME is preferred when present; otherwise the @c TypeName<T>
 * specialization is used. The @c NamedType constraint guarantees that one of the two is available.
 *
 * @tparam K The @c TypeKind whose prefix to prepend.
 * @tparam T The element type for which to produce the prefixed name. It must satisfy @code NamedType@endcode.
 * @return A @c CString containing the prefixed type name.
 */
template <TypeKind K, NamedType T>
[[nodiscard]] constexpr auto GetPrefixedTypeName() {
  constexpr auto prefix = GetTypePrefix<K>();

  if constexpr (has_type_name_member_v<T>) {
    return prefix + CString(T::NAME);
  } else if constexpr (has_type_name_mapping_v<T>) {
    return prefix + CString(TypeName<T>::value);
  }
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
  return GetTypePrefix<K>().append(name);
}

/**
 * @brief Concept that is satisfied when @c C is a @c std::vector specialization.
 *
 * It checks that @c C exposes a @c value_type member and that @c C is exactly
 * @c std::vector of that element type.
 *
 * @tparam C The type to check.
 */
template <typename C>
concept StdVector = requires { typename C::value_type; } && std::same_as<C, std::vector<typename C::value_type>>;

/**
 * @brief Concept that is satisfied when @c C is a @c std::vector and its element type satisfies
 * @code NamedType@endcode.
 *
 * @tparam C The type to check.
 */
template <typename C>
concept NamedVectorType = StdVector<C> && NamedType<typename C::value_type>;

/**
 * @brief Returns the canonical type name for a @c std::vector whose element type satisfies @code NamedType@endcode.
 *
 * The returned name uses the @c TypeKind::ARRAY prefix followed by the element type name
 * (e.g., @c "array:double" for @code std::vector<double>@endcode).
 *
 * @tparam C The container type to name. It must satisfy @code NamedVectorType@endcode.
 * @return A @c CString containing the prefixed array type name.
 */
template <NamedVectorType C>
constexpr auto GetTypeName() {
  return GetPrefixedTypeName<TypeKind::ARRAY, typename C::value_type>();
}
}  // namespace core::rtti

/**
 * @brief Macro to register a @c TypeKind prefix for a type name mapping specialization.
 *
 * @param _kind The @c TypeKind for which to register the prefix (e.g., @c TypeKind::ARRAY).
 * @param _prefix The string prefix to associate with the given @c TypeKind (e.g., @c "array:").
 */
#define REGISTER_TYPE_PREFIX(_kind, _prefix)          \
  template <>                                         \
  constexpr auto core::rtti::GetTypePrefix<_kind>() { \
    return core::CString(_prefix);                    \
  }

/**
 * @brief Macro to register a type name mapping for a type that cannot declare a static @c NAME member.
 *
 * @param _type The C++ type for which to register the name mapping (e.g., @c std::vector<int>).
 * @param _name The string name to associate with the given type (e.g., @c "array:int").
 */
#define REGISTER_TYPE_NAME(_type, _name)         \
  template <>                                    \
  struct core::rtti::TypeName<_type> {           \
    static constexpr core::CString value{_name}; \
  }
