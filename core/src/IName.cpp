#include "IName.hpp"

#include "INamePool.hpp"

namespace core {
IName::IName(const hash_t hash) noexcept : m_hash(hash) {}

IName::IName(const std::string_view str) noexcept : IName(fnv1a_64(str)) {}

IName::IName(const std::string& str) noexcept : IName(fnv1a_64(str)) {}

IName::IName(const char* str) noexcept : IName(fnv1a_64(str)) {}

bool IName::empty() const noexcept {
  return m_hash == 0;
}

std::string_view IName::toString() const noexcept {
  return INamePool::get().getName(*this);
}

hash_t IName::hash() const noexcept {
  return m_hash;
}

IName::operator hash_t() const noexcept {
  return m_hash;
}

IName::operator bool() const noexcept {
  return m_hash != 0;
}

IName::operator std::string_view() const noexcept {
  return toString();
}

bool IName::operator==(const IName& other) const noexcept {
  return m_hash == other.m_hash;
}

bool IName::operator!=(const IName& other) const noexcept {
  return m_hash != other.m_hash;
}
}  // namespace core
