#pragma once

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
   * @brief Returns a @c std::string_view over the string data, excluding the null terminator.
   *
   * @return A view of the string data with length @c N-1.
   */
  [[nodiscard]] constexpr std::string_view sv() const {
    return {value, N - 1};
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
    return sv();
  }

  /**
   * @brief Implicit conversion to @code IName@endcode.
   *
   * @return An @c IName constructed by hashing the string data.
   */
  constexpr operator IName() const {
    return {sv()};
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
}  // namespace core
