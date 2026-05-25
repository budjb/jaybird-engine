#pragma once

#include <concepts>
#include <string_view>

namespace core::rtti {
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
}  // namespace core::rtti
