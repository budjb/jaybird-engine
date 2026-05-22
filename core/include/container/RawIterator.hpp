#pragma once

#include <cstddef>

namespace core::container {
/**
 * @brief A polymorphic iterator for type-erased vectors, allowing iteration over elements without knowing their
 * specific type at compile time. This class provides a way to access and manipulate elements in a type-erased vector by
 * treating the underlying data as raw bytes and using pointer arithmetic based on the element size.
 *
 * This iterator does not follow the typical conventions of an STL iterator since it is not typed and therefore cannot
 * return references. In all cases, elements are accessed or returned through typeless pointers (i.e., @c void* ).
 * Arithmetic performed on the iterator is in terms of elements in the associated container, not raw bytes.
 */
class RawIterator {
 public:
  /**
   * @brief Constructs a PolymorphicIterator with the given data pointer and element size. The data pointer should point
   * to the current element in the type-erased vector, and the element size should be the size of each element in bytes.
   *
   * @param data A pointer to the current element in the type-erased vector.
   * @param elementSize The size of each element in the vector, in bytes. It is important that this value
   * accurately reflects the size of the elements in the vector, as incorrect values could lead to undefined behavior
   * when iterating through the vector.
   */
  explicit RawIterator(void* data, const std::size_t elementSize) noexcept
      : m_data(data), m_elementSize(elementSize) {}

  /**
   * @brief Gets the data pointer of the current element. This method returns a void* that points to the current element
   * in the type-erased vector. The caller can use this pointer to access or manipulate the element, but it is the
   * caller's responsibility to ensure that the pointer is used correctly based on the actual type of the element. The
   * iterator itself does not provide any type information, so the caller must have knowledge of the element type and
   * size to use this pointer safely.
   *
   * @return A void* pointing to the current element in the type-erased vector.
   */
  [[nodiscard]] void* data() noexcept {
    return m_data;
  }

  /**
   * @brief Gets the const data pointer of the current element. This method returns a const void* that points to the
   * current element in the type-erased vector. The caller can use this pointer to read the element, but it is the
   * caller's responsibility to ensure that the pointer is used correctly based on the actual type of the element. The
   * iterator itself does not provide any type information, so the caller must have knowledge of the element type and
   * size to use this pointer safely.
   *
   * @return A const void* pointing to the current element in the type-erased vector.
   */
  [[nodiscard]] const void* data() const noexcept {
    return m_data;
  }

  /**
   * @brief Accesses the element at the specified index relative to the current position of the iterator. This method
   * calculates the memory address of the element at the given index by performing pointer arithmetic based on the
   * current data pointer and the element size.
   *
   * @param index The index of the element to access, relative to the current position of the iterator.
   * @return A void* pointing to the element at the specified index relative to the current position of the iterator.
   */
  [[nodiscard]] void* at(const std::size_t index) {
    return static_cast<std::byte*>(m_data) + index * m_elementSize;
  }

  /**
   * @brief Const version of at(). This method accesses the element at the specified index relative to the current
   * position of the iterator, but it returns a const void* pointer.
   *
   * @param index The index of the element to access, relative to the current position of the iterator.
   * @return A const void* pointing to the element at the specified index relative to the current position of the
   * iterator.
   */
  [[nodiscard]] const void* at(const std::size_t index) const {
    return static_cast<const std::byte*>(m_data) + index * m_elementSize;
  }

  /**
   * @brief Pre-increment operator. This operator advances the iterator to the next element in the type-erased vector by
   * performing pointer arithmetic based on the element size.
   *
   * @return A reference to the updated iterator after advancing to the next element.
   */
  RawIterator& operator++() noexcept {
    m_data = static_cast<std::byte*>(m_data) + m_elementSize;
    return *this;
  }

  /**
   * @brief Post-increment operator. This operator advances the iterator to the next element in the type-erased vector,
   * but it returns a copy of the iterator before it was advanced.
   *
   * @return A copy of the iterator before it was advanced to the next element.
   */
  RawIterator operator++(int) noexcept {
    const RawIterator tmp = *this;
    ++*this;
    return tmp;
  }

