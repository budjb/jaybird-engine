#pragma once

#include <concepts>
#include <string_view>

namespace core::rtti {
/**
 * @brief A template struct that serves as a mapping from types to their corresponding type names.
 *
 * This struct is intended to be specialized for specific types, where the specialization will define a static member
 * variable named value that holds the type name. Supported forms include CString<N>, string literals, const char*, and
 * other types convertible to std::string_view.
 */
template <typename>
struct TypeName;

/**
 * @brief A convenient variable template that provides easy access to the type name for a given type T.
 *
 * The type name is expected to be defined in a specialization of the TypeName struct for the type T, and it should be
 * a CString, const char*, string literal, or other type convertible to std::string_view that represents the name of
 * the type.
 *
 * @tparam T The type for which to retrieve the type name. This type should have a corresponding specialization of the
 * TypeName struct that defines the value member variable with the type name.
 * @return The type name for the specified type T, as defined in the TypeName struct specialization for T.
 */
template <typename T>
inline constexpr auto GetTypeName = TypeName<T>::value;

/**
 * @brief A concept that checks if a type T has a static member variable named NAME.
 *
 * This accepts the common RTTI naming styles, including:
 * - `static constexpr char NAME[] = "Foo";`
 * - `inline static constexpr auto NAME = "Foo";`
 * - `inline static constexpr CString<N> NAME{"Foo"};`
 * - `inline static constexpr const char* NAME = "Foo";`
 *
 * This is useful for types that want to provide their own type name without needing to specialize the TypeName struct.
 */
template <typename T>
concept HasTypeNameMember = requires {
  { T::NAME } -> std::convertible_to<std::string_view>;
};

/**
 * @brief A concept that checks if a type T has a corresponding type name mapping defined in the TypeName struct. This
 * concept is satisfied if there is a specialization of the TypeName struct for the type T that defines a static member
 * variable named value.
 *
 * @return True if there is a specialization of the TypeName struct for the type T that defines a static member variable
 * named value, or false otherwise. This concept is
 * satisfied if there is a specialization of the TypeName struct for the type T that defines a static member variable
 * named value.
 */
template <typename T>
concept HasTypeNameMapping = requires { TypeName<T>::value; };
}  // namespace core::rtti
