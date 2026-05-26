#pragma once

#include <cstdint>
#include <string_view>

namespace core {
/**
 * @brief Type alias for hash values used throughout the RTTI system.
 */
using hash_t = std::uint64_t;

/**
 * @brief Computes the FNV-1a 64-bit hash of the given string.
 *
 * @param str The string to hash.
 * @return The computed hash value.
 */
constexpr hash_t fnv1a_64(const std::string_view str) noexcept {
  static constexpr uint64_t fnv_prime = 1099511628211ULL;
  static constexpr uint64_t fnv_offset_basis = 14695981039346656037ULL;

  uint64_t hash = fnv_offset_basis;

  for (const char c : str) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= fnv_prime;
  }

  return hash;
}
}  // namespace core
