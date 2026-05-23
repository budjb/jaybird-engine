#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace core {
using hash_t = std::uint64_t;

/**
 * @brief The IName class represents an interned string name that can be used as a unique identifier for various
 * entities in the system, such as types, functions, or variables. It provides methods for converting between the
 * interned string and its hash value, as well as comparison operators for checking equality between IName instances.
 */
class IName {
 public:
  /**
   * @brief Constructs an IName with a hash value of 0, representing an empty or invalid name.
   */
  IName() = default;

  /**
   * @brief Constructs an IName with the given hash value, which should be a valid hash of an interned string.
   *
   * @param hash The hash value of the interned string name.
   */
  explicit IName(hash_t hash) noexcept;

  /**
   * @brief Constructs an IName by computing the hash value of the given string view, which should be a valid interned
   * string.
   *
   * @param str The string view of the interned string name to be hashed and stored in the IName instance.
   * @note The constructor does not register the provided string with the interned name pool.
   */
  explicit IName(std::string_view str) noexcept;

  /**
   * @brief Returns whether this IName instance represents an empty or invalid name, which is determined by checking if
   * its hash value is 0. An IName with a hash value of 0 is considered empty or invalid, while an IName with a non-zero
   * hash value is considered valid and can be used to retrieve the corresponding interned string name from the name
   * pool.
   *
   * Critically, a false result from this function does not imply that the interned name is registered with the pool,
   * but rather that the contained hash is zero.
   *
   * @return true if this IName instance has a hash value of 0, indicating that it is empty or invalid; false otherwise.
   */
  [[nodiscard]] bool empty() const noexcept;

  /**
   * @brief Returns the string view of the interned string name associated with this IName instance, if it exists in the
   * interned name pool. If the hash value of this IName does not correspond to a valid interned string, an empty string
   * view may be returned.
   *
   * @return The string view of the interned string name associated with this IName instance, or an empty string view if
   * the hash value does not correspond to a valid interned string.
   */
  [[nodiscard]] std::string_view toString() const noexcept;

  /**
   * @brief Returns the hash value of the interned string name associated with this IName instance, which can be used as
   * a unique identifier for the name. The hash value is computed using a specific hashing algorithm (e.g., FNV-1a) and
   * should be consistent for the same string input across different runs of the program.
   *
   * @return The hash value of the interned string name associated with this IName instance.
   */
  [[nodiscard]] hash_t hash() const noexcept;

  /**
   * @brief Converts this IName instance to its underlying hash value, allowing it to be used in contexts where a hash
   * value is expected, such as in hash-based containers or when performing comparisons with other hash values.
   *
   * @return The hash value of the interned string name associated with this IName instance.
   */
  operator hash_t() const noexcept;

  /**
   * @brief Returns whether this IName instance represents a valid interned string name, which is determined by checking
   * if its hash value is non-zero. An IName with a hash value of 0 is considered invalid or empty, while an IName with
   * a non-zero hash value is considered valid and can be used to retrieve the corresponding interned string name from
   * the name pool.
   *
   * Critically, a true result from this function does not imply that the interned name is registered with the pool, but
   * rather that the contained hash is non-zero.
   *
   * @return true if this IName instance has a non-zero hash value, indicating that it may represent a valid interned
   * string name; false otherwise.
   */
  operator bool() const noexcept;

  /**
   * @brief Converts this IName instance to a string view of the interned string name it represents, allowing it to be
   * used in contexts where a string view is expected, such as when printing the name or performing string-based
   * comparisons. If the hash value of this IName does not correspond to a valid interned string, an empty string view
   * may be returned.
   *
   * @return The string view of the interned string name associated with this IName instance, or an empty string view if
   * the hash value does not correspond to a valid interned string.
   */
  operator std::string_view() const noexcept;

  /**
   * @brief Compares this IName instance with another IName instance for equality by comparing their hash values. If the
   * hash values of both IName instances are the same, they are considered equal, indicating that they likely represent
   * the same interned string name.
   *
   * @param other The other IName instance to compare with this instance for equality.
   * @return true if the hash values of both IName instances are the same, indicating that they are equal; false
   * otherwise.
   */
  bool operator==(const IName& other) const noexcept;

  /**
   * @brief Compares this IName instance with another IName instance for inequality by comparing their hash values. If
   * the hash values of both IName instances are different, they are considered not equal, indicating that they likely
   * represent different interned string names.
   *
   * @param other The other IName instance to compare with this instance for inequality.
   * @return true if the hash values of both IName instances are different, indicating that they are not equal; false
   * otherwise.
   */
  bool operator!=(const IName& other) const noexcept;

 private:
  /**
   * @brief The hash value of the interned string name associated with this IName instance, which serves as a unique
   * identifier for the name. The hash value is computed using a specific hashing algorithm (e.g., FNV-1a) and should be
   * consistent for the same string input across different runs of the program.
   *
   * A hash value of 0 is considered invalid or empty, while a non-zero hash value is considered valid and can be used
   * to retrieve the corresponding interned string name from the name pool.
   */
  const hash_t m_hash = 0;
};
}  // namespace core

/**
 * @brief Specialization of the std::hash template for the core::IName class, allowing IName instances to be used as
 * keys in hash-based containers such as std::unordered_map or std::unordered_set.
 */
template <>
struct std::hash<core::IName> {
  std::size_t operator()(const core::IName& s) const noexcept {
    return std::hash<core::hash_t>{}(s.hash());
  }
};
