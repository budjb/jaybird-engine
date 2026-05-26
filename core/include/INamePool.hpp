#pragma once

#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "Hash.hpp"
#include "IName.hpp"
#include "Export.hpp"

namespace core {
/**
 * @brief A singleton class that manages a pool of interned string names, allowing for efficient storage and retrieval
 * of strings based on their hash values. The INamePool class provides methods for adding strings to the pool,
 * retrieving strings based on their hash values, and checking for the existence of strings in the pool.
 *
 * The INamePool class is designed to be used in conjunction with the IName class, which represents interned string
 * names as hash values. This provides a compact and efficient way to represent strings in the system while still
 * allowing for easy retrieval of the original string values when needed.
 */
class JAYBIRD_API INamePool {
 public:
  /**
   * @brief Retrieves the singleton instance of the INamePool class, which manages the pool of interned string names.
   * This method ensures that only one instance of the INamePool class exists throughout the program, providing a
   * centralized location for managing interned string names and their associated hash values.
   *
   * @return A reference to the singleton instance of the INamePool class.
   */
  static INamePool& get();

  /**
   * @brief Adds a string to the pool of interned string names and returns an IName object representing the hash value
   * of the string.
   *
   * This method ensures that each unique string is stored only once in the pool, allowing for efficient memory usage
   * and fast retrieval of string values based on their hash values.
   *
   * @param str The string view to be added to the pool of interned string names. The string view should be a valid
   * UTF-8 encoded string, and the method will compute its hash value and store it in the pool if it does not already
   * exist.
   * @return An IName object representing the hash value of the input string view.
   */
  IName addName(std::string_view str) noexcept;

  /**
   * @brief Retrieves the original string value associated with a given IName object, which represents the hash value of
   * the string.
   *
   * @param str The IName object representing the hash value of the string for which to retrieve the original string
   * value.
   * @return A string view representing the original string value associated with the given IName object. If the hash
   * value does not exist in the pool, an empty string view will be returned.
   */
  [[nodiscard]] std::string_view getName(const IName& str) const noexcept;

  /**
   * @brief Retrieves the original string value associated with a given hash value, which is represented as a 64-bit
   * unsigned integer.
   *
   * @param hash The hash value for which to retrieve the original string value.
   * @return A string view representing the original string value associated with the given hash value. If the hash
   * value does not exist in the pool, an empty string view will be returned.
   */
  [[nodiscard]] std::string_view getName(hash_t hash) const noexcept;

  /**
   * @brief Checks if a given IName object, which represents the hash value of a string, exists in the pool of interned
   * string names.
   *
   * @param str The IName object representing the hash value of the string to check for existence in the pool.
   * @return A boolean value indicating whether the given IName object exists in the pool of interned string names.
   * Returns true if the IName object exists in the pool, and false otherwise.
   */
  bool hasName(const IName& str) const noexcept;

  /**
   * @brief Overloaded subscript operator that allows for retrieving the original string value associated with a given
   * IName object or hash value using array-like syntax. This operator provides a convenient way to access the original
   * string values from the pool of interned string names based on their hash values.
   *
   * @param str The IName object representing the hash value of the string for which to retrieve the original string
   * value.
   * @return A string view representing the original string value associated with the given IName object. If the hash
   * value does not exist in the pool, an empty string view will be returned.
   */
  std::string_view operator[](const IName& str) const noexcept {
    return getName(str);
  }

  /**
   * @brief Overloaded subscript operator that allows for retrieving the original string value associated with a given
   * hash value using array-like syntax. This operator provides a convenient way to access the original string values
   * from the pool of interned string names based on their hash values.
   *
   * @param hash The hash value for which to retrieve the original string value.
   * @return A string view representing the original string value associated with the given hash value. If the hash
   * value does not exist in the pool, an empty string view will be returned.
   */
  std::string_view operator[](const hash_t hash) const noexcept {
    return getName(hash);
  }

 private:
  /**
   * @brief Constructs an INamePool object. The constructor is private to enforce the singleton pattern, ensuring that
   * only one instance of the INamePool class can be created throughout the program.
   */
  INamePool() = default;

  /**
   * @brief A mutable shared mutex used to synchronize access to the pool of interned string names, allowing for
   * concurrent read access while ensuring exclusive access for write operations. The mutex is mutable to allow for
   * locking in const member functions, enabling thread-safe access to the pool of interned string names even when
   * accessed through const references to the INamePool instance.
   */
  mutable std::shared_mutex m_mutex;

  /**
   * @brief An unordered map that serves as the underlying data structure for storing the pool of interned string names,
   * mapping IName objects (which represent hash values) to their corresponding string values. This map allows for
   * efficient storage and retrieval of string values based on their hash values, enabling fast lookups and
   * memory-efficient storage of strings in the system. The unordered map is used to ensure that each unique string is
   * stored only once in the pool, while still allowing for fast retrieval of string values based on their hash values.
   */
  std::unordered_map<IName, std::string> m_names;
};
}  // namespace core