  /**
   * @brief Pre-decrement operator. This operator moves the iterator to the previous element in the type-erased vector
   * by performing pointer arithmetic based on the element size.
   *
   * @return A reference to the updated iterator after moving to the previous element.
   */
  RawIterator& operator--() noexcept {
    m_data = static_cast<std::byte*>(m_data) - m_elementSize;
    return *this;
  }

  /**
   * @brief Post-decrement operator. This operator moves the iterator to the previous element in the type-erased vector,
   * but it returns a copy of the iterator before it was moved.
   *
   * @return A copy of the iterator before it was moved to the previous element.
   */
  RawIterator operator--(int) noexcept {
    const RawIterator tmp = *this;
    --*this;
    return tmp;
  }

  /**
   * @brief Adds an offset to the iterator, moving it forward by the specified number of elements. The offset is
   * multiplied by the element size to calculate the correct byte offset for pointer arithmetic.
   *
   * @param offset The number of elements to move the iterator forward. This value can be negative to move the iterator
   * backward.
   * @return A reference to the updated iterator after adding the offset.
   */
  RawIterator& operator+=(const std::ptrdiff_t offset) noexcept {
    m_data = static_cast<std::byte*>(m_data) + offset * static_cast<std::ptrdiff_t>(m_elementSize);
    return *this;
  }

  /**
   * @brief Subtracts an offset from the iterator, moving it backward by the specified number of elements. The offset is
   * multiplied by the element size to calculate the correct byte offset for pointer arithmetic.
   *
   * @param offset The number of elements to move the iterator backward. This value can be negative to move the iterator
   * forward.
   * @return A reference to the updated iterator after subtracting the offset.
   */
  RawIterator& operator-=(const std::ptrdiff_t offset) noexcept {
    m_data = static_cast<std::byte*>(m_data) - offset * static_cast<std::ptrdiff_t>(m_elementSize);
    return *this;
  }

  /**
   * @brief Creates a new iterator that is advanced by the specified offset. This operator does not modify the current
   * iterator but returns a new one that is moved forward by the given number of elements.
   *
   * @param offset The number of elements to move the new iterator forward. This value can be negative to move the new
   * iterator backward.
   * @return A new PolymorphicIterator that is advanced by the specified offset from the current iterator.
   */
  [[nodiscard]] RawIterator operator+(const std::ptrdiff_t offset) const noexcept {
    RawIterator result = *this;
    result += offset;
    return result;
  }

  /**
   * @brief Creates a new iterator that is retreated by the specified offset. This operator does not modify the current
   * iterator but returns a new one that is moved backward by the given number of elements.
   *
   * @param offset The number of elements to move the new iterator backward. This value can be negative to move the new
   * iterator forward.
   * @return A new PolymorphicIterator that is retreated by the specified offset from the current iterator.
   */
  [[nodiscard]] RawIterator operator-(const std::ptrdiff_t offset) const noexcept {
    RawIterator result = *this;
    result -= offset;
    return result;
  }

  /**
   * @brief Calculates the distance between two iterators in terms of the number of elements between them. This operator
   * performs pointer arithmetic based on the element size to determine how many elements separate the two iterators.
   *
   * @param other The other PolymorphicIterator to compare with. Both iterators should be associated with the same
   * type-erased vector and should have the same element size for this calculation to be meaningful.
   * @return The number of elements between the current iterator and the other iterator. A positive value indicates that
   * the current iterator is ahead of the other, while a negative value indicates that it is behind.
   */
  [[nodiscard]] std::ptrdiff_t operator-(const RawIterator& other) const noexcept {
    const auto* thisBytes = static_cast<const std::byte*>(m_data);
    const auto* otherBytes = static_cast<const std::byte*>(other.m_data);
    return (thisBytes - otherBytes) / static_cast<std::ptrdiff_t>(m_elementSize);
  }

