#pragma once

#include <cstddef>

#include "IArrayType.hpp"

namespace core::rtti {
/**
 * @brief A template class representing an iterator for array types in the RTTI system. This iterator provides standard
 * iterator operations such as incrementing, decrementing, and pointer arithmetic, while also handling the case where
 * the underlying type is void (i.e., when the array type is not known at compile time). In the case of a void type, the
 * iterator uses the size of the inner type from the array type descriptor to perform pointer arithmetic correctly.
 *
 * While the iterator attempts to provide a similar API contract as STL iterators, it allows type-erased access to array
 * elements, which means that it may not fully conform to all STL iterator requirements. For example, functions that
 * typically return references will return pointers instead.
 *
 * @tparam T The type of the elements in the array.
 */
template <typename T = void>
class Iterator {
 public:
  /**
   * @brief Constructs an Iterator for the given array type and pointer.
   *
   * @param arrayType A pointer to the IArrayType descriptor for the array type that this iterator will be iterating
   * over.
   * @param ptr A pointer to the current element in the array.
   */
  explicit Iterator(IArrayType* arrayType, T* ptr) noexcept : m_arrayType(arrayType), m_ptr(ptr) {}

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
  Iterator& operator++() noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) + m_arrayType->inner()->size();
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
  Iterator operator++(int) noexcept {
    const Iterator tmp(*this);
    ++*this;
    return tmp;
  }

  /**
   * @brief Pre-decrement operator that moves the iterator to the previous element in the array.
   *
   * @return A reference to the iterator after it has been moved to the previous element in the array.
   */
  Iterator& operator--() noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) - m_arrayType->inner()->size();
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
  Iterator operator--(int) noexcept {
    const Iterator tmp(*this);
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
  Iterator& operator+=(const std::ptrdiff_t offset) noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) + m_arrayType->inner()->size() * offset;
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
  Iterator& operator-=(const std::ptrdiff_t offset) noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) - m_arrayType->inner()->size() * offset;
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
  Iterator operator+(const std::ptrdiff_t offset) const noexcept {
    Iterator tmp(*this);
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
  Iterator operator-(const std::ptrdiff_t offset) const noexcept {
    Iterator tmp(*this);
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
  std::ptrdiff_t operator-(const Iterator& iterator) const noexcept {
    if constexpr (std::is_void_v<T>) {
      const std::ptrdiff_t size = m_arrayType->inner()->size();  // NOLINT(*-narrowing-conversions)
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
  bool operator==(const Iterator& iterator) const noexcept {
    return m_ptr == iterator.m_ptr;
  }

  /**
   * @brief Compares this iterator with another iterator for inequality.
   *
   * @param iterator The other iterator to compare with this iterator.
   * @return true if the iterators point to different positions in the array, false if they point to the same position.
   */
  bool operator!=(const Iterator& iterator) const noexcept {
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
  bool operator<(const Iterator& iterator) const noexcept {
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
  bool operator<=(const Iterator& iterator) const noexcept {
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
  bool operator>(const Iterator& iterator) const noexcept {
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
  bool operator>=(const Iterator& iterator) const noexcept {
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
      return static_cast<std::byte*>(m_ptr) + index * m_arrayType->inner()->size();
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
  friend Iterator operator+(const std::ptrdiff_t offset, const Iterator& iterator) noexcept {
    Iterator tmp(iterator);
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
  friend Iterator operator-(const std::ptrdiff_t offset, const Iterator& iterator) noexcept {
    Iterator tmp(iterator);
    tmp -= offset;
    return tmp;
  }

 private:
  /**
   * @brief A pointer to the IArrayType descriptor for the array type that this iterator is iterating over. This is used
   * to perform pointer arithmetic correctly when the underlying type is void, by using the size of the inner type from
   * the array type descriptor.
   */
  IArrayType* m_arrayType;

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
 * @brief A template class representing a reverse iterator for array types in the RTTI system. This reverse iterator
 * provides standard reverse iterator operations such as incrementing, decrementing, and pointer arithmetic, while also
 * handling the case where the underlying type is void (i.e., when the array type is not known at compile time). In the
 * case of a void type, the reverse iterator uses the size of the inner type from the array type descriptor to perform
 * pointer arithmetic correctly.
 *
 * @tparam T The type of the elements in the array.
 */
template <typename T = void>
class ReverseIterator {
 public:
  /**
   * @brief Constructs a ReverseIterator from a regular Iterator. The reverse iterator will point to the element before
   * the position of the given iterator, which is the standard behavior for reverse iterators in C++.
   *
   * @param iterator The regular iterator from which to construct the reverse iterator. The reverse iterator will point
   * to the element before the position of this iterator.
   */
  explicit ReverseIterator(Iterator<T> iterator) noexcept : m_iterator(iterator) {}

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
  Iterator<T> m_iterator;
};

}  // namespace core::rtti
