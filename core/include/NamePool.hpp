#pragma once

#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "Export.hpp"
#include "Hash.hpp"
#include "Name.hpp"

namespace core {
/**
 * @brief Singleton manager for interned strings, mapping FNV-1a hashes to their original string values.
 *
 * It is thread-safe for concurrent reads via @c std::shared_mutex; writes acquire an exclusive lock.
 */
class JAYBIRD_API NamePool {
 public:
  /**
   * @brief Returns the singleton instance of the @code NamePool@endcode.
   *
   * @return A reference to the singleton @code NamePool@endcode.
   */
  static NamePool& get();

  /**
   * @brief Interns a string and returns its associated @code Name@endcode.
   *
   * This method is thread-safe. If the string is already interned, the existing @c Name is returned without
   * modification.
   *
   * @param str The string to intern.
   * @return A @c Name representing the hash of the string.
   */
  Name addName(std::string_view str) noexcept;

  /**
   * @brief Retrieves the string associated with the given @code Name@endcode.
   *
   * @param str The @c Name to look up.
   * @return The interned string view, or an empty view if not found.
   */
  [[nodiscard]] std::string_view getName(const Name& str) const noexcept;

  /**
   * @brief Retrieves the string associated with the given hash value.
   *
   * @param hash The hash to look up.
   * @return The interned string view, or an empty view if not found.
   */
  [[nodiscard]] std::string_view getName(hash_t hash) const noexcept;

  /**
   * @brief Checks whether the given @c Name is present in the pool.
   *
   * @param str The @c Name to check.
   * @return @c true if the name exists, @c false otherwise.
   */
  bool hasName(const Name& str) const noexcept;

  /**
   * @brief Returns the string associated with the given @code Name@endcode.
   *
   * @param str The @c Name to look up.
   * @return The interned string view, or an empty view if not found.
   */
  std::string_view operator[](const Name& str) const noexcept {
    return getName(str);
  }

  /**
   * @brief Returns the string associated with the given hash value.
   *
   * @param hash The hash to look up.
   * @return The interned string view, or an empty view if not found.
   */
  std::string_view operator[](const hash_t hash) const noexcept {
    return getName(hash);
  }

 private:
  /**
   * @brief Private constructor, enforcing singleton access via @code get()@endcode.
   */
  NamePool() = default;

  /**
   * @brief Protects concurrent access to the pool.
   */
  mutable std::shared_mutex m_mutex;

  /**
   * @brief Maps interned hashes to their corresponding strings.
   */
  std::unordered_map<Name, std::string> m_names;
};
}  // namespace core
