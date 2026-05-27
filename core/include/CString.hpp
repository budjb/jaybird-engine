#pragma once

#include <string>
#include <string_view>

#include "IName.hpp"

namespace core {
/**
 * @brief A fixed-size compile-time string type for storing type names and string literals.
 *
 * The string size is determined by template parameter @c N, which should include the null terminator. This type
 * captures string literals as compile-time constants that can be evaluated at compile time.
 *
 * @tparam N The size of the string literal, including the null terminator (e.g., N = 4 for "Foo").
 */
template <std::size_t N>
class CString {
 public:
  /**
   * @brief Constructs a CString from a string literal by copying characters from the provided
   * string literal, including the null terminator.
   *
   * @param str Reference to a character array of size N
   */
  explicit constexpr CString(const char (&str)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
      value[i] = str[i];
    }
  }

  /**
   * @brief Checks if the string is empty, which is true when @c N is 1 (i.e., the buffer holds only the null
   * terminator and no visible characters).
   *
   * @return @c true if the string contains no characters, @c false otherwise.
   */
  [[nodiscard]] constexpr bool empty() const noexcept {
    return N == 1;
  }

  /**
   * @brief Returns the size of the string buffer, including the null terminator.
   *
   * The actual length of the visible string content is @c N - 1.
   *
   * @return The total buffer size, including the null terminator.
   */
  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return N;
  }

  /**
   * @brief Returns the length of the string content, excluding the null terminator.
   *
   * @return The number of visible characters, equal to @c N - 1.
   */
  [[nodiscard]] constexpr std::size_t length() const noexcept {
    return N - 1;
  }

  /**
   * @brief Returns a new @c std::string formed by appending a @c std::string_view to this string.
   *
   * @param other The @c std::string_view to append.
   * @return A new @c std::string containing the concatenated result of this @c CString and the provided
   * @code std::string_view@endcode.
   */
  [[nodiscard]] std::string append(const std::string_view other) const {
    std::string str = value;
    str.append(other);
    return str;
  }

  /**
   * @brief Returns a new @c CString formed by appending another @c CString to this one.
   *
   * @tparam M The size of the other @c CString, including the null terminator.
   * @param other The @c CString to append.
   * @return A new @c CString containing the concatenated result.
   */
  template <std::size_t M>
  [[nodiscard]] constexpr auto append(const CString<M>& other) const {
    char data[N + M - 1]{};

    for (std::size_t i = 0; i < N - 1; ++i) {
      data[i] = value[i];
    }

    for (std::size_t i = 0; i < M - 1; ++i) {
      data[N - 1 + i] = other.c_str()[i];
    }

    data[N + M - 2] = '\0';
    return CString<N + M - 1>(data);
  }

  /**
   * @brief Implicit conversion to @code std::string_view@endcode.
   *
   * @return A @c std::string_view over the string data, equivalent to calling @code sv()@endcode.
   */
  constexpr operator std::string_view() const {
    return {value, N - 1};
  }

  /**
   * @brief Implicit conversion to @code IName@endcode.
   *
   * @return An @c IName constructed by hashing the string data.
   */
  constexpr operator IName() const {
    return IName{value};
  }

  /**
   * @brief Implicit conversion to @code bool@endcode.
   *
   * It returns @c true if the string is empty (i.e., @c empty() returns @c true), and @c false otherwise.
   *
   * @return @c true if the string is empty, @c false otherwise.
   */
  constexpr operator bool() const noexcept {
    return empty();
  }

  /**
   * @brief Returns a new @c std::string formed by concatenating @c lhs and @code rhs@endcode.
   *
   * Unlike the @c CString overload, this operator returns a @c std::string because the result size cannot
   * be determined at compile time.
   *
   * @param lhs The left-hand @code CString@endcode.
   * @param rhs The right-hand @c std::string_view to append.
   * @return A new @c std::string containing the concatenated result.
   */
  friend constexpr auto operator+(const CString& lhs, const std::string_view rhs) {
    return lhs.append(rhs);
  }

  /**
   * @brief Returns a new @c CString formed by concatenating @c lhs and @code rhs@endcode.
   *
   * @tparam M The size of the right-hand @c CString, including the null terminator.
   * @param lhs The left-hand @code CString@endcode.
   * @param rhs The right-hand @code CString@endcode.
   * @return A new @c CString containing the concatenated result.
   */
  template <std::size_t M>
  friend constexpr auto operator+(const CString& lhs, const CString<M>& rhs) {
    return lhs.append(rhs);
  }

  /**
   * @brief Returns the underlying null-terminated C-style string.
   *
   * @return A pointer to the null-terminated character array of length @code N@endcode.
   */
  [[nodiscard]] constexpr const char* c_str() const {
    return value;
  }

 private:
  /**
   * @brief The character array that holds the string data.
   */
  char value[N]{};
};

/**
 * @brief Deduction guide for CString. Allows construction without explicit template parameter.
 *
 * Usage: CString("Foo") automatically deduces N = 4
 *
 * @tparam N The size of the string literal, deduced from the array size
 */
template <std::size_t N>
CString(const char (&)[N]) -> CString<N>;

/**
 * @brief Primary type trait template for detecting @c CString specializations.
 *
 * It inherits from @c std::false_type for all types that are not a @c CString specialization.
 * The partial specialization below overrides this for @c CString<N>.
 */
template <typename>
struct is_cstring : std::false_type {};

/**
 * @brief Partial specialization of @c is_cstring for @c CString<N>.
 *
 * It inherits from @c std::true_type, making this trait evaluate to @c true for any @c CString<N>.
 *
 * @tparam N The size of the @c CString specialization, including the null terminator.
 */
template <std::size_t N>
struct is_cstring<CString<N>> : std::true_type {};

/**
 * @brief Variable template that is @c true if @c T is a @c CString specialization, with cv and reference
 * qualifiers stripped before checking.
 *
 * @tparam T The type to check.
 */
template <typename T>
constexpr bool is_cstring_v = is_cstring<std::remove_cvref_t<T>>::value;

/**
 * @brief Variable template that is @c true if @c T is a character array type
 * (e.g., @c char[], @c const char[], or @c char[N]).
 *
 * @tparam T The type to check.
 */
template <typename T>
constexpr bool is_char_array_v =
    std::is_array_v<std::remove_reference_t<T>> &&
    std::is_same_v<char, std::remove_cv_t<std::remove_all_extents_t<std::remove_reference_t<T>>>>;

/**
 * @brief Concept that is satisfied when @c T is either a @c CString specialization or a character array.
 *
 * It is intended to constrain template parameters that can be used as compile-time C-style string sources.
 *
 * @tparam T The type to check for CString convertibility.
 */
template <typename T>
concept is_cstring_convertible_v = is_cstring_v<T> || is_char_array_v<T>;
}  // namespace core
