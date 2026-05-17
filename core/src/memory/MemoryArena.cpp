#include "memory/MemoryArena.hpp"

namespace core::memory {
MemoryArena::MemoryArena(const std::size_t size) noexcept
    : m_size(size), m_data(new std::byte[m_size]), m_cursor(reinterpret_cast<uintptr_t>(m_data)) {}

MemoryArena::~MemoryArena() noexcept {
  delete[] m_data;
}

void* MemoryArena::allocate(const std::size_t size, const std::size_t alignment) noexcept {
  const auto current_address = m_cursor;
  const auto offset = (alignment - current_address % alignment) % alignment;
  const auto aligned_address = current_address + offset;
  const auto new_cursor = aligned_address + size;

  if (new_cursor > reinterpret_cast<uintptr_t>(m_data) + m_size) {
    return nullptr;
  }

  m_cursor = new_cursor;
  return reinterpret_cast<void*>(aligned_address);
}

void MemoryArena::clear() noexcept {
  m_cursor = reinterpret_cast<uintptr_t>(m_data);
}
}  // namespace core::memory
