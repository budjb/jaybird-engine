#pragma once

#include "IContainerType.hpp"

namespace core::rtti {

template <typename>
class Iterator;

template <typename>
class ReverseIterator;

class IArrayType : public IContainerType {
 public:
  explicit IArrayType(const IString& name, std::size_t size, std::size_t alignment, const IType* inner) noexcept;
  ~IArrayType() override = default;

  [[nodiscard]] virtual std::size_t length(const void* array) const noexcept = 0;
  [[nodiscard]] virtual std::size_t capacity(const void* array) const noexcept = 0;
  [[nodiscard]] virtual std::size_t maxLength(const void* array) const noexcept = 0;

  [[nodiscard]] virtual void* at(const void* array, std::size_t index) = 0;
  [[nodiscard]] virtual const void* at(const void* array, std::size_t index) const = 0;

  template <typename T>
  [[nodiscard]] T* at(const void* array, const std::size_t index) {
    return static_cast<T*>(at(array, index));
  }

  template <typename T>
  [[nodiscard]] const T* at(const void* array, const std::size_t index) const {
    return static_cast<const T*>(at(array, index));
  }

  [[nodiscard]] virtual void* front(const void* array) = 0;
  [[nodiscard]] virtual const void* front(const void* array) const = 0;

  [[nodiscard]] virtual void* back(const void* array) = 0;
  [[nodiscard]] virtual const void* back(const void* array) const = 0;

  [[nodiscard]] virtual Iterator<void> begin(void* array) noexcept = 0;
  template <typename T>
  [[nodiscard]] Iterator<T> begin(void* array) noexcept {
    return Iterator<T>(begin(static_cast<T*>(array)));
  }

  [[nodiscard]] virtual Iterator<void> end(void* array) noexcept = 0;

  template <typename T>
  [[nodiscard]] Iterator<T> end(void* array) noexcept {
    return Iterator<T>(end(static_cast<T*>(array)));
  }

  [[nodiscard]] ReverseIterator<void> rbegin(void* array) noexcept;

  template <typename T>
  [[nodiscard]] ReverseIterator<T> rbegin(void* array) noexcept {
    return ReverseIterator<T>(rbegin(static_cast<T*>(array)));
  }

  [[nodiscard]] ReverseIterator<void> rend(void* array) noexcept;

  template <typename T>
  [[nodiscard]] ReverseIterator<T> rend(void* array) noexcept {
    return ReverseIterator<T>(rend(static_cast<T*>(array)));
  }

  virtual void erase(const void* array, std::size_t index) = 0;
  virtual void insert(const void* array, std::size_t index, const void* value) = 0;
  virtual void pushBack(const void* array, const void* value) = 0;
  virtual void popBack(const void* array) = 0;
  virtual void remove(const void* array, std::size_t index) = 0;
  virtual void replace(const void* array, std::size_t index, const void* value) = 0;
  virtual void reserve(void* array, std::size_t size) = 0;
  virtual void resize(void* array, std::size_t size) = 0;
  virtual void shrinkToFit(void* array) = 0;
  virtual void clear(void* array) = 0;
};

}  // namespace core::rtti
