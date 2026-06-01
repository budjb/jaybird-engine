#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "Export.hpp"
#include "Hash.hpp"

namespace core::rtti {
template <typename T>
struct RTTINameProvider;
}

namespace core {
/**
 * @brief Type alias for the hash value used in @code Name@endcode, representing the FNV-1a hash of a string.
 */
using hash_t = std::uint64_t;

/**
 * @brief Represents an interned string name for efficient comparison and hashing.
 *
 * A @c Name is a lightweight handle that stores the FNV-1a hash of a string.
 * The actual string must be stored in the @code NamePool@endcode.
 */
class JAYBIRD_API Name {
 public:
  /**
   * @brief Constructs an empty @c Name with hash value 0.
   */
  constexpr Name() = default;

  /**
   * @brief Constructs a @c Name from the given hash value.
   *
   * @param hash The pre-computed FNV-1a hash value to store.
   */
  constexpr Name(hash_t hash) noexcept;

  /**
   * @brief Constructs a @c Name by hashing the given string literal at compile time.
   *
   * @tparam N The size of the string literal, including the null terminator (e.g., N = 4 for "Foo").
   * @param str Reference to a character array of size N representing the string literal to hash.
   */
  template <std::size_t N>
  constexpr Name(const char (&str)[N]) noexcept : m_hash(fnv1a_64(str)) {}

  /**
   * @brief Constructs a @c Name by hashing the given string view.
   *
   * @note This does not register the string with @code NamePool@endcode.
   * Call @c NamePool::get().addName() to do that.
   *
   * @param str The string view whose content is hashed to form the name.
   */
  constexpr Name(std::string_view str) noexcept;

  /**
   * @brief Constructs a @c Name by hashing the given string.
   *
   * @param str The string whose content is hashed to form the name.
   */
  Name(const std::string& str) noexcept;

  /**
   * @brief Constructs a @c Name by hashing the given null-terminated C-string.
   *
   * @param str A pointer to the null-terminated character sequence to hash.
   */
  constexpr Name(const char* str) noexcept;

  /**
   * @brief Returns @c true if this @c Name has hash value 0 (empty/invalid).
   *
   * @return @c true if the hash is 0, @c false otherwise.
   * @note A false result does not imply the name is registered with the pool.
   */
  [[nodiscard]] constexpr bool empty() const noexcept;

  /**
   * @brief Returns the string associated with this @c Name from the pool, or an empty view if not found.
   *
   * @return The interned string view, or an empty view if the hash is not registered in the pool.
   */
  [[nodiscard]] std::string_view toString() const noexcept;

  /**
   * @brief Returns the hash value of this @code Name@endcode.
   *
   * @return The stored FNV-1a hash value.
   */
  [[nodiscard]] constexpr hash_t hash() const noexcept;

  /**
   * @brief Implicit conversion to @code hash_t@endcode.
   *
   * @return The stored FNV-1a hash value.
   */
  constexpr operator hash_t() const noexcept;

  /**
   * @brief Returns @c true if this @c Name has a non-zero hash (potentially valid).
   *
   * @note A true result does not imply the name is registered with the pool.
   *
   * @return @c true if the hash is non-zero, @c false otherwise.
   */
  constexpr operator bool() const noexcept;

  /**
   * @brief Implicit conversion to @c std::string_view via @code toString()@endcode.
   *
   * @return The interned string view, or an empty view if not found in the pool.
   */
  constexpr operator std::string_view() const noexcept;

  /**
   * @brief Compares two @c Name instances by their hash values.
   *
   * @param other The @c Name instance to compare against this one.
   * @return @c true if the hash values are equal, @c false otherwise.
   */
  constexpr bool operator==(const Name& other) const noexcept;

  /**
   * @brief Compares two @c Name instances by their hash values.
   *
   * @param other The @c Name instance to compare against this one.
   * @return @c true if the hash values differ, @c false otherwise.
   */
  constexpr bool operator!=(const Name& other) const noexcept;

 private:
  /**
   * @brief The hash value of this @code Name@endcode.
   */
  hash_t m_hash = 0;
};
}  // namespace core

/**
 * @brief Specialization of @c std::hash for @code core::Name@endcode, allowing its use as a key in unordered
 * containers.
 */
template <>
struct std::hash<core::Name> {
  std::size_t operator()(const core::Name& s) const noexcept {
    return std::hash<core::hash_t>{}(s.hash());
  }
};

/**
 * @brief Specialization of @code TypeName@endcode that maps @code core::Name@endcode to the canonical RTTI name.
 */
template <>
struct core::rtti::RTTINameProvider<core::Name> {
  static constexpr char value[] = "Name";
};
