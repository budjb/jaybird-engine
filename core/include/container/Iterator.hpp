#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>

namespace core::container {
template <typename T>
class Iterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using iterator_concept = std::random_access_iterator_tag;
  using value_type = std::remove_cv_t<T>;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using reference = T&;

  Iterator() noexcept = default;
  explicit Iterator(pointer ptr) noexcept : m_ptr(ptr) {}

  template <typename U>
    requires std::convertible_to<U*, T*>
  Iterator(const Iterator<U>& other) noexcept : m_ptr(other.base()) {}

  [[nodiscard]] pointer base() const noexcept {
    return m_ptr;
  }

  [[nodiscard]] reference operator*() const noexcept {
    return *m_ptr;
  }

  [[nodiscard]] pointer operator->() const noexcept {
    return m_ptr;
  }

  [[nodiscard]] reference operator[](const difference_type offset) const noexcept {
    return m_ptr[offset];
  }

  Iterator& operator++() noexcept {
    ++m_ptr;
    return *this;
  }

  Iterator operator++(int) noexcept {
    Iterator tmp(*this);
    ++(*this);
    return tmp;
  }

  Iterator& operator--() noexcept {
    --m_ptr;
    return *this;
  }

  Iterator operator--(int) noexcept {
    Iterator tmp(*this);
    --(*this);
    return tmp;
  }

  Iterator& operator+=(const difference_type offset) noexcept {
    m_ptr += offset;
    return *this;
  }

  Iterator& operator-=(const difference_type offset) noexcept {
    m_ptr -= offset;
    return *this;
  }

  [[nodiscard]] Iterator operator+(const difference_type offset) const noexcept {
    Iterator tmp(*this);
    tmp += offset;
    return tmp;
  }

  [[nodiscard]] Iterator operator-(const difference_type offset) const noexcept {
    Iterator tmp(*this);
    tmp -= offset;
    return tmp;
  }

  template <typename U>
  [[nodiscard]] difference_type operator-(const Iterator<U>& other) const noexcept {
    return m_ptr - other.base();
  }

  template <typename U>
  [[nodiscard]] bool operator==(const Iterator<U>& other) const noexcept {
    return m_ptr == other.base();
  }

  template <typename U>
  [[nodiscard]] bool operator!=(const Iterator<U>& other) const noexcept {
    return !(*this == other);
  }

  template <typename U>
  [[nodiscard]] bool operator<(const Iterator<U>& other) const noexcept {
    return m_ptr < other.base();
  }

  template <typename U>
  [[nodiscard]] bool operator<=(const Iterator<U>& other) const noexcept {
    return !(other < *this);
  }

  template <typename U>
  [[nodiscard]] bool operator>(const Iterator<U>& other) const noexcept {
    return other < *this;
  }

  template <typename U>
  [[nodiscard]] bool operator>=(const Iterator<U>& other) const noexcept {
    return !(*this < other);
  }

  friend Iterator operator+(const difference_type offset, Iterator it) noexcept {
    it += offset;
    return it;
  }

 private:
  pointer m_ptr = nullptr;
};
}  // namespace core::container
