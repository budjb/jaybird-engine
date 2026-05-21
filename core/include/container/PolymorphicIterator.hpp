#pragma once

#include <cstddef>
#include <iterator>

namespace core::container {
class PolymorphicIterator {
 public:
  using value_type = std::byte;
  using iterator_category = std::random_access_iterator_tag;
  using iterator_concept = std::random_access_iterator_tag;

  explicit PolymorphicIterator(void* data, const std::size_t elementSize) noexcept
      : m_data(data), m_elementSize(elementSize) {}

  [[nodiscard]] void* data() noexcept {
    return m_data;
  }

  [[nodiscard]] const void* data() const noexcept {
    return m_data;
  }

  [[nodiscard]] void* at(const std::size_t index) {
    return static_cast<std::byte*>(m_data) + index * m_elementSize;
  }

  [[nodiscard]] const void* at(const std::size_t index) const {
    return static_cast<const std::byte*>(m_data) + index * m_elementSize;
  }

  // Arithmetic operators
  PolymorphicIterator& operator++() noexcept {
    m_data = static_cast<std::byte*>(m_data) + m_elementSize;
    return *this;
  }

  PolymorphicIterator operator++(int) noexcept {
    PolymorphicIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  PolymorphicIterator& operator--() noexcept {
    m_data = static_cast<std::byte*>(m_data) - m_elementSize;
    return *this;
  }

  PolymorphicIterator operator--(int) noexcept {
    PolymorphicIterator tmp = *this;
    --(*this);
    return tmp;
  }

  PolymorphicIterator& operator+=(const std::ptrdiff_t offset) noexcept {
    m_data = static_cast<std::byte*>(m_data) + offset * static_cast<std::ptrdiff_t>(m_elementSize);
    return *this;
  }

  PolymorphicIterator& operator-=(const std::ptrdiff_t offset) noexcept {
    m_data = static_cast<std::byte*>(m_data) - offset * static_cast<std::ptrdiff_t>(m_elementSize);
    return *this;
  }

  [[nodiscard]] PolymorphicIterator operator+(const std::ptrdiff_t offset) const noexcept {
    PolymorphicIterator result = *this;
    result += offset;
    return result;
  }

  [[nodiscard]] PolymorphicIterator operator-(const std::ptrdiff_t offset) const noexcept {
    PolymorphicIterator result = *this;
    result -= offset;
    return result;
  }

  [[nodiscard]] std::ptrdiff_t operator-(const PolymorphicIterator& other) const noexcept {
    const auto* thisBytes = static_cast<const std::byte*>(m_data);
    const auto* otherBytes = static_cast<const std::byte*>(other.m_data);
    return (thisBytes - otherBytes) / static_cast<std::ptrdiff_t>(m_elementSize);
  }

  // Comparison operators
  [[nodiscard]] bool operator==(const PolymorphicIterator& other) const noexcept {
    return m_data == other.m_data;
  }

  [[nodiscard]] bool operator!=(const PolymorphicIterator& other) const noexcept {
    return m_data != other.m_data;
  }

  [[nodiscard]] bool operator<(const PolymorphicIterator& other) const noexcept {
    return m_data < other.m_data;
  }

  [[nodiscard]] bool operator>(const PolymorphicIterator& other) const noexcept {
    return m_data > other.m_data;
  }

  [[nodiscard]] bool operator<=(const PolymorphicIterator& other) const noexcept {
    return m_data <= other.m_data;
  }

  [[nodiscard]] bool operator>=(const PolymorphicIterator& other) const noexcept {
    return m_data >= other.m_data;
  }

 private:
  void* m_data;
  std::size_t m_elementSize;
};

/**
 * @brief A reverse iterator adapter for PolymorphicIterator, providing backward iteration through
 * a type-erased vector. This class mimics std::reverse_iterator but is adapted for the
 * type-erased semantics of PolymorphicIterator.
 *
 * ReverseIterator wraps a forward PolymorphicIterator and reverses its traversal direction.
 * When incremented, it decrements the underlying iterator, and vice versa.
 */
class PolymorphicReverseIterator {
 public:
  explicit PolymorphicReverseIterator(const PolymorphicIterator& base) noexcept : m_current(base) {}

  /**
   * @brief Gets the underlying forward iterator.
   *
   * @return The wrapped PolymorphicIterator.
   */
  [[nodiscard]] PolymorphicIterator base() const noexcept {
    return m_current;
  }

