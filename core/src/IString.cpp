#include "IString.hpp"
#include "StringPool.hpp"

namespace core {
IString::IString(const hash_t hash) noexcept : m_hash(hash) {
}

IString::IString(const std::string_view str) noexcept
  : IString(fnv1a_64(str)) {
}

std::string_view IString::toString() const noexcept {
  return StringPool::get().getString(*this);
}

hash_t IString::hash() const noexcept {
  return m_hash;
}

IString::operator unsigned long long() const noexcept {
  return m_hash;
}

IString::operator bool() const noexcept {
  return m_hash != 0;
}

IString::operator std::string_view() const noexcept {
  return toString();
}

bool IString::operator==(const IString& other) const noexcept {
  return m_hash == other.m_hash;
}
} // core