#pragma once

#include <cstddef>

namespace core::rtti {
/**
 * @brief A template class representing an iterator for type-erased RTTI arrays.
 *
 * This iterator provides standard operations (increment, decrement, arithmetic) while handling the case where the
 * underlying type is @c void (unknown at compile time). For void types, the iterator uses the inner type's size from
 * the array descriptor for pointer arithmetic. It attempts to provide an STL-like API contract but uses type-erased
 * pointers, which may differ from STL iterators (for example, by returning pointers instead of references).
 *
 * @tparam T The type of the elements in the array.
 */
template <typename T = void>
class RTTIIterator {
 public:
  /**
   * @brief Constructs an Iterator for the given array type and pointer.
   *
   * @param elementSize The size in bytes of one element for type-erased iteration (`T=void`).
   * @param ptr A pointer to the current element in the array.
   */
  explicit RTTIIterator(const std::size_t elementSize, T* ptr) noexcept : m_elementSize(elementSize), m_ptr(ptr) {}

  /**
   * @brief Returns a pointer to the current element in the array, cast to the specified type.
   *
   * @return A pointer to the current element in the array, cast to the specified type.
   */
  [[nodiscard]] T* base() const noexcept {
    return static_cast<std::byte*>(m_ptr);
  }

  /**
   * @brief Allows implicit conversion of the iterator to a pointer to the current element in the array, cast to the
   * specified type.
   *
   * @return A pointer to the current element in the array, cast to the specified type.
   */
  operator T*() const noexcept {
    return m_ptr;
  }

  /**
   * @brief Returns a pointer to the current element in the array, cast to the specified type.
   *
   * @return A pointer to the current element in the array, cast to the specified type.
   */
  T* get() const noexcept {
    return static_cast<T*>(m_ptr);
  }

