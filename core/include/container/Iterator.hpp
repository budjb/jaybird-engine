#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>

namespace core::container {
/**
 *@brief A random access iterator that can be used with any pointer type.
 *
 * @tparam T The type of the elements pointed to by the iterator. Can be const or non-const.
 */
template <typename T>
class Iterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using iterator_concept = std::random_access_iterator_tag;
  using value_type = std::remove_cv_t<T>;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using reference = T&;

  /**
   * @brief Default constructor. Creates an iterator that does not point to any element.
   */
  Iterator() noexcept = default;

  /**
   * @brief Constructor that takes a pointer to an element. The iterator will point to the element pointed to by the
   * pointer.
   *
   * @param ptr A pointer to an element. The iterator will point to the element pointed to by the pointer.
   */
  explicit Iterator(pointer ptr) noexcept : m_ptr(ptr) {}

  /**
   * @brief Copy constructor that allows converting from an iterator of a different type. The other iterator must point
   * to an element of a type that is convertible to T.
   *
   * @tparam U The type of the elements pointed to by the other iterator. Must be convertible to T.
   * @param other The other iterator to copy from. The other iterator must point to an element of a type that is
   * convertible to T.
   */
  template <typename U>
    requires std::convertible_to<U*, T*>
  explicit Iterator(const Iterator<U>& other) noexcept : m_ptr(other.base()) {}

  /**
   * @brief Returns the underlying pointer of the iterator. The pointer points to the element that the iterator
   * currently points to.
   *
   * @return The underlying pointer of the iterator. The pointer points to the element that the iterator currently
   * points to.
   */
  [[nodiscard]] pointer base() const noexcept {
    return m_ptr;
  }

  /**
   * @brief Dereference operator. Returns a reference to the element that the iterator currently points to.
   *
   * @return A reference to the element that the iterator currently points to.
   */
  [[nodiscard]] reference operator*() const noexcept {
    return *m_ptr;
  }

  /**
   * @brief Arrow operator. Returns the underlying pointer of the iterator. The pointer points to the element that the
   * iterator currently points to.
   *
   * @return The underlying pointer of the iterator. The pointer points to the element that the iterator currently
   * points to.
   */
  [[nodiscard]] pointer operator->() const noexcept {
    return m_ptr;
  }

  /**
   * @brief Subscript operator. Returns a reference to the element that is offset from the element that the iterator
   * currently points to by the specified offset.
   *
   * @param offset The offset from the element that the iterator currently points to. The offset can be positive or
   * negative.
   * @return A reference to the element that is offset from the element that the iterator currently points to by the
   * specified offset.
   */
  [[nodiscard]] reference operator[](const difference_type offset) const noexcept {
    return m_ptr[offset];
  }

  /**
   * @brief Pre-increment operator. Advances the iterator to the next element and returns a reference to the iterator
   * itself.
   *
   * @return A reference to the iterator itself after advancing to the next element.
   */
  Iterator& operator++() noexcept {
    ++m_ptr;
    return *this;
  }

  /**
   * @brief Post-increment operator. Advances the iterator to the next element and returns a copy of the iterator before
   * advancing.
   *
   * @return A copy of the iterator before advancing to the next element.
   */
  Iterator operator++(int) noexcept {
    Iterator tmp(*this);
    ++*this;
    return tmp;
  }

  /**
   * @brief Pre-decrement operator. Moves the iterator to the previous element and returns a reference to the iterator
   * itself.
   *
   * @return A reference to the iterator itself after moving to the previous element.
   */
  Iterator& operator--() noexcept {
    --m_ptr;
    return *this;
  }

  /**
   * @brief Post-decrement operator. Moves the iterator to the previous element and returns a copy of the iterator
   * before moving.
   *
   * @return A copy of the iterator before moving to the previous element.
   */
  Iterator operator--(int) noexcept {
    Iterator tmp(*this);
    --*this;
    return tmp;
  }

  /**
   * @brief Compound assignment operator for addition. Advances the iterator by the specified offset and returns a
   * reference to the iterator itself.
   *
   * @param offset The offset to advance the iterator by. The offset can be positive or negative.
   * @return A reference to the iterator itself after advancing by the specified offset.
   */
  Iterator& operator+=(const difference_type offset) noexcept {
    m_ptr += offset;
    return *this;
  }

  /**
   * @brief Compound assignment operator for subtraction. Moves the iterator back by the specified offset and returns a
   * reference to the iterator itself.
   *
   * @param offset The offset to move the iterator back by. The offset can be positive or negative.
   * @return A reference to the iterator itself after moving back by the specified offset.
   */
  Iterator& operator-=(const difference_type offset) noexcept {
    m_ptr -= offset;
    return *this;
  }

  /**
   * @brief Addition operator. Returns a new iterator that is advanced by the specified offset from the current
   * iterator.
   *
   * @param offset The offset to advance the iterator by. The offset can be positive or negative.
   * @return A new iterator that is advanced by the specified offset from the current iterator.
   */
  [[nodiscard]] Iterator operator+(const difference_type offset) const noexcept {
    Iterator tmp(*this);
    tmp += offset;
    return tmp;
  }

  /**
   * @brief Subtraction operator. Returns a new iterator that is moved back by the specified offset from the current
   * iterator.
   *
   * @param offset The offset to move the iterator back by. The offset can be positive or negative.
   * @return A new iterator that is moved back by the specified offset from the current iterator.
   */
  [[nodiscard]] Iterator operator-(const difference_type offset) const noexcept {
    Iterator tmp(*this);
    tmp -= offset;
    return tmp;
  }

