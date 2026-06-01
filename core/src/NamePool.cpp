#include "NamePool.hpp"

namespace core {
NamePool& NamePool::get() {
  static NamePool instance;
  return instance;
}

Name NamePool::addName(const std::string_view str) noexcept {
  std::unique_lock lock(m_mutex);
  const auto iStr = Name(str);

  if (!m_names.contains(iStr)) {
    m_names.emplace(iStr, std::string(str));
  }

  return iStr;
}

std::string_view NamePool::getName(const Name& str) const noexcept {
  std::shared_lock lock(m_mutex);

  if (const auto it = m_names.find(str); it != m_names.end()) {
    return it->second;
  }

  return {};
}

std::string_view NamePool::getName(const hash_t hash) const noexcept {
  return getName(Name(hash));
}

bool NamePool::hasName(const Name& str) const noexcept {
  std::shared_lock lock(m_mutex);
  return m_names.contains(str);
}
}  // namespace core
