#pragma once

#include <concepts>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include "Vector.hpp"
#include "rtti/RTTITypeKind.hpp"
#include "types/CString.hpp"

namespace core::rtti {
namespace detail {
/**
 * @brief Maps integral signedness and width to a fixed-width integer type.
 *
 * The primary template maps unsupported combinations to @c void so callers can
 * fall back to the original source type.
 *
 * @tparam IsSigned This value indicates whether the source type is signed.
 * @tparam Size The width in bytes.
 */
template <bool IsSigned, std::size_t Size>
struct FixedWidthIntegral {
  using type = void;
};

/**
 * @brief Specialization of @c FixedWidthIntegral for @code int8_t@endcode.
 */
template <>
struct FixedWidthIntegral<true, 1> {
  using type = std::int8_t;
};

/**
 * @brief Specialization of @c FixedWidthIntegral for @code int16_t@endcode.
 */
template <>
struct FixedWidthIntegral<true, 2> {
  using type = std::int16_t;
};

/**
 * @brief Specialization of @c FixedWidthIntegral for @code int32_t@endcode.
 */
template <>
struct FixedWidthIntegral<true, 4> {
  using type = std::int32_t;
};

/**
 * @brief Specialization of @c FixedWidthIntegral for @code int64_t@endcode.
 */
template <>
struct FixedWidthIntegral<true, 8> {
  using type = std::int64_t;
};

/**
 * @brief Specialization of @c FixedWidthIntegral for @code uint8_t@endcode.
 */
template <>
struct FixedWidthIntegral<false, 1> {
  using type = std::uint8_t;
};

/**
 * @brief Specialization of @c FixedWidthIntegral for @code uint16_t@endcode.
 */
template <>
struct FixedWidthIntegral<false, 2> {
  using type = std::uint16_t;
};

/**
 * @brief Specialization of @c FixedWidthIntegral for @code uint32_t@endcode.
 */
template <>
struct FixedWidthIntegral<false, 4> {
  using type = std::uint32_t;
};

/**
 * @brief Specialization of @c FixedWidthIntegral for @code uint64_t@endcode.
 */
template <>
struct FixedWidthIntegral<false, 8> {
  using type = std::uint64_t;
};

/**
 * @brief Maps a source type to the canonical key used for @c RTTINameProvider lookups.
 *
 * This mapper removes cv-ref qualifiers and normalizes non-boolean, non-character integral types to fixed-width signed
 * or unsigned integer types by width.
 *
 * This ensures that fixed width types are portable since the size of native C++ types are not guaranteed.
 *
 * @tparam T The source type to map.
 */
template <typename T>
struct CanonicalTypeNameType {
 private:
  /**
   * @brief This alias is the type used as the base for the fixed-width type lookup.
   */
  using BaseType = std::remove_cvref_t<T>;

  /**
   * @brief This flag indicates whether the base type is a character type that should be excluded from integral
   * normalization.
   */
  static constexpr bool isCharacterCategory = std::same_as<BaseType, char> || std::same_as<BaseType, wchar_t> ||
                                              std::same_as<BaseType, char8_t> || std::same_as<BaseType, char16_t> ||
                                              std::same_as<BaseType, char32_t>;

  /**
   * @brief This flag indicates whether the base type is an integral type that is a candidate for normalization to a
   * fixed-width type.
   */
  static constexpr bool isIntegralCandidate =
      std::is_integral_v<BaseType> && !std::same_as<BaseType, bool> && !isCharacterCategory;

  /**
   * @brief This alias is the canonical integral type corresponding to the base type's signedness and width, or
   * @c void when the base type is not a candidate for normalization.
   */
  using CanonicalIntegral = FixedWidthIntegral<std::is_signed_v<BaseType>, sizeof(BaseType)>::type;

