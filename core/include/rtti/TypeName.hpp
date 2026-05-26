#pragma once

#include <any>
#include <concepts>
#include <string>
#include <string_view>

#include "CString.hpp"
#include "rtti/TypeKind.hpp"

namespace core::rtti {
/**
 * @brief An implementation of @c ToString that simply returns the provided value.
 *
 * @tparam N The size of the CString, including the null terminator.
 * @param str The core::CString to convert to a std::string_view.
 * @return The provided CString.
 */
template <std::size_t N>
[[nodiscard]] constexpr auto ToCString(const CString<N>& str) {
  return str;
}

/**
 * @brief A helper function that converts a string literal (const char array) to a CString.
 *
 * @tparam N The size of the string literal, including the null terminator.
 * @param str The string literal to convert to a CString.
 * @return A CString that views the contents of the string literal.
 */
template <std::size_t N>
[[nodiscard]] constexpr auto ToCString(const char (&str)[N]) {
  return core::CString<N>(str);
}

/**
 * @brief Returns the string prefix associated with the given @c TypeKind for use in type name construction.
 *
 * The base template returns an empty string view; specializations provide the appropriate prefix
 * for each kind (e.g., @c "array:" for @c TypeKind::ARRAY).
 *
 * @tparam TK The @c TypeKind for which to retrieve the prefix.
 * @return The prefix string view, or an empty view if none is defined for @code TK@endcode.
 */
template <TypeKind TK>
constexpr std::string_view TypePrefix() {
  return "";
}

/**
 * @brief Returns the prefix string for array type names.
 *
 * @return The string @c "array:" used as a prefix for array type names.
 */
template <>
constexpr std::string_view TypePrefix<TypeKind::ARRAY>() {
  return "array:";
}

/**
 * @brief Returns a prefixed type name string by prepending the @c TypeKind prefix to the given string.
 *
 * If no prefix is defined for @c T, an empty string is returned.
 *
 * @tparam T The @c TypeKind whose prefix to apply.
 * @param str The base type name string to prefix.
 * @return The prefixed type name, or an empty string if no prefix is defined for @code T@endcode.
 */
template <TypeKind T>
constexpr std::string TypePrefix(const std::string_view str) {
  const auto prefix = TypePrefix<T>();

  if (prefix.empty()) {
    return "";
  }

  return std::string(prefix).append(str);
}

/**
 * @brief Returns a prefixed type name string by prepending the @c TypeKind prefix to the string resolved from an
 * @code IName@endcode.
 *
 * @tparam T The @c TypeKind whose prefix to apply.
 * @param name The @c IName whose string value to prefix.
 * @return The prefixed type name, or an empty string if no prefix is defined for @code T@endcode.
 */
template <TypeKind T>
constexpr std::string TypePrefix(const IName& name) {
  return TypePrefix<T>(std::string_view(name));
}

/**
 * @brief A template struct that serves as a mapping from types to their corresponding type names.
 *
 * This struct is intended to be specialized for specific types, where the specialization will define a static member
 * variable named value that holds the type name. Supported forms include @c CString<N>, string literals, @code const
 * char*@endcode, and other types convertible to @code std::string_view@endcode.
 */
template <typename>
struct TypeName;

/**
 * @brief A concept that checks if a type @c T has a static member variable named @code NAME@endcode.
 *
 * This accepts the common RTTI naming styles, including:
 * - `static constexpr char NAME[] = "Foo";`
 * - `inline static constexpr auto NAME = "Foo";`
 * - `inline static constexpr CString<N> NAME{"Foo"};`
 * - `inline static constexpr const char* NAME = "Foo";`
 *
 * This is useful for types that want to provide their own type name without needing to specialize the TypeName struct.
 *
 * @tparam T The type to check for having a static member variable named NAME.
 * @return True if the type T has a static member variable named NAME that is convertible to a std::string_view, or
 * false otherwise.
 */
template <typename T>
concept HasTypeNameMember = requires {
  { T::NAME } -> std::convertible_to<std::string_view>;
};

/**
 * @brief A concept that checks if a type T has a corresponding type name mapping defined in the TypeName struct.
 *
 * This concept is satisfied if there is a specialization of the TypeName struct for the type T that defines a static
 * member variable named @c value that is convertible to a @code std::string_view@endcode.
 *
 * @tparam T The type to check for having a type name.
 * @return True if there is a specialization of the TypeName struct for the type T that defines a static member variable
 * named value, or false otherwise.
 */
template <typename T>
concept HasTypeNameMapping = requires {
  { TypeName<T>::value } -> std::convertible_to<std::string_view>;
};

/**
 * @brief A concept that checks if a type T has a type name that can be retrieved either from a static member variable
 * named NAME or from a specialization of the TypeName struct.
 *
 * This concept is satisfied if either the HasTypeNameMember concept or the HasTypeNameMapping concept is satisfied for
 * the type T. This allows for flexibility in how types can provide their type names, either through a static member
 * variable or through a separate mapping struct.
 *
 * @tparam T The type to check for having a type name.
 * @return True if the type T has a type name that can be retrieved either from a static member variable named NAME or
 * from a specialization of the TypeName struct, or false otherwise.
 */
template <typename T>
concept HasTypeName = HasTypeNameMember<T> || HasTypeNameMapping<T>;

/**
 * @brief Retrieves the type name for a given type T as a std::string_view. The type name can be obtained either from a
 * static member variable named NAME or from a specialization of the TypeName struct, depending on which is available
 * for the type T.
 *
 * @tparam T The type for which to retrieve the type name. This type must satisfy the HasTypeName concept, meaning it
 * must have a type name that can be retrieved either from a static member variable named NAME or from a specialization
 * of the TypeName struct.
 * @return The type name for the type T as a std::string_view.
 */
template <typename T>
  requires HasTypeName<T>
constexpr std::string_view GetTypeName() noexcept {
  if constexpr (HasTypeNameMember<T>) {
    return T::NAME;
  }
  if constexpr (HasTypeNameMapping<T>) {
    return TypeName<T>::value;
  }
  static_assert(HasTypeName<T>);
  std::unreachable();
}

/**
 * @brief Returns the canonical array type name for an element type @c T at compile time.
 *
 * The result is the element type's name prefixed with @c "array:" (e.g., @c "array:int" for
 * @code int@endcode). The element type must satisfy the @c HasTypeName concept, meaning it must
 * expose its name via a @c T::NAME member or a @c TypeName<T> specialization.
 *
 * @tparam T The element type for which to produce the array type name.
 * @return A @c CString or @c std::string containing the array type name.
 */
template <typename T>
  requires HasTypeName<T>
[[nodiscard]] constexpr auto GetTypeArrayName() {
  constexpr auto arrayPrefix = CString("array:");

  if constexpr (HasTypeNameMember<T>) {
    if constexpr (requires { ToCString(T::NAME); }) {
      return arrayPrefix + ToCString(T::NAME);
    } else {
      static_assert(std::is_convertible_v<decltype(T::NAME), const char*>,
                    "GetTypeArrayName requires T::NAME to be core::CString, a char array literal, or const char*.");
      return TypePrefix<TypeKind::ARRAY>(std::string_view(T::NAME));
    }
  }

  static_assert(HasTypeNameMapping<T>);
  if constexpr (requires { ToCString(TypeName<T>::value); }) {
    return arrayPrefix + ToCString(TypeName<T>::value);
  } else {
    static_assert(
        std::is_convertible_v<decltype(TypeName<T>::value), const char*>,
        "GetTypeArrayName requires TypeName<T>::value to be core::CString, a char array literal, or const char*.");

    return arrayPrefix + ToCString(TypeName<T>::value);
  }
}
}  // namespace core::rtti
