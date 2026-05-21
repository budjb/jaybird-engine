#pragma once

#include "IVector.hpp"

namespace core::container {
class VectorModifier {
 public:
  explicit VectorModifier(IVector& other) noexcept : m_vector(other) {}

  [[nodiscard]] void* data() noexcept {
    return m_vector.m_data;
  }

  [[nodiscard]] const void* data() const noexcept {
    return m_vector.m_data;
  }

  [[nodiscard]] std::size_t size() const noexcept {
    return m_vector.size();
  }

  [[nodiscard]] std::size_t capacity() const noexcept {
    return m_vector.capacity();
  }

  [[nodiscard]] bool empty() const noexcept {
    return m_vector.empty() == 0;
  }

  [[nodiscard]] void* at(const std::size_t index) {
    if (index >= m_vector.m_size) {
      throw std::out_of_range("Index out of range");
    }
    return static_cast<std::byte*>(m_vector.m_data) + index * m_vector.m_elementSize;
  }

  [[nodiscard]] const void* at(const std::size_t index) const {
    if (index >= m_vector.m_size) {
      throw std::out_of_range("Index out of range");
    }
    return static_cast<const std::byte*>(m_vector.m_data) + index * m_vector.m_elementSize;
  }

  void resize(const std::size_t newSize) {
    m_vector.resize(newSize);
  }

  void* operator[](const std::size_t index) noexcept {
    return at(index);
  }

  const void* operator[](const std::size_t index) const noexcept {
    return at(index);
  }

 private:
  IVector& m_vector;
};
}  // namespace core::container
