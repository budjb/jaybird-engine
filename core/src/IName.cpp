#include "IName.hpp"

#include "INamePool.hpp"

namespace core {
constexpr IName::IName(const hash_t hash) noexcept : m_hash(hash) {}

constexpr IName::IName(const std::string_view str) noexcept : IName(fnv1a_64(str)) {}

IName::IName(const std::string& str) noexcept : IName(fnv1a_64(str)) {}

constexpr IName::IName(const char* str) noexcept : IName(fnv1a_64(str)) {}

constexpr bool IName::empty() const noexcept {
  return m_hash == 0;
}

std::string_view IName::toString() const noexcept {
  return INamePool::get().getName(*this);
}

constexpr hash_t IName::hash() const noexcept {
  return m_hash;
}

constexpr IName::operator hash_t() const noexcept {
  return m_hash;
}

constexpr IName::operator bool() const noexcept {
  return m_hash != 0;
}

constexpr IName::operator std::string_view() const noexcept {
  return toString();
}

constexpr bool IName::operator==(const IName& other) const noexcept {
  return m_hash == other.m_hash;
}

constexpr bool IName::operator!=(const IName& other) const noexcept {
  return m_hash != other.m_hash;
}
}  // namespace core
