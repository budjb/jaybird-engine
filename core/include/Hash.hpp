#pragma once

#include <cstdint>
#include <string_view>

namespace core {
/**
 * @brief A type alias for the hash value type used in the system, which is defined as a 64-bit unsigned integer. This
 * type is used to represent the hash values of interned string names and other entities in the system, providing a
 * consistent and efficient way to store and compare hash values across different components of the system.
 */
using hash_t = std::uint64_t;

/**
 * @brief Computes the FNV-1a hash of a given string view, which is a widely used non-cryptographic hash function that
 * provides good performance and low collision rates for small inputs.
 *
 * The FNV-1a hash is computed by iterating over each character in the input string, XORing it with the current hash
 * value, and then multiplying the result by a prime number. The resulting hash value can be used as a unique identifier
 * for the input string, allowing it to be efficiently stored and compared in hash-based data structures such as hash
 * tables or hash maps.
 *
 * @param str The input string view for which to compute the FNV-1a hash. The string view should be a valid UTF-8
 * encoded string, and the hash value will be computed based on the bytes of the string.
 * @return The computed FNV-1a hash value for the input string view, represented as a 64-bit unsigned integer. The hash
 * value is computed using the FNV-1a algorithm and should be consistent for the same input string across different runs
 * of the program.
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
