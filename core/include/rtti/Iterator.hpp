#pragma once

#include <cstddef>

#include "IArrayType.hpp"

namespace core::rtti {

template <typename T = void>
class Iterator {
 public:
  explicit Iterator(IArrayType* arrayType, T* ptr) noexcept : m_arrayType(arrayType), m_ptr(ptr) {}

  [[nodiscard]] T* base() const noexcept {
    return static_cast<std::byte*>(m_ptr);
  }

  operator T*() const noexcept {
    return m_ptr;
  }

  T* get() const noexcept {
    return static_cast<T*>(m_ptr);
  }

  Iterator& operator++() noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) + m_arrayType->inner()->size();
    } else {
      m_ptr = static_cast<T*>(m_ptr) + 1;
    }
    return *this;
  }

  Iterator operator++(int) noexcept {
    const Iterator tmp(*this);
    ++*this;
    return tmp;
  }

  Iterator& operator--() noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) - m_arrayType->inner()->size();
    } else {
      m_ptr = static_cast<T*>(m_ptr) - 1;
    }
    return *this;
  }

  Iterator operator--(int) noexcept {
    const Iterator tmp(*this);
    --*this;
    return tmp;
  }

  Iterator& operator+=(const std::ptrdiff_t offset) noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) + m_arrayType->inner()->size() * offset;
    } else {
      m_ptr = static_cast<T*>(m_ptr) + offset;
    }
    return *this;
  }

  Iterator& operator-=(const std::ptrdiff_t offset) noexcept {
    if constexpr (std::is_void_v<T>) {
      m_ptr = static_cast<std::byte*>(m_ptr) - m_arrayType->inner()->size() * offset;
    } else {
      m_ptr = static_cast<T*>(m_ptr) - offset;
    }
    return *this;
  }

  Iterator operator+(const std::ptrdiff_t offset) const noexcept {
    Iterator tmp(*this);
    tmp += offset;
    return tmp;
  }

  Iterator operator-(const std::ptrdiff_t offset) const noexcept {
    Iterator tmp(*this);
    tmp -= offset;
    return tmp;
  }

  std::ptrdiff_t operator-(const Iterator& iterator) const noexcept {
    if constexpr (std::is_void_v<T>) {
      const std::ptrdiff_t size = m_arrayType->inner()->size();  // NOLINT(*-narrowing-conversions)
      return (static_cast<std::byte*>(m_ptr) - static_cast<std::byte*>(iterator.m_ptr)) / size;
    } else {
      return static_cast<T*>(m_ptr) - static_cast<T*>(iterator.m_ptr);
    }
  }

  bool operator==(const Iterator& iterator) const noexcept {
    return m_ptr == iterator.m_ptr;
  }

  bool operator!=(const Iterator& iterator) const noexcept {
    return m_ptr != iterator.m_ptr;
  }

  bool operator<(const Iterator& iterator) const noexcept {
    return m_ptr < iterator.m_ptr;
  }

  bool operator<=(const Iterator& iterator) const noexcept {
    return m_ptr <= iterator.m_ptr;
  }

  bool operator>(const Iterator& iterator) const noexcept {
    return m_ptr > iterator.m_ptr;
  }

  bool operator>=(const Iterator& iterator) const noexcept {
    return m_ptr >= iterator.m_ptr;
  }

  void* operator[](const std::ptrdiff_t index) const noexcept {
    if constexpr (std::is_void_v<T>) {
      return static_cast<std::byte*>(m_ptr) + index * m_arrayType->inner()->size();
    } else {
      return static_cast<T*>(m_ptr) + index;
    }
  }

  friend Iterator operator+(const std::ptrdiff_t offset, const Iterator& iterator) noexcept {
    Iterator tmp(iterator);
    tmp += offset;
    return tmp;
  }

  friend Iterator operator-(const std::ptrdiff_t offset, const Iterator& iterator) noexcept {
    Iterator tmp(iterator);
    tmp -= offset;
    return tmp;
  }

 private:
  IArrayType* m_arrayType;
  void* m_ptr;
};

template <typename T = void>
class ReverseIterator {
 public:
  explicit ReverseIterator(Iterator<T> iterator) noexcept : m_iterator(iterator) {}

  [[nodiscard]] T* base() const noexcept {
    return m_iterator.base();
  }

  operator T*() const noexcept {
    return get();
  }

  T* get() const noexcept {
    return m_iterator - 1;
  }

  ReverseIterator& operator++() noexcept {
    --m_iterator;
    return *this;
  }

  ReverseIterator operator++(int) noexcept {
    const ReverseIterator tmp(*this);
    ++*this;
    return tmp;
  }

  ReverseIterator& operator--() noexcept {
    ++m_iterator;
    return *this;
  }

  ReverseIterator operator--(int) noexcept {
    const ReverseIterator tmp(*this);
    --*this;
    return tmp;
  }

  ReverseIterator& operator+=(const std::ptrdiff_t offset) noexcept {
    m_iterator -= offset;
    return *this;
  }

  ReverseIterator& operator-=(const std::ptrdiff_t offset) noexcept {
    m_iterator += offset;
    return *this;
  }

  ReverseIterator operator+(const std::ptrdiff_t offset) const noexcept {
    ReverseIterator tmp(*this);
    tmp += offset;
    return tmp;
  }

  ReverseIterator operator-(const std::ptrdiff_t offset) const noexcept {
    ReverseIterator tmp(*this);
    tmp -= offset;
    return tmp;
  }

  std::ptrdiff_t operator-(const ReverseIterator& iterator) const noexcept {
    return iterator.m_iterator - m_iterator;
  }

  bool operator==(const ReverseIterator& iterator) const noexcept {
    return m_iterator == iterator.m_iterator;
  }

  bool operator!=(const ReverseIterator& iterator) const noexcept {
    return m_iterator != iterator.m_iterator;
  }

  bool operator<(const ReverseIterator& iterator) const noexcept {
    return m_iterator > iterator.m_iterator;
  }

  bool operator<=(const ReverseIterator& iterator) const noexcept {
    return m_iterator >= iterator.m_iterator;
  }

  bool operator>(const ReverseIterator& iterator) const noexcept {
    return m_iterator < iterator.m_iterator;
  }

  bool operator>=(const ReverseIterator& iterator) const noexcept {
    return m_iterator <= iterator.m_iterator;
  }

  void* operator[](const std::ptrdiff_t index) const noexcept {
    return m_iterator[-(index + 1)];
  }

  friend ReverseIterator operator+(const std::ptrdiff_t offset, const ReverseIterator& iterator) noexcept {
    ReverseIterator tmp(iterator);
    tmp += offset;
    return tmp;
  }

  friend ReverseIterator operator-(const std::ptrdiff_t offset, const ReverseIterator& iterator) noexcept {
    ReverseIterator tmp(iterator);
    tmp -= offset;
    return tmp;
  }

 private:
  Iterator<T> m_iterator;
};

}  // namespace core::rtti
