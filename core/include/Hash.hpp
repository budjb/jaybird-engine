#pragma once

#include <cstdint>
#include <string_view>

namespace core {
using hash_t = std::uint64_t;

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
