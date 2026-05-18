#pragma once
#include <iterator>

namespace core::container {
/**
 * @brief A simple implementation of a forward iterator for a container. This iterator provides basic functionality for
 * iterating over a sequence of elements in a container, including dereferencing, incrementing, and comparing for
 * equality.
 *
 * The iterator is designed to be compatible with the C++ Standard Library's iterator requirements, allowing it to be
 * used with standard algorithms and range-based for loops.
 *
 * @tparam T The type of elements that the iterator points to.
 */
template <typename T>
class Iterator {
 public:
  /**
   * @brief Type alias for the iterator category of this iterator (STL).
   */
  using iterator_category = std::bidirectional_iterator_tag;

  /**
   * @brief Type alias for the value type of elements pointed to by this iterator (STL).
   */
  using value_type = T;

  /**
   * @brief Type alias for the difference type used to represent pointer differences and iterator distances (STL).
   */
  using difference_type = std::ptrdiff_t;

  /**
   * @brief Type alias for the pointer type used by this iterator to represent pointers to elements (STL).
   */
  using pointer = T*;

  /**
   * @brief Type alias for the const pointer type used by this iterator to represent read-only pointers to elements
   * (STL).
   */
  using const_pointer = const T*;

  /**
   * @brief Type alias for the reference type used by this iterator to represent references to elements (STL).
   */
  using reference = T&;

  /**
   * @brief Constructs a new Iterator object that points to the specified element. The iterator is initialized with a
   * pointer to the element that it will point to, allowing it to be used for iterating over a sequence of elements in a
   * container.
   *
   * @param ptr A pointer to the element that the iterator will point to.
   */
  explicit Iterator(const_pointer ptr) noexcept : m_ptr(ptr) {}

  /**
   * @brief Dereferences the iterator to access the element it points to. This operator allows the iterator to be used
   * in expressions that require access to the element, such as in range-based for loops or when using standard
   * algorithms that operate on iterators.
   *
   * @return A reference to the element that the iterator points to, allowing for both reading and writing of the
   * element.
   */
  reference operator*() const noexcept {
    return *m_ptr;
  }

  /**
   * @brief Provides access to the element that the iterator points to.
   *
   * @return A pointer to the element that the iterator points to, allowing for both reading and writing of the
   * element's members.
   */
  pointer operator->() const noexcept {
    return m_ptr;
  }

  /**
   * @brief Pre-increment operator that advances the iterator to the next element in the sequence. This operator
   * modifies the iterator itself and returns a reference to the updated iterator, allowing for chaining of increment
   * operations.
   *
   * @return A reference to the updated iterator after advancing to the next element in the sequence.
   */
  Iterator& operator++() noexcept {
    ++m_ptr;
    return *this;
  }

  /**
   * @brief Post-increment operator that advances the iterator to the next element in the sequence. This operator
   * creates a temporary copy of the iterator before advancing it, allowing the original iterator to be returned while
   * advancing to the next element in the sequence.
   *
   * @return A copy of the original iterator before advancing to the next element in the sequence.
   */
  Iterator operator++(int) noexcept {
    Iterator tmp(*this);
    ++m_ptr;
    return tmp;
  }

  /**
   * @brief Pre-decrement operator that moves the iterator to the previous element in the sequence. This operator
   * modifies the iterator itself and returns a reference to the updated iterator, allowing for chaining of decrement
   * operations.
   *
   * @return A reference to the updated iterator after moving to the previous element in the sequence.
   */
  Iterator& operator--() noexcept {
    --m_ptr;
    return *this;
  }

  /**
   * @brief Post-decrement operator that moves the iterator to the previous element in the sequence. This operator
   * creates a temporary copy of the iterator before moving it, allowing the original iterator to be returned while
   * moving to the previous element in the sequence.
   *
   * @return A copy of the original iterator before moving to the previous element in the sequence.
   */
  Iterator operator--(int) noexcept {
    Iterator tmp(*this);
    --m_ptr;
    return tmp;
  }

  /**
   * @brief Equality comparison operator that checks if two iterators are equal by comparing their underlying pointers.
   * This operator allows for checking if two iterators point to the same element in a container, which is useful for
   * determining the end of a sequence when iterating through elements.
   *
   * @param other The other iterator to compare with this iterator for equality.
   * @return true if the two iterators are equal (i.e., if they point to the same element), and false otherwise.
   */
  bool operator==(const Iterator& other) const noexcept {
    return m_ptr == other.m_ptr;
  }

  /**
   * @brief Inequality comparison operator that checks if two iterators are not equal by comparing their underlying
   * pointers. This operator allows for checking if two iterators point to different elements in a container, which is
   * useful for determining if the end of a sequence has been reached when iterating through elements.
   *
   * @param other The other iterator to compare with this iterator for inequality.
   * @return true if the two iterators are not equal (i.e., if they point to different elements), and false otherwise.
   */
  bool operator!=(const Iterator& other) const noexcept {
    return m_ptr != other.m_ptr;
  }

 private:
  /**
   * @brief A pointer to the element that the iterator points to. This pointer is used to access the element and to
   * perform the necessary operations for iterating through a sequence of elements in a container.
   */
  pointer m_ptr;
};
}  // namespace core::container