  /**
   * @brief Pre-increment operator that advances the iterator to the next element in the array.
   *
   * @return A reference to the iterator after it has been advanced to the next element in the array.
   */
  RTTIIterator& operator++() noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) + m_elementSize;
    } else {
      m_ptr = static_cast<T*>(m_ptr) + 1;
    }
    return *this;
  }

  /**
   * @brief Post-increment operator that advances the iterator to the next element in the array, but returns a copy of
   * the iterator before it was advanced.
   *
   * @return An iterator pointing to the current element in the array before the increment operation was performed.
   */
  RTTIIterator operator++(int) noexcept {
    const RTTIIterator tmp(*this);
    ++*this;
    return tmp;
  }

  /**
   * @brief Pre-decrement operator that moves the iterator to the previous element in the array.
   *
   * @return A reference to the iterator after it has been moved to the previous element in the array.
   */
  RTTIIterator& operator--() noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) - m_elementSize;
    } else {
      m_ptr = static_cast<T*>(m_ptr) - 1;
    }
    return *this;
  }

  /**
   * @brief Post-decrement operator that moves the iterator to the previous element in the array, but returns a copy of
   * the iterator before it was moved.
   *
   * @return An iterator pointing to the current element in the array before the decrement operation was performed.
   */
  RTTIIterator operator--(int) noexcept {
    const RTTIIterator tmp(*this);
    --*this;
    return tmp;
  }

  /**
   * @brief Adds the specified offset to the iterator, advancing it by the given number of elements in the array.
   *
   * @param offset The number of elements by which to advance the iterator. This can be positive to move forward or
   * negative to move backward in the array.
   * @return A reference to the iterator after it has been advanced by the specified offset.
   */
  RTTIIterator& operator+=(const std::ptrdiff_t offset) noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) + m_elementSize * offset;
    } else {
      m_ptr = static_cast<T*>(m_ptr) + offset;
    }
    return *this;
  }

  /**
   * @brief Subtracts the specified offset from the iterator, moving it backward by the given number of elements in the
   * array.
   *
   * @param offset The number of elements by which to move the iterator backward. This can be positive to move backward
   * or negative to move forward in the array.
   * @return A reference to the iterator after it has been moved backward by the specified offset.
   */
  RTTIIterator& operator-=(const std::ptrdiff_t offset) noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) - m_elementSize * offset;
    } else {
      m_ptr = static_cast<T*>(m_ptr) - offset;
    }
    return *this;
  }

  /**
   * @brief Returns a new iterator that is advanced by the specified offset from the current iterator position. The
   * original iterator remains unchanged.
   *
   * @param offset The number of elements by which to advance the new iterator. This can be positive to move forward or
   * negative to move backward in the array.
   * @return A new iterator that is advanced by the specified offset from the current iterator position.
   */
  RTTIIterator operator+(const std::ptrdiff_t offset) const noexcept {
    RTTIIterator tmp(*this);
    tmp += offset;
    return tmp;
  }

  /**
   * @brief Returns a new iterator that is moved backward by the specified offset from the current iterator position.
   * The original iterator remains unchanged.
   *
   * @param offset The number of elements by which to move the new iterator backward. This can be positive to move
   * backward or negative to move forward in the array.
   * @return A new iterator that is moved backward by the specified offset from the current iterator position.
   */
  RTTIIterator operator-(const std::ptrdiff_t offset) const noexcept {
    RTTIIterator tmp(*this);
    tmp -= offset;
    return tmp;
  }

  /**
   * @brief Returns the distance between this iterator and another iterator, measured in the number of elements between
   * them. The distance is positive if this iterator is ahead of the other iterator, negative if it is behind, and zero
   * if they are at the same position.
   *
   * @param iterator The other iterator to which the distance is calculated. Both iterators must be iterating over the
   * same array type and should be comparable.
   * @return The distance between this iterator and the other iterator, measured in the number of elements between them.
   */
  std::ptrdiff_t operator-(const RTTIIterator& iterator) const noexcept {
    if (m_ptr == iterator.m_ptr) {
      return 0;
    }

    if constexpr (std::is_void_v<T>) {
      const auto size = static_cast<std::ptrdiff_t>(m_elementSize);
      return (static_cast<std::byte*>(m_ptr) - static_cast<std::byte*>(iterator.m_ptr)) / size;
    } else {
      return static_cast<T*>(m_ptr) - static_cast<T*>(iterator.m_ptr);
    }
  }

  /**
   * @brief Compares this iterator with another iterator for equality.
   *
   * @param iterator The other iterator to compare with this iterator.
   * @return true if both iterators point to the same position in the array, false otherwise.
   */
  bool operator==(const RTTIIterator& iterator) const noexcept {
    return m_ptr == iterator.m_ptr;
  }

  /**
   * @brief Compares this iterator with another iterator for inequality.
   *
   * @param iterator The other iterator to compare with this iterator.
   * @return true if the iterators point to different positions in the array, false if they point to the same position.
   */
  bool operator!=(const RTTIIterator& iterator) const noexcept {
    return m_ptr != iterator.m_ptr;
  }

  /**
   * @brief Compares this iterator with another iterator to determine if this iterator points to a position that is
   * before the position pointed to by the other iterator in the array.
   *
   * @param iterator The other iterator to compare with this iterator. Both iterators must be iterating over the same
   * array type and should be comparable.
   * @return true if this iterator points to a position that is before the position pointed to by the other iterator in
   * the array, false otherwise.
   */
  bool operator<(const RTTIIterator& iterator) const noexcept {
    return m_ptr < iterator.m_ptr;
  }

  /**
   * @brief Compares this iterator with another iterator to determine if this iterator points to a position that is
   * before or the same as the position pointed to by the other iterator in the array.
   *
   * @param iterator The other iterator to compare with this iterator. Both iterators must be iterating over the same
   * array type and should be comparable.
   * @return true if this iterator points to a position that is before or the same as the position pointed to by the
   * other iterator in the array, false otherwise.
   */
  bool operator<=(const RTTIIterator& iterator) const noexcept {
    return m_ptr <= iterator.m_ptr;
  }

  /**
   * @brief Compares this iterator with another iterator to determine if this iterator points to a position that is
   * after the position pointed to by the other iterator in the array.
   *
   * @param iterator The other iterator to compare with this iterator. Both iterators must be iterating over the same
   * array type and should be comparable.
   * @return true if this iterator points to a position that is after the position pointed to by the other iterator in
   * the array, false otherwise.
   */
  bool operator>(const RTTIIterator& iterator) const noexcept {
    return m_ptr > iterator.m_ptr;
  }

  /**
   * @brief Compares this iterator with another iterator to determine if this iterator points to a position that is
   * after or the same as the position pointed to by the other iterator in the array.
   *
   * @param iterator The other iterator to compare with this iterator. Both iterators must be iterating over the same
   * array type and should be comparable.
   * @return true if this iterator points to a position that is after or the same as the position pointed to by the
   * other iterator in the array, false otherwise.
   */
  bool operator>=(const RTTIIterator& iterator) const noexcept {
    return m_ptr >= iterator.m_ptr;
  }

  /**
   * @brief Returns a pointer to the element at the specified index from the current iterator position. The index is
   * relative to the current position of the iterator, meaning that an index of 0 returns a pointer to the current
   * element, an index of 1 returns a pointer to the next element, and so on.
   *
   * @param index The index of the element to access, relative to the current position of the iterator. This can be
   * positive to access elements ahead of the current position or negative to access elements behind the current
   * position in the array.
   * @return A pointer to the element at the specified index from the current iterator position, cast to the specified
   * type.
   */
  void* operator[](const std::ptrdiff_t index) const noexcept {
    if constexpr (std::is_void_v<T>) {
      return static_cast<std::byte*>(m_ptr) + index * m_elementSize;
    } else {
      return static_cast<T*>(m_ptr) + index;
    }
  }

  /**
   * @brief Returns a new iterator that is advanced by the specified offset from the given iterator position. The
   * original iterator remains unchanged. This operator allows for commutative addition, meaning that the offset can be
   * on either side of the operator.
   *
   * @param offset The number of elements by which to advance the new iterator. This can be positive to move forward or
   * negative to move backward in the array.
   * @param iterator The iterator from which to calculate the new iterator position. The new iterator will be advanced
   * by the specified offset from this iterator's position.
   * @return A new iterator that is advanced by the specified offset from the given iterator position.
   */
  friend RTTIIterator operator+(const std::ptrdiff_t offset, const RTTIIterator& iterator) noexcept {
    RTTIIterator tmp(iterator);
    tmp += offset;
    return tmp;
  }

  /**
   * @brief Returns a new iterator that is moved backward by the specified offset from the given iterator position. The
   * original iterator remains unchanged. This operator allows for commutative subtraction, meaning that the offset can
   * be on either side of the operator.
   *
   * @param offset The number of elements by which to move the new iterator backward. This can be positive to move
   * backward or negative to move forward in the array.
   * @param iterator The iterator from which to calculate the new iterator position. The new iterator will be moved
   * backward by the specified offset from this iterator's position.
   * @return A new iterator that is moved backward by the specified offset from the given iterator position.
   */
  friend RTTIIterator operator-(const std::ptrdiff_t offset, const RTTIIterator& iterator) noexcept {
    RTTIIterator tmp(iterator);
    tmp -= offset;
    return tmp;
  }

 private:
  /**
   * @brief Element size in bytes used by the type-erased (`T=void`) iterator path.
   */
  std::size_t m_elementSize;

  /**
   * @brief A pointer to the current element in the array. This is the primary data member of the iterator that tracks
   * its position within the array. The type of this pointer is void* to allow for type-erased access to array elements,
   * and the actual type of the elements is determined by the template parameter T.
   *
   * When T is void, the iterator uses the size of the inner type from the array type descriptor to perform pointer
   * arithmetic correctly, treating the pointer as a byte pointer for arithmetic operations.
   */
  void* m_ptr;
};