  /**
   * @brief Gets the data pointer of the current element.
   *
   * @return A void* pointing to the current element.
   */
  [[nodiscard]] void* data() noexcept {
    PolymorphicIterator result = m_current;
    --result;
    return result.data();
  }

  /**
   * @brief Gets the const data pointer of the current element.
   *
   * @return A const void* pointing to the current element.
   */
  [[nodiscard]] const void* data() const noexcept {
    PolymorphicIterator result = m_current;
    --result;
    return result.data();
  }

  /**
   * @brief Accesses element at offset (in reverse direction).
   *
   * @param index The index relative to current position (in reverse).
   * @return A void* to the element at that index.
   */
  [[nodiscard]] void* at(const std::size_t index) noexcept {
    PolymorphicIterator result = m_current - static_cast<std::ptrdiff_t>(index + 1);
    return result.data();
  }

  /**
   * @brief Const version of at().
   *
   * @param index The index relative to current position (in reverse).
   * @return A const void* to the element at that index.
   */
  [[nodiscard]] const void* at(const std::size_t index) const noexcept {
    PolymorphicIterator result = m_current - static_cast<std::ptrdiff_t>(index + 1);
    return result.data();
  }

  // Arithmetic operators (reversed semantics)
  /**
   * @brief Pre-increment moves backward (decrements underlying iterator).
   */
  PolymorphicReverseIterator& operator++() noexcept {
    --m_current;
    return *this;
  }

  /**
   * @brief Post-increment moves backward.
   */
  PolymorphicReverseIterator operator++(int) noexcept {
    PolymorphicReverseIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  /**
   * @brief Pre-decrement moves forward (increments underlying iterator).
   */
  PolymorphicReverseIterator& operator--() noexcept {
    ++m_current;
    return *this;
  }

  /**
   * @brief Post-decrement moves forward.
   */
  PolymorphicReverseIterator operator--(int) noexcept {
    PolymorphicReverseIterator tmp = *this;
    --(*this);
    return tmp;
  }

  /**
   * @brief Add offset in reverse direction (subtracts from underlying).
   */
  PolymorphicReverseIterator& operator+=(const std::ptrdiff_t offset) noexcept {
    m_current -= offset;
    return *this;
  }

  /**
   * @brief Subtract offset in reverse direction (adds to underlying).
   */
  PolymorphicReverseIterator& operator-=(const std::ptrdiff_t offset) noexcept {
    m_current += offset;
    return *this;
  }

  /**
   * @brief Create reverse iterator advanced by offset (in reverse).
   */
  [[nodiscard]] PolymorphicReverseIterator operator+(const std::ptrdiff_t offset) const noexcept {
    PolymorphicReverseIterator result = *this;
    result += offset;
    return result;
  }

  /**
   * @brief Create reverse iterator retreated by offset (in reverse).
   */
  [[nodiscard]] PolymorphicReverseIterator operator-(const std::ptrdiff_t offset) const noexcept {
    PolymorphicReverseIterator result = *this;
    result -= offset;
    return result;
  }

  /**
   * @brief Calculate distance between two reverse iterators (reversed semantics).
   */
  [[nodiscard]] std::ptrdiff_t operator-(const PolymorphicReverseIterator& other) const noexcept {
    return other.m_current - m_current;
  }

  // Comparison operators
  /**
   * @brief Equality comparison.
   */
  [[nodiscard]] bool operator==(const PolymorphicReverseIterator& other) const noexcept {
    return m_current == other.m_current;
  }

  /**
   * @brief Inequality comparison.
   */
  [[nodiscard]] bool operator!=(const PolymorphicReverseIterator& other) const noexcept {
    return m_current != other.m_current;
  }

  /**
   * @brief Less-than comparison (reversed semantics).
   */
  [[nodiscard]] bool operator<(const PolymorphicReverseIterator& other) const noexcept {
    return other.m_current < m_current;
  }

  /**
   * @brief Greater-than comparison (reversed semantics).
   */
  [[nodiscard]] bool operator>(const PolymorphicReverseIterator& other) const noexcept {
    return other.m_current > m_current;
  }

  /**
   * @brief Less-equal comparison (reversed semantics).
   */
  [[nodiscard]] bool operator<=(const PolymorphicReverseIterator& other) const noexcept {
    return other.m_current <= m_current;
  }

  /**
   * @brief Greater-equal comparison (reversed semantics).
   */
  [[nodiscard]] bool operator>=(const PolymorphicReverseIterator& other) const noexcept {
    return other.m_current >= m_current;
  }

 private:
  PolymorphicIterator m_current;
};
}  // namespace core::container
