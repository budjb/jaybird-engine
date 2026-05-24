#pragma once

#include <string_view>

namespace core {
/**
 * @brief A fixed-size string type that can be used for compile-time string literals. This is useful for storing type
 * names and other string data in a way that can be evaluated at compile time.
 *
 * The size of the string is determined by the template parameter N, which should be the size of the string literal
 * including the null terminator.
 *
 * @tparam N The size of the string literal, including the null terminator. For example, for a string literal "Foo", N
 * should be 4.
 */
template <std::size_t N>
struct FixedString {
  /**
   * @brief The character array that holds the string data. This is initialized in the constructor by copying the
   * characters from the provided string literal. The size of the array is N, which includes space for the null
   * terminator.
   */
  char value[N]{};

  /**
   * @brief Constructs a FixedString from a string literal. The constructor takes a reference to a character array of
   * size N, which should be a string literal. The characters from the string literal are copied into the value array,
   * including the null terminator.
   *
   * @param str
   */
  explicit constexpr FixedString(const char (&str)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
      value[i] = str[i];
    }
  }

  /**
   * @brief Returns a std::string_view representing the string data stored in the FixedString. This method returns a
   * view of the string data that excludes the null terminator, allowing for easy use of the FixedString in contexts
   * where a string_view is expected. The returned string_view is valid as long as the FixedString instance is valid,
   * and it provides a convenient way to access the string data without needing to copy it or manage memory.
   *
   * @note The returned string_view does not include the null terminator, so it will have a size of N - 1.
   *
   * @return std::string_view A view of the string data stored in the FixedString, excluding the null terminator.
   */
  [[nodiscard]] constexpr std::string_view sv() const {
    return {value, N - 1};
  }

  /**
   * @brief Implicit conversion operator that returns a std::string_view representing the string data stored in the
   * FixedString. This method returns a view of the string data that excludes the null terminator, allowing for easy use
   * of the FixedString in contexts where a string_view is expected. The returned string_view is valid as long as the
   * FixedString instance is valid, and it provides a convenient way to access the string data without needing to copy
   * it or manage memory.
   *
   * @note The returned string_view does not include the null terminator, so it will have a size of N - 1.
   *
   * @return std::string_view A view of the string data stored in the FixedString, excluding the null terminator.
   */
  operator std::string_view() const {
    return sv();
  }
};

/**
 * @brief Deduction guide for FixedString. This allows for the construction of a FixedString from a string literal
 * without needing to explicitly specify the template parameter N. The deduction guide takes a reference to a character
 * array of size N, which should be a string literal, and deduces the template parameter N from the size of the array.
 *
 * This makes it more convenient to create FixedString instances from string literals, as the user can simply write
 * FixedString("Foo") and the compiler will automatically deduce that N is 4 (including the null terminator).
 *
 * @tparam N The size of the string literal, including the null terminator. This is deduced from the size of the
 * character array parameter.
 * @return A FixedString instance initialized with the provided string literal. The size of the FixedString is
 * determined by the size of the string literal, including the null terminator.
 */
template <std::size_t N>
FixedString(const char (&)[N]) -> FixedString<N>;

}  // namespace core
