#include "types/Name.hpp"

#include "NamePool.hpp"

namespace core {
constexpr Name::Name(const hash_t hash) noexcept : m_hash(hash) {}

constexpr Name::Name(const std::string_view str) noexcept : Name(fnv1a_64(str)) {}

Name::Name(const std::string& str) noexcept : Name(fnv1a_64(str)) {}

constexpr Name::Name(const char* str) noexcept : Name(fnv1a_64(str)) {}

constexpr bool Name::empty() const noexcept {
  return m_hash == 0;
}

std::string_view Name::toString() const noexcept {
  return NamePool::get().getName(*this);
}

constexpr hash_t Name::hash() const noexcept {
  return m_hash;
}

constexpr Name::operator hash_t() const noexcept {
  return m_hash;
}

constexpr Name::operator bool() const noexcept {
  return m_hash != 0;
}

constexpr Name::operator std::string_view() const noexcept {
  return toString();
}

constexpr bool Name::operator==(const Name& other) const noexcept {
  return m_hash == other.m_hash;
}

constexpr bool Name::operator!=(const Name& other) const noexcept {
  return m_hash != other.m_hash;
}
}  // namespace core
