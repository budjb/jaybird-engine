#include "StringPool.hpp"

namespace core {
StringPool& StringPool::get() {
  static StringPool instance;
  return instance;
}

IString StringPool::addString(const std::string_view str) noexcept {
  std::unique_lock lock(m_mutex);
  const auto iStr = IString(str);

  if (!m_strings.contains(iStr)) {
    m_strings.emplace(iStr, std::string(str));
  }

  return iStr;
}

std::string_view StringPool::getString(const IString& str) const noexcept {
  std::shared_lock lock(m_mutex);

  return m_strings.at(str);
}

std::string_view StringPool::getString(const hash_t hash) const noexcept {
  return getString(IString(hash));
}

bool StringPool::hasString(const IString& str) const noexcept {
  std::shared_lock lock(m_mutex);
  return m_strings.contains(str);
}
}  // namespace core
