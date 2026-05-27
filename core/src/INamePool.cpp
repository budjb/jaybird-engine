#include "INamePool.hpp"

namespace core {
INamePool& INamePool::get() {
  static INamePool instance;
  return instance;
}

IName INamePool::addName(const std::string_view str) noexcept {
  std::unique_lock lock(m_mutex);
  const auto iStr = IName(str);

  if (!m_names.contains(iStr)) {
    m_names.emplace(iStr, std::string(str));
  }

  return iStr;
}

std::string_view INamePool::getName(const IName& str) const noexcept {
  std::shared_lock lock(m_mutex);

  if (const auto it = m_names.find(str); it != m_names.end()) {
    return it->second;
  }

  return {};
}

std::string_view INamePool::getName(const hash_t hash) const noexcept {
  return getName(IName(hash));
}

bool INamePool::hasName(const IName& str) const noexcept {
  std::shared_lock lock(m_mutex);
  return m_names.contains(str);
}
}  // namespace core