/**
 * @brief A template class representing a reverse iterator for type-erased RTTI arrays.
 *
 * This reverse iterator provides standard operations (increment, decrement, arithmetic) while handling the case
 * where the underlying type is @c void. For void types, it uses the inner type's size from the array descriptor for
 * pointer arithmetic. It wraps a regular @code Iterator@endcode.
 *
 * @tparam T The type of the elements in the array.
 */
template <typename T = void>
class ReverseIterator {
 public:
  /**
   * @brief Constructs a ReverseIterator from a regular @code Iterator@endcode.
   *
   * The reverse iterator points to the element before the given iterator, following standard C++ reverse iterator
   * behavior.
   *
   * @param iterator The regular iterator from which to construct the reverse iterator.
   */
  explicit ReverseIterator(RTTIIterator<T> iterator) noexcept : m_iterator(iterator) {}

  /**
   * @brief Returns a pointer to the current element in the array that the reverse iterator points to, cast to the
   * specified type. The reverse iterator points to the element before the position of the underlying regular iterator,
   * so this function returns a pointer to that element.
   *
   * @return A pointer to the current element in the array that the reverse iterator points to, cast to the specified
   * type.
   */
  [[nodiscard]] T* base() const noexcept {
    return m_iterator.base();
  }

  /**
   * @brief Allows implicit conversion of the reverse iterator to a pointer to the current element in the array that the
   * reverse iterator points to, cast to the specified type.
   */
  operator T*() const noexcept {
    return get();
  }