  /**
   * @brief Equality comparison operator. This operator checks if two PolymorphicIterators are equal by comparing their
   * data pointers. For two iterators to be considered equal, they must point to the same element in the type-erased
   * vector (i.e., their data pointers must be the same).
   *
   * @param other The other PolymorphicIterator to compare with.
   * @return true if the two iterators are equal (point to the same element), false otherwise.
   */
  [[nodiscard]] bool operator==(const RawIterator& other) const noexcept {
    return m_data == other.m_data;
  }

  /**
   * @brief Inequality comparison operator. This operator checks if two PolymorphicIterators are not equal by comparing
   * their data pointers. For two iterators to be considered not equal, they must point to different elements in the
   * type-erased vector (i.e., their data pointers must be different).
   *
   * @param other The other PolymorphicIterator to compare with.
   * @return true if the two iterators are not equal (point to different elements), false otherwise.
   */
  [[nodiscard]] bool operator!=(const RawIterator& other) const noexcept {
    return m_data != other.m_data;
  }

  /**
   * @brief Less-than comparison operator. This operator checks if the current iterator is less than the other iterator
   * by comparing their data pointers. For this comparison to be meaningful, both iterators should be associated with
   * the same type-erased vector and should have the same element size. The operator returns true if the current
   * iterator points to an element that is located before the element pointed to by the other iterator in the memory
   * layout of the vector.
   *
   * @param other The other PolymorphicIterator to compare with. Both iterators should be associated with the same
   * type-erased vector and should have the same element size for this comparison to be meaningful.
   * @return true if the current iterator is less than the other iterator (i.e., points to an element that is located
   * before the element pointed to by the other iterator), false otherwise.
   */
  [[nodiscard]] bool operator<(const RawIterator& other) const noexcept {
    return m_data < other.m_data;
  }

  /**
   * @brief Greater-than comparison operator. This operator checks if the current iterator is greater than the other
   * iterator by comparing their data pointers. For this comparison to be meaningful, both iterators should be
   * associated with the same type-erased vector and should have the same element size. The operator returns true if the
   * current iterator points to an element that is located after the element pointed to by the other iterator in the
   * memory layout of the vector.
   *
   * @param other The other PolymorphicIterator to compare with. Both iterators should be associated with the same
   * type-erased vector and should have the same element size for this comparison to be meaningful.
   * @return true if the current iterator is greater than the other iterator (i.e., points to an element that is located
   * after the element pointed to by the other iterator), false otherwise.
   */
  [[nodiscard]] bool operator>(const RawIterator& other) const noexcept {
    return m_data > other.m_data;
  }

  /**
   * @brief Less-equal comparison operator. This operator checks if the current iterator is less than or equal to the
   * other iterator by comparing their data pointers. For this comparison to be meaningful, both iterators should be
   * associated with the same type-erased vector and should have the same element size. The operator returns true if the
   * current iterator points to an element that is located before or at the same position as the element pointed to by
   * the other iterator in the memory layout of the vector.
   *
   * @param other The other PolymorphicIterator to compare with. Both iterators should be associated with the same
   * type-erased vector and should have the same element size for this comparison to be meaningful.
   * @return true if the current iterator is less than or equal to the other iterator (i.e., points to an element that
   * is located before or at the same position as the element pointed to by the other iterator), false otherwise.
   */
  [[nodiscard]] bool operator<=(const RawIterator& other) const noexcept {
    return m_data <= other.m_data;
  }

  /**
   * @brief Greater-equal comparison operator. This operator checks if the current iterator is greater than or equal to
   * the other iterator by comparing their data pointers. For this comparison to be meaningful, both iterators should be
   * associated with the same type-erased vector and should have the same element size. The operator returns true if the
   * current iterator points to an element that is located after or at the same position as the element pointed to by
   * the other iterator in the memory layout of the vector.
   *
   * @param other The other PolymorphicIterator to compare with. Both iterators should be associated with the same
   * type-erased vector and should have the same element size for this comparison to be meaningful.
   * @return true if the current iterator is greater than or equal to the other iterator (i.e., points to an element
   * that is located after or at the same position as the element pointed to by the other iterator), false otherwise.
   */
  [[nodiscard]] bool operator>=(const RawIterator& other) const noexcept {
    return m_data >= other.m_data;
  }