  /**
   * @brief Difference operator. Returns the difference between the current iterator and another iterator. The other
   * iterator must point to an element of a type that is convertible to T.
   *
   * @tparam U The type of the elements pointed to by the other iterator. Must be convertible to T.
   * @param other The other iterator to compare with. The other iterator must point to an element of a type that is
   * convertible to T.
   * @return The difference between the current iterator and the other iterator. The result is positive if the current
   * iterator is ahead of the other iterator, negative if it is behind, and zero if they point to the same element.
   */
  template <typename U>
  [[nodiscard]] difference_type operator-(const Iterator<U>& other) const noexcept {
    return m_ptr - other.base();
  }

  /**
   * @brief Comparison operators. Compares the current iterator with another iterator. The other iterator must point to
   * an element of a type that is convertible to T.
   *
   * @tparam U The type of the elements pointed to by the other iterator. Must be convertible to T.
   * @param other The other iterator to compare with. The other iterator must point to an element of a type that is
   * convertible to T.
   * @return The result of the comparison. The operators return true if the comparison is true, and false otherwise.
   */
  template <typename U>
  [[nodiscard]] bool operator==(const Iterator<U>& other) const noexcept {
    return m_ptr == other.base();
  }

  /**
   * @brief Inequality operator. Compares the current iterator with another iterator for inequality. The other iterator
   * must point to an element of a type that is convertible to T.
   *
   * @tparam U The type of the elements pointed to by the other iterator. Must be convertible to T.
   * @param other The other iterator to compare with. The other iterator must point to an element of a type that is
   * convertible to T.
   * @return The result of the inequality comparison. Returns true if the current iterator and the other iterator do not
   * point to the same element, and false otherwise.
   */
  template <typename U>
  [[nodiscard]] bool operator!=(const Iterator<U>& other) const noexcept {
    return !(*this == other);
  }

  /**
   * @brief Relational operators. Compares the current iterator with another iterator for ordering. The other iterator
   * must point to an element of a type that is convertible to T.
   *
   * @tparam U The type of the elements pointed to by the other iterator. Must be convertible to T.
   * @param other The other iterator to compare with. The other iterator must point to an element of a type that is
   * convertible to T.
   * @return The result of the relational comparison. The operators return true if the comparison is true, and false
   * otherwise.
   */
  template <typename U>
  [[nodiscard]] bool operator<(const Iterator<U>& other) const noexcept {
    return m_ptr < other.base();
  }

  /**
   * @brief Relational operators. Compares the current iterator with another iterator for ordering. The other iterator
   * must point to an element of a type that is convertible to T.
   *
   * @tparam U The type of the elements pointed to by the other iterator. Must be convertible to T.
   * @param other The other iterator to compare with. The other iterator must point to an element of a type that is
   * convertible to T.
   * @return The result of the relational comparison. The operators return true if the comparison is true, and false
   * otherwise.
   */
  template <typename U>
  [[nodiscard]] bool operator<=(const Iterator<U>& other) const noexcept {
    return !(other < *this);
  }

  /**
   * @brief Relational operators. Compares the current iterator with another iterator for ordering. The other iterator
   * must point to an element of a type that is convertible to T.
   *
   * @tparam U The type of the elements pointed to by the other iterator. Must be convertible to T.
   * @param other The other iterator to compare with. The other iterator must point to an element of a type that is
   * convertible to T.
   * @return The result of the relational comparison. The operators return true if the comparison is true, and false
   * otherwise.
   */
  template <typename U>
  [[nodiscard]] bool operator>(const Iterator<U>& other) const noexcept {
    return other < *this;
  }

  /**
   * @brief Relational operators. Compares the current iterator with another iterator for ordering. The other iterator
   * must point to an element of a type that is convertible to T.
   *
   * @tparam U The type of the elements pointed to by the other iterator. Must be convertible to T.
   * @param other The other iterator to compare with. The other iterator must point to an element of a type that is
   * convertible to T.
   * @return The result of the relational comparison. The operators return true if the comparison is true, and false
   * otherwise.
   */
  template <typename U>
  [[nodiscard]] bool operator>=(const Iterator<U>& other) const noexcept {
    return !(*this < other);
  }

  /**
   * @brief Addition operator. Returns a new iterator that is advanced by the specified offset from the current
   * iterator. The offset is added to the current iterator, and the result is a new iterator that points to the element
   * that is offset from the element that the current iterator points to by the specified offset.
   *
   * @param offset The offset to advance the iterator by. The offset can be positive or negative. A positive offset
   * advances the iterator forward, while a negative offset moves it backward.
   * @param it The current iterator to add the offset to. The iterator must point to an element of a type that is
   * convertible to T.
   * @return A new iterator that is advanced by the specified offset from the current iterator. The new iterator points
   * to the element that is offset from the element that the current iterator points to by the specified offset.
   */
  friend Iterator operator+(const difference_type offset, Iterator it) noexcept {
    it += offset;
    return it;
  }

 private:
  /**
   * @brief The underlying pointer of the iterator. The pointer points to the element that the iterator currently points
   * to. The pointer can be null, which indicates that the iterator does not point to any element.
   */
  pointer m_ptr = nullptr;
};
}  // namespace core::container