  /**
   * @brief Returns a pointer to the current element in the array that the reverse iterator points to, cast to the
   * specified type. The reverse iterator points to the element before the position of the underlying regular iterator,
   * so this function returns a pointer to that element.
   *
   * @return A pointer to the current element in the array that the reverse iterator points to, cast to the specified
   * type.
   */
  T* get() const noexcept {
    return m_iterator - 1;
  }

  /**
   * @brief Pre-increment operator that moves the reverse iterator to the previous element in the array (which is the
   * next element in the reverse direction).
   *
   * @return A reference to the reverse iterator after it has been moved to the previous element in the array.
   */
  ReverseIterator& operator++() noexcept {
    --m_iterator;
    return *this;
  }

  /**
   * @brief Post-increment operator that moves the reverse iterator to the previous element in the array (which is the
   * next element in the reverse direction), but returns a copy of the reverse iterator before it was moved.
   *
   * @return An iterator pointing to the current element in the array before the increment operation was performed.
   */
  ReverseIterator operator++(int) noexcept {
    const ReverseIterator tmp(*this);
    ++*this;
    return tmp;
  }

  /**
   * @brief Pre-decrement operator that moves the reverse iterator to the next element in the array (which is the
   * previous element in the reverse direction).
   *
   * @return A reference to the reverse iterator after it has been moved to the next element in the array.
   */
  ReverseIterator& operator--() noexcept {
    ++m_iterator;
    return *this;
  }

  /**
   * @brief Post-decrement operator that moves the reverse iterator to the next element in the array (which is the
   * previous element in the reverse direction), but returns a copy of the reverse iterator before it was moved.
   *
   * @return An iterator pointing to the current element in the array before the decrement operation was performed.
   */
  ReverseIterator operator--(int) noexcept {
    const ReverseIterator tmp(*this);
    --*this;
    return tmp;
  }

  /**
   * @brief Adds the specified offset to the reverse iterator, moving it backward by the given number of elements in the
   * array (which is the next position in the reverse direction).
   *
   * @param offset The number of elements by which to move the reverse iterator backward. This can be positive to move
   * backward or negative to move forward in the array (which is the previous position in the reverse direction).
   * @return A reference to the reverse iterator after it has been moved backward by the specified offset.
   */
  ReverseIterator& operator+=(const std::ptrdiff_t offset) noexcept {
    m_iterator -= offset;
    return *this;
  }

  /**
   * @brief Subtracts the specified offset from the reverse iterator, moving it forward by the given number of elements
   * in the array (which is the previous position in the reverse direction).
   *
   * @param offset The number of elements by which to move the reverse iterator forward. This can be positive to move
   * forward or negative to move backward in the array (which is the next position in the reverse direction).
   * @return A reference to the reverse iterator after it has been moved forward by the specified offset.
   */
  ReverseIterator& operator-=(const std::ptrdiff_t offset) noexcept {
    m_iterator += offset;
    return *this;
  }