 private:
  /**
   * @brief The data pointer for the current element in the type-erased vector. This pointer is treated as a void* and
   * is used to access the current element.
   */
  void* m_data;

  /**
   * @brief The size of each element in the type-erased vector, in bytes.
   */
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
  /**
   * @brief Constructs a PolymorphicReverseIterator from a given PolymorphicIterator.
   *
   * @param base The forward PolymorphicIterator to wrap.
   */
  explicit PolymorphicReverseIterator(const RawIterator& base) noexcept : m_current(base) {}

  /**
   * @brief Gets the underlying forward iterator.
   *
   * @return The wrapped PolymorphicIterator.
   */
  [[nodiscard]] RawIterator base() const noexcept {
    return m_current;
  }

  /**
   * @brief Gets the data pointer of the current element.
   *
   * @return A void* pointing to the current element.
   */
  [[nodiscard]] void* data() noexcept {
    RawIterator result = m_current;
    --result;
    return result.data();
  }

  /**
   * @brief Gets the const data pointer of the current element.
   *
   * @return A const void* pointing to the current element.
   */
  [[nodiscard]] const void* data() const noexcept {
    RawIterator result = m_current;
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
    RawIterator result = m_current - static_cast<std::ptrdiff_t>(index + 1);
    return result.data();
  }

  /**
   * @brief Const version of at().
   *
   * @param index The index relative to current position (in reverse).
   * @return A const void* to the element at that index.
   */
  [[nodiscard]] const void* at(const std::size_t index) const noexcept {
    RawIterator result = m_current - static_cast<std::ptrdiff_t>(index + 1);
    return result.data();
  }

  /**
   * @brief Pre-increment moves backward (decrements underlying iterator).
   *
   * @return A reference to the updated reverse iterator after moving backward.
   */
  PolymorphicReverseIterator& operator++() noexcept {
    --m_current;
    return *this;
  }

  /**
   * @brief Post-increment moves backward.
   *
   * @return A copy of the reverse iterator before it was moved backward.
   */
  PolymorphicReverseIterator operator++(int) noexcept {
    const PolymorphicReverseIterator tmp = *this;
    ++*this;
    return tmp;
  }

  /**
   * @brief Pre-decrement moves forward (increments underlying iterator).
   *
   * @return A reference to the updated reverse iterator after moving forward.
   */
  PolymorphicReverseIterator& operator--() noexcept {
    ++m_current;
    return *this;
  }

  /**
   * @brief Post-decrement moves forward.
   *
   * @return A copy of the reverse iterator before it was moved forward.
   */
  PolymorphicReverseIterator operator--(int) noexcept {
    const PolymorphicReverseIterator tmp = *this;
    --*this;
    return tmp;
  }

  /**
   * @brief Add offset in reverse direction (subtracts from underlying).
   *
   * @param offset The number of elements to move the reverse iterator forward (in reverse direction).
   * @return A reference to the updated reverse iterator after adding the offset.
   */
  PolymorphicReverseIterator& operator+=(const std::ptrdiff_t offset) noexcept {
    m_current -= offset;
    return *this;
  }

  /**
   * @brief Subtract offset in reverse direction (adds to underlying).
   *
   * @param offset The number of elements to move the reverse iterator backward (in reverse direction).
   * @return A reference to the updated reverse iterator after subtracting the offset.
   */
  PolymorphicReverseIterator& operator-=(const std::ptrdiff_t offset) noexcept {
    m_current += offset;
    return *this;
  }

