#include "memory/Buffer.hpp"

#include <format>
#include <utility>

namespace core::memory {
Buffer::Buffer(const std::size_t size) noexcept : m_size(size), m_data(new std::byte[size]) {}

Buffer::Buffer(Buffer&& other) noexcept : m_size(other.m_size), m_data(std::exchange(other.m_data, nullptr)) {}

Buffer::~Buffer() noexcept {
  delete[] m_data;
}

void* Buffer::at(const std::size_t offset) const {
  if (offset >= m_size) {
    throw std::out_of_range(
        std::format("core::memory:Buffer::at(): offset {} out of range (max {})", offset, m_size - 1));
  }
  return m_data + offset;
}

std::size_t Buffer::size() const noexcept {
  return m_size;
}

bool Buffer::contains(void* ptr) const noexcept {
  if (!m_data) {
    return false;
  }

  const auto start = reinterpret_cast<uintptr_t>(m_data);
  const auto end = start + m_size;
  const auto address = reinterpret_cast<uintptr_t>(ptr);
  return address >= start && address < end;
}
}  // namespace core::memory