  /**
   * @brief Returns a new reverse iterator that is moved backward by the specified offset from the current reverse
   * iterator position (which is the next position in the reverse direction). The original reverse iterator remains
   * unchanged.
   *
   * @param offset The number of elements by which to move the new reverse iterator backward. This can be positive to
   * move backward or negative to move forward in the array (which is the previous position in the reverse direction).
   * @return A new reverse iterator that is moved backward by the specified offset from the current reverse iterator
   * position.
   */
  ReverseIterator operator+(const std::ptrdiff_t offset) const noexcept {
    ReverseIterator tmp(*this);
    tmp += offset;
    return tmp;
  }

  /**
   * @brief Returns a new reverse iterator that is moved forward by the specified offset from the current reverse
   * iterator position (which is the previous position in the reverse direction). The original reverse iterator remains
   * unchanged.
   *
   * @param offset The number of elements by which to move the new reverse iterator forward. This can be positive to
   * move forward or negative to move backward in the array (which is the next position in the reverse direction).
   * @return A new reverse iterator that is moved forward by the specified offset from the current reverse iterator
   * position.
   */
  ReverseIterator operator-(const std::ptrdiff_t offset) const noexcept {
    ReverseIterator tmp(*this);
    tmp -= offset;
    return tmp;
  }

  /**
   * @brief Returns the distance between this reverse iterator and another reverse iterator, measured in the number of
   * elements between them. The distance is positive if this reverse iterator is ahead of the other reverse iterator in
   * the reverse direction, negative if it is behind, and zero if they are at the same position.
   *
   * @param iterator The other reverse iterator to which the distance is calculated. Both reverse iterators must be
   * iterating over the same array type and should be comparable.
   * @return The distance between this reverse iterator and the other reverse iterator, measured in the number of
   * elements between them.
   */
  std::ptrdiff_t operator-(const ReverseIterator& iterator) const noexcept {
    return iterator.m_iterator - m_iterator;
  }

  /**
   * @brief Compares this reverse iterator with another reverse iterator for equality. Two reverse iterators are
   * considered equal if they point to the same position in the array (i.e., if their underlying regular iterators point
   * to the same position).
   *
   * @param iterator The other reverse iterator to compare with this reverse iterator.
   * @return true if both reverse iterators point to the same position in the array, false otherwise.
   */
  bool operator==(const ReverseIterator& iterator) const noexcept {
    return m_iterator == iterator.m_iterator;
  }

  /**
   * @brief Compares this reverse iterator with another reverse iterator for inequality. Two reverse iterators are
   * considered not equal if they point to different positions in the array (i.e., if their underlying regular iterators
   * point to different positions).
   *
   * @param iterator The other reverse iterator to compare with this reverse iterator.
   * @return true if the reverse iterators point to different positions in the array, false if they point to the same
   * position.
   */
  bool operator!=(const ReverseIterator& iterator) const noexcept {
    return m_iterator != iterator.m_iterator;
  }

  /**
   * @brief Compares this reverse iterator with another reverse iterator to determine if this reverse iterator points to
   * a position that is before the position pointed to by the other reverse iterator in the reverse direction.
   *
   * @param iterator The other reverse iterator to compare with this reverse iterator. Both reverse iterators must be
   * iterating over the same array type and should be comparable.
   * @return true if this reverse iterator points to a position that is before the position pointed to by the other
   * reverse iterator in the reverse direction, false otherwise.
   */
  bool operator<(const ReverseIterator& iterator) const noexcept {
    return m_iterator > iterator.m_iterator;
  }

  /**
   * @brief Compares this reverse iterator with another reverse iterator to determine if this reverse iterator points to
   * a position that is before or the same as the position pointed to by the other reverse iterator in the reverse
   * direction.
   *
   * @param iterator The other reverse iterator to compare with this reverse iterator. Both reverse iterators must be
   * iterating over the same array type and should be comparable.
   * @return true if this reverse iterator points to a position that is before or the same as the position pointed to by
   * the other reverse iterator in the reverse direction, false otherwise.
   */
  bool operator<=(const ReverseIterator& iterator) const noexcept {
    return m_iterator >= iterator.m_iterator;
  }