  /**
   * @brief Create reverse iterator advanced by offset (in reverse).
   *
   * @param offset The number of elements to move the new reverse iterator forward (in reverse direction).
   * @return A new PolymorphicReverseIterator that is advanced by the specified offset from the current reverse
   * iterator.
   */
  [[nodiscard]] PolymorphicReverseIterator operator+(const std::ptrdiff_t offset) const noexcept {
    PolymorphicReverseIterator result = *this;
    result += offset;
    return result;
  }

  /**
   * @brief Create reverse iterator retreated by offset (in reverse).
   *
   * @param offset The number of elements to move the new reverse iterator backward (in reverse direction).
   * @return A new PolymorphicReverseIterator that is retreated by the specified offset from the current reverse
   * iterator.
   */
  [[nodiscard]] PolymorphicReverseIterator operator-(const std::ptrdiff_t offset) const noexcept {
    PolymorphicReverseIterator result = *this;
    result -= offset;
    return result;
  }

  /**
   * @brief Calculate distance between two reverse iterators (reversed semantics).
   *
   * @param other The other PolymorphicReverseIterator to compare with.
   * @return The number of elements between the current reverse iterator and the other reverse iterator, with reversed
   * semantics.
   */
  [[nodiscard]] std::ptrdiff_t operator-(const PolymorphicReverseIterator& other) const noexcept {
    return other.m_current - m_current;
  }

  /**
   * @brief Equality comparison.
   *
   * @param other The other PolymorphicReverseIterator to compare with.
   * @return true if the two reverse iterators are equal (point to the same element), false otherwise.
   */
  [[nodiscard]] bool operator==(const PolymorphicReverseIterator& other) const noexcept {
    return m_current == other.m_current;
  }

  /**
   * @brief Inequality comparison.
   *
   * @param other The other PolymorphicReverseIterator to compare with.
   * @return true if the two reverse iterators are not equal (point to different elements), false otherwise.
   */
  [[nodiscard]] bool operator!=(const PolymorphicReverseIterator& other) const noexcept {
    return m_current != other.m_current;
  }

  /**
   * @brief Less-than comparison (reversed semantics).
   *
   * @param other The other PolymorphicReverseIterator to compare with.
   * @return true if the current reverse iterator is less than the other reverse iterator (i.e., points to an element
   * that is located after the element pointed to by the other reverse iterator), false otherwise.
   */
  [[nodiscard]] bool operator<(const PolymorphicReverseIterator& other) const noexcept {
    return other.m_current < m_current;
  }

  /**
   * @brief Greater-than comparison (reversed semantics).
   *
   * @param other The other PolymorphicReverseIterator to compare with.
   * @return true if the current reverse iterator is greater than the other reverse iterator (i.e., points to an element
   * that is located before the element pointed to by the other reverse iterator), false otherwise.
   */
  [[nodiscard]] bool operator>(const PolymorphicReverseIterator& other) const noexcept {
    return other.m_current > m_current;
  }

  /**
   * @brief Less-equal comparison (reversed semantics).
   *
   * @param other The other PolymorphicReverseIterator to compare with.
   * @return true if the current reverse iterator is less than or equal to the other reverse iterator (i.e., points to
   * an element that is located after or at the same position as the element pointed to by the other reverse iterator),
   * false otherwise.
   */
  [[nodiscard]] bool operator<=(const PolymorphicReverseIterator& other) const noexcept {
    return other.m_current <= m_current;
  }

  /**
   * @brief Greater-equal comparison (reversed semantics).
   *
   * @param other The other PolymorphicReverseIterator to compare with.
   * @return true if the current reverse iterator is greater than or equal to the other reverse iterator (i.e., points
   * to an element that is located before or at the same position as the element pointed to by the other reverse
   * iterator), false otherwise.
   */
  [[nodiscard]] bool operator>=(const PolymorphicReverseIterator& other) const noexcept {
    return other.m_current >= m_current;
  }

 private:
  /**
   * @brief The underlying forward iterator that this reverse iterator wraps. The reverse iterator's operations will
   * manipulate this forward iterator to achieve reverse traversal through the type-erased vector.
   */
  RawIterator m_current;
};
}  // namespace core::container
