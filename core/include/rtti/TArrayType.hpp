#pragma once

#include <concepts>
#include <cstddef>
#include <vector>

#include "IArrayType.hpp"
#include "Iterator.hpp"
#include "TType.hpp"

namespace core::rtti {
template <typename InnerType, typename ElementType>
concept TypedInnerDescriptorFor = std::derived_from<InnerType, IType> && requires { typename InnerType::Type; } &&
                                  std::same_as<typename InnerType::Type, ElementType>;

template <typename T>
class TArrayType : public IArrayType {
 public:
  using Type = std::vector<T>;

  template <typename InnerType>
    requires TypedInnerDescriptorFor<InnerType, T>
  explicit TArrayType(const IString& name, const InnerType* inner)
      : IArrayType(name, sizeof(Type), alignof(Type), static_cast<const IType*>(inner)) {}

  void assign(void* destination, const void* source) override {
    if (destination != nullptr) {
      *static_cast<Type*>(destination) = *static_cast<const Type*>(source);
    }
  }

  void* create() override {
    void* memory = new std::byte[sizeof(Type)];
    construct(memory);
    return memory;
  }

  void free(void* memory) override {
    if (memory != nullptr) {
      destroy(memory);
      delete[] static_cast<std::byte*>(memory);
    }
  }

  void destroy(void* memory) noexcept override {
    if (memory != nullptr) {
      std::destroy_at<Type>(static_cast<Type*>(memory));
    }
  }

  void construct(void* memory) noexcept override {
    if (memory != nullptr) {
      std::construct_at<Type>(static_cast<Type*>(memory));
    }
  }

  bool equals(const void* lhs, const void* rhs) const noexcept override {
    if (lhs == nullptr || rhs == nullptr) {
      return lhs == rhs;
    }
    return *static_cast<const Type*>(lhs) == *static_cast<const Type*>(rhs);
  }

  [[nodiscard]] std::size_t length(const void* array) const noexcept override {
    if (array != nullptr) {
      return static_cast<const Type*>(array)->size();
    }
    return 0;
  }

  [[nodiscard]] std::size_t capacity(const void* array) const noexcept override {
    if (array != nullptr) {
      return static_cast<const Type*>(array)->capacity();
    }
    return 0;
  }

  [[nodiscard]] std::size_t maxLength(const void* array) const noexcept override {
    if (array != nullptr) {
      return static_cast<const Type*>(array)->max_size();
    }
    return 0;
  }

  [[nodiscard]] void* at(const void* array, std::size_t index) override {
    if (array != nullptr) {
      return &static_cast<Type*>(const_cast<void*>(array))->at(index);
    }
    return nullptr;
  }

  [[nodiscard]] const void* at(const void* array, std::size_t index) const override {
    if (array != nullptr) {
      return &static_cast<const Type*>(array)->at(index);
    }
    return nullptr;
  }

  [[nodiscard]] void* front(const void* array) override {
    if (array != nullptr) {
      return &static_cast<Type*>(const_cast<void*>(array))->front();
    }
    return nullptr;
  }

  [[nodiscard]] const void* front(const void* array) const override {
    if (array != nullptr) {
      return &static_cast<const Type*>(array)->front();
    }
    return nullptr;
  }

  [[nodiscard]] void* back(const void* array) override {
    if (array != nullptr) {
      return &static_cast<Type*>(const_cast<void*>(array))->back();
    }
    return nullptr;
  }

  [[nodiscard]] const void* back(const void* array) const override {
    if (array != nullptr) {
      return &static_cast<const Type*>(array)->back();
    }
    return nullptr;
  }

  Iterator<> begin(void* ptr) noexcept override {
    if (ptr == nullptr) {
      return Iterator<>(this, nullptr);
    }
    auto* vec = static_cast<Type*>(ptr);
    return Iterator<>(this, static_cast<void*>(vec->data()));
  }

  Iterator<> end(void* ptr) noexcept override {
    if (ptr == nullptr) {
      return Iterator<>(this, nullptr);
    }
    auto* vec = static_cast<Type*>(ptr);
    return Iterator<>(this, static_cast<void*>(vec->data() + vec->size()));
  }

  void erase(const void* array, const std::size_t index) override {
    if (array != nullptr) {
      auto* vector = static_cast<Type*>(const_cast<void*>(array));
      vector->erase(vector->begin() + static_cast<std::ptrdiff_t>(index));
    }
  }

  void insert(const void* array, const std::size_t index, const void* value) override {
    if (array != nullptr && value != nullptr) {
      auto* vector = static_cast<Type*>(const_cast<void*>(array));
      vector->insert(vector->begin() + static_cast<std::ptrdiff_t>(index), *static_cast<const T*>(value));
    }
  }

  void pushBack(const void* array, const void* value) override {
    if (array != nullptr && value != nullptr) {
      static_cast<Type*>(const_cast<void*>(array))->push_back(*static_cast<const T*>(value));
    }
  }

  void popBack(const void* array) override {
    if (array != nullptr) {
      static_cast<Type*>(const_cast<void*>(array))->pop_back();
    }
  }

  void remove(const void* array, const std::size_t index) override {
    if (array != nullptr) {
      auto* vector = static_cast<Type*>(const_cast<void*>(array));
      vector->erase(vector->begin() + static_cast<std::ptrdiff_t>(index));
    }
  }

  void replace(const void* array, std::size_t index, const void* value) override {
    if (array != nullptr && value != nullptr) {
      static_cast<Type*>(const_cast<void*>(array))->at(index) = *static_cast<const T*>(value);
    }
  }

  void resize(void* array, std::size_t size) override {
    if (array != nullptr) {
      static_cast<Type*>(array)->resize(size);
    }
  }

  void reserve(void* array, std::size_t size) override {
    if (array != nullptr) {
      static_cast<Type*>(array)->reserve(size);
    }
  }

  void shrinkToFit(void* array) override {
    if (array != nullptr) {
      static_cast<Type*>(array)->shrink_to_fit();
    }
  }

  void clear(void* array) override {
    if (array != nullptr) {
      static_cast<Type*>(array)->clear();
    }
  }
};
}  // namespace core::rtti