 public:
  /**
   * @brief This alias is the canonical type to use for @c RTTINameProvider lookups.
   *
   * It is the normalized fixed-width integral type when the base type qualifies, or the original base type otherwise.
   */
  using type =
      std::conditional_t<isIntegralCandidate && !std::same_as<CanonicalIntegral, void>, CanonicalIntegral, BaseType>;
};
}  // namespace detail

/**
 * @brief Returns the canonical type used as a @c RTTINameProvider lookup key.
 *
 * @tparam T The source type to normalize.
 */
template <typename T>
using CanonicalType = detail::CanonicalTypeNameType<T>::type;

/**
 * @brief Returns the string prefix associated with a given @c RTTITypeKind for use in type name construction.
 *
 * The base template returns an empty @c CString; specializations provide the appropriate prefix
 * for each kind (e.g., @c "array:" for @code RTTITypeKind::ARRAY@endcode).
 *
 * @return The prefix @c CString for the given @c RTTITypeKind, or an empty @c CString if none is defined.
 */
template <RTTITypeKind>
constexpr auto GetRTTIPrefix() {
  return CString("");
}

/**
 * @brief Returns the prefix string for array type names.
 *
 * @return The string @c "array:" used as a prefix for array type names.
 */
template <>
constexpr auto GetRTTIPrefix<RTTITypeKind::ARRAY>() {
  return CString("array:");
}

/**
 * @brief Returns the prefix string for reference type names.
 *
 * @return The string @c "ref:" used as a prefix for reference type names.
 */
template <>
constexpr auto GetRTTIPrefix<RTTITypeKind::REF>() {
  return CString("ref:");
}

/**
 * @brief Returns the prefix string for weak reference type names.
 *
 * @return The string @c "wref:" used as a prefix for weak reference type names.
 */
template <>
constexpr auto GetRTTIPrefix<RTTITypeKind::WEAK_REF>() {
  return CString("wref:");
}

/**
 * @brief Returns the prefix string for raw pointer type names.
 *
 * @return The string @c "ptr:" used as a prefix for raw pointer type names.
 */
template <>
constexpr auto GetRTTIPrefix<RTTITypeKind::POINTER>() {
  return CString("ptr:");
}

/**
 * @brief Provider template for mapping a C++ type @c T to its RTTI string name.
 *
 * Specialize this struct for any type that cannot declare a @c static constexpr NAME member, providing a
 * @code static constexpr CString value@endcode member with the desired name.
 *
 * @tparam T The type for which to declare a name provider.
 */
template <typename T>
struct RTTINameProvider;

/**
 * @brief Variable template that is @c true when @c T exposes a static @c NAME member convertible to a @c CString or
 * character array.
 *
 * It allows a type to declare its own RTTI name inline rather than through a @c RTTINameProvider specialization.
 *
 * @tparam T The type to check for a static @c NAME member.
 */
template <typename T>
constexpr bool has_type_name_member_v = requires {
  { CanonicalType<T>::NAME } -> CStringConvertible;
};

/**
 * @brief Variable template that is @c true when a @c RTTINameProvider<T> specialization exists and defines a @c value
 * member convertible to a @c CString or character array.
 *
 * @tparam T The type to check for a @code RTTINameProvider@endcode.
 */
template <typename T>
constexpr bool has_type_name_provider_v = requires {
  { RTTINameProvider<CanonicalType<T>>::value } -> CStringConvertible;
};

/**
 * @brief Concept that is satisfied when a type name can be resolved for @c T, either via a @c T::NAME member
 * or a @c RTTINameProvider<T> specialization.
 *
 * @tparam T The type to check for an available type name.
 */
template <typename T>
concept NamedRTTIType = has_type_name_member_v<T> || has_type_name_provider_v<T>;

/**
 * @brief Retrieves the type name for @c T as a compile-time @code CString@endcode.
 *
 * The name is resolved by checking @c T::NAME first (via @code has_type_name_member_v@endcode), then falling back to
 * a @c RTTINameProvider<T> specialization (via @code has_type_name_provider_v@endcode). The @c NamedRTTIType constraint
 * guarantees that at least one of the two paths is available.
 *
 * @tparam T The type whose name is to be retrieved. It must satisfy @code NamedRTTIType@endcode.
 * @return The type name as a @code CString@endcode.
 */
template <NamedRTTIType T>
constexpr auto GetRTTIName() noexcept {
  using Type = CanonicalType<T>;

  if constexpr (has_type_name_member_v<Type>) {
    return CString(Type::NAME);
  } else if constexpr (has_type_name_provider_v<Type>) {
    return CString(RTTINameProvider<Type>::value);
  }
}

/**
 * @brief Returns the canonical prefixed type name for element type @c T at compile time.
 *
 * The result is the element type's name prefixed with the string corresponding to @c K (e.g., @c "array:int"
 * for @c RTTITypeKind::ARRAY and @code int@endcode). @c T::NAME is preferred when present; otherwise the
 * @c RTTINameProvider<T> specialization is used. The @c NamedRTTIType constraint guarantees that one of the two is
 * available.
 *
 * @tparam K The @c RTTITypeKind whose prefix to prepend.
 * @tparam T The element type for which to produce the prefixed name. It must satisfy @code NamedRTTIType@endcode.
 * @return A @c CString containing the prefixed type name.
 */
template <RTTITypeKind K, NamedRTTIType T>
[[nodiscard]] constexpr auto GetPrefixedRTTIName() {
  return GetRTTIPrefix<K>() + GetRTTIName<T>();
}

/**
 * @brief Returns the canonical type name for a runtime string, prefixed with the string associated with @c K.
 *
 * @tparam K The @c RTTITypeKind whose prefix to prepend.
 * @param name The base type name to prefix.
 * @return A @c std::string containing the concatenated prefix and base name.
 */
template <RTTITypeKind K>
[[nodiscard]] std::string GetPrefixedRTTIName(const std::string_view name) {
  return GetRTTIPrefix<K>().append(name);
}

/**
 * @brief Name-provider specialization for @code std::shared_ptr<T>@endcode.
 *
 * This provider maps @code std::shared_ptr<T>@endcode to the canonical prefixed RTTI name
 * for reference descriptors, such as @c "ref:MyType".
 *
 * @tparam T The referenced element type, which must satisfy @code NamedRTTIType@endcode.
 */
template <NamedRTTIType T>
struct RTTINameProvider<std::shared_ptr<T>> {
  /**
   * @brief The canonical RTTI name for @code std::shared_ptr<T>@endcode.
   */
  static constexpr auto value = GetPrefixedRTTIName<RTTITypeKind::REF, T>();
};

/**
 * @brief Concept that is satisfied when @c C is a @c Vector specialization.
 *
 * It checks that @c C exposes a @c value_type member and that @c C is exactly
 * @c Vector of that element type.
 *
 * @tparam C The type to check.
 */
template <typename C>
concept VectorType = requires { typename std::remove_cvref_t<C>::value_type; } &&
                     std::same_as<std::remove_cvref_t<C>, Vector<typename std::remove_cvref_t<C>::value_type>>;

/**
 * @brief Concept that is satisfied when @c C is a @c Vector and its element type satisfies
 * @code NamedRTTIType@endcode.
 *
 * @tparam C The type to check.
 */
template <typename C>
concept NamedVectorType = VectorType<C> && NamedRTTIType<typename std::remove_cvref_t<C>::value_type>;

/**
 * @brief Returns the canonical type name for a @c Vector whose element type satisfies @code NamedRTTIType@endcode.
 *
 * The returned name uses the @c RTTITypeKind::ARRAY prefix followed by the element type name
 * (e.g., @c "array:double" for @code Vector<double>@endcode).
 *
 * @tparam C The container type to name. It must satisfy @code NamedVectorType@endcode.
 * @return A @c CString containing the prefixed array type name.
 */
template <NamedVectorType C>
constexpr auto GetRTTIName() {
  return GetPrefixedRTTIName<RTTITypeKind::ARRAY, typename std::remove_cvref_t<C>::value_type>();
}

}  // namespace core::rtti