  /**
   * @brief Compares this reverse iterator with another reverse iterator to determine if this reverse iterator points to
   * a position that is after the position pointed to by the other reverse iterator in the reverse direction.
   *
   * @param iterator The other reverse iterator to compare with this reverse iterator. Both reverse iterators must be
   * iterating over the same array type and should be comparable.
   * @return true if this reverse iterator points to a position that is after the position pointed to by the other
   * reverse iterator in the reverse direction, false otherwise.
   */
  bool operator>(const ReverseIterator& iterator) const noexcept {
    return m_iterator < iterator.m_iterator;
  }

  /**
   * @brief Compares this reverse iterator with another reverse iterator to determine if this reverse iterator points to
   * a position that is after or the same as the position pointed to by the other reverse iterator in the reverse
   * direction.
   *
   * @param iterator The other reverse iterator to compare with this reverse iterator. Both reverse iterators must be
   * iterating over the same array type and should be comparable.
   * @return true if this reverse iterator points to a position that is after or the same as the position pointed to by
   * the other reverse iterator in the reverse direction, false otherwise.
   */
  bool operator>=(const ReverseIterator& iterator) const noexcept {
    return m_iterator <= iterator.m_iterator;
  }

  /**
   * @brief Returns a pointer to the element at the specified index from the current reverse iterator position. The
   * index is relative to the current position of the reverse iterator, meaning that an index of 0 returns a pointer to
   * the current element that the reverse iterator points to, an index of 1 returns a pointer to the next element in the
   * reverse direction, and so on.
   *
   * @param index The index of the element to access, relative to the current position of the reverse iterator. This can
   * be positive to access elements ahead of the current position in the reverse direction or negative to access
   * elements behind the current position in the reverse direction.
   * @return A pointer to the element at the specified index from the current reverse iterator position, cast to the
   * specified type.
   */
  void* operator[](const std::ptrdiff_t index) const noexcept {
    return m_iterator[-(index + 1)];
  }

  /**
   * @brief Returns a new reverse iterator that is moved backward by the specified offset from the given reverse
   * iterator position (which is the next position in the reverse direction). The original reverse iterator remains
   * unchanged. This operator allows for commutative addition, meaning that the offset can be on either side of the
   * operator.
   *
   * @param offset The number of elements by which to move the new reverse iterator backward. This can be positive to
   * move backward or negative to move forward in the array (which is the previous position in the reverse direction).
   * @param iterator The reverse iterator from which to calculate the new reverse iterator position. The new reverse
   * iterator will be moved backward by the specified offset from this reverse iterator's position.
   * @return A new reverse iterator that is moved backward by the specified offset from the given reverse iterator
   * position.
   */
  friend ReverseIterator operator+(const std::ptrdiff_t offset, const ReverseIterator& iterator) noexcept {
    ReverseIterator tmp(iterator);
    tmp += offset;
    return tmp;
  }

  /**
   * @brief Returns a new reverse iterator that is moved forward by the specified offset from the given reverse iterator
   * position (which is the previous position in the reverse direction). The original reverse iterator remains
   * unchanged. This operator allows for commutative subtraction, meaning that the offset can be on either side of the
   * operator.
   *
   * @param offset The number of elements by which to move the new reverse iterator forward. This can be positive to
   * move forward or negative to move backward in the array (which is the next position in the reverse direction).
   * @param iterator The reverse iterator from which to calculate the new reverse iterator position. The new reverse
   * iterator will be moved forward by the specified offset from this reverse iterator's position.
   * @return A new reverse iterator that is moved forward by the specified offset from the given reverse iterator
   * position.
   */
  friend ReverseIterator operator-(const std::ptrdiff_t offset, const ReverseIterator& iterator) noexcept {
    ReverseIterator tmp(iterator);
    tmp -= offset;
    return tmp;
  }

 private:
  /**
   * @brief The underlying regular iterator that this reverse iterator is based on. The reverse iterator points to the
   * element before the position of this regular iterator, which is the standard behavior for reverse iterators in C++.
   * This member is used to implement all the operations of the reverse iterator by manipulating the underlying regular
   * iterator accordingly.
   */
  RTTIIterator<T> m_iterator;
};
}  // namespace core::rtti
