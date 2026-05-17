#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace core {
using hash_t = std::uint64_t;

class IString {
 public:
  IString() = default;

  explicit IString(hash_t hash) noexcept;

  explicit IString(std::string_view str) noexcept;

  [[nodiscard]] std::string_view toString() const noexcept;

  [[nodiscard]] hash_t hash() const noexcept;

  operator hash_t() const noexcept;

  operator bool() const noexcept;

  operator std::string_view() const noexcept;

  bool operator==(const IString& other) const noexcept;

 private:
  const hash_t m_hash = 0;
};
}  // namespace core

template <>
struct std::hash<core::IString> {
  std::size_t operator()(const core::IString& s) const noexcept { return std::hash<core::hash_t>{}(s.hash()); }
};
