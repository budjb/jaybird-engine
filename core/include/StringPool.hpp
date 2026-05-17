#pragma once

#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "Hash.hpp"
#include "IString.hpp"

namespace core {
class StringPool {
 public:
  static StringPool& get();

  IString addString(std::string_view str) noexcept;

  [[nodiscard]] std::string_view getString(const IString& str) const noexcept;

  [[nodiscard]] std::string_view getString(hash_t hash) const noexcept;

  bool hasString(const IString& str) const noexcept;

 private:
  StringPool() = default;

  mutable std::shared_mutex m_mutex;
  std::unordered_map<IString, std::string> m_strings;
};
}  // namespace core
