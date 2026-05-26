#pragma once

#include <string_view>

#include "IName.hpp"

namespace core {
/**
 * @brief A fixed-size compile-time string type for storing type names and other string literals.
 *
 * The size of the string is determined by the template parameter N, which should be the size of
 * the string literal including the null terminator. This type provides a way to capture string
 * literals as compile-time constants that can be evaluated at compile time.
 *
 * @tparam N The size of the string literal, including the null terminator. For example, for a
 * string literal "Foo", N should be 4.
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
   * @brief Returns a std::string_view representing the string data, excluding the null terminator.
   *
   * @return std::string_view A view of the string data (size N - 1)
   */
  [[nodiscard]] constexpr std::string_view sv() const {
    return {value, N - 1};
  }

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
   * @brief Implicit conversion to std::string_view.
   */
  constexpr operator std::string_view() const {
    return sv();
  }

  constexpr operator IName() const {
    return {sv()};
  }

  template <std::size_t M>
  friend constexpr auto operator+(const CString& lhs, const CString<M>& rhs) {
    return lhs.append(rhs);
  }
  /**
   * @brief Returns the underlying C-style string, including its null terminator.
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