/**
 * @brief Macro to create a specialization of @c GetRTTIPrefix for the given @c RTTITypeKind enumeration.
 *
 * @param _kind The @c RTTITypeKind for which to register the prefix (e.g., @c RTTITypeKind::ARRAY).
 * @param _prefix The string prefix to associate with the given @c RTTITypeKind (e.g., @c "array:").
 */
#define REGISTER_TYPE_PREFIX(_kind, _prefix)          \
  template <>                                         \
  constexpr auto core::rtti::GetRTTIPrefix<_kind>() { \
    return core::CString(_prefix);                    \
  }

/**
 * @brief Macro to register a type name provider for a type that cannot declare a static @c NAME member.
 *
 * @param _type The C++ type for which to register the name provider (e.g., @c Vector<int>).
 * @param _name The string name to associate with the given type (e.g., @c "array:int").
 */
#define REGISTER_TYPE_NAME(_type, _name)         \
  template <>                                    \
  struct core::rtti::RTTINameProvider<_type> {   \
    static constexpr core::CString value{_name}; \
  }

REGISTER_TYPE_NAME(std::int8_t, "int8");
REGISTER_TYPE_NAME(std::int16_t, "int16");
REGISTER_TYPE_NAME(std::int32_t, "int32");
REGISTER_TYPE_NAME(std::int64_t, "int64");
REGISTER_TYPE_NAME(std::uint8_t, "uint8");
REGISTER_TYPE_NAME(std::uint16_t, "uint16");
REGISTER_TYPE_NAME(std::uint32_t, "uint32");
REGISTER_TYPE_NAME(std::uint64_t, "uint64");
REGISTER_TYPE_NAME(float, "float");
REGISTER_TYPE_NAME(double, "double");
REGISTER_TYPE_NAME(bool, "bool");
REGISTER_TYPE_NAME(std::string, "string");
