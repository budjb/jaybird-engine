#pragma once

#include <cstddef>
#include <memory>

#include "IType.hpp"

namespace core::rtti {
template <typename T>
class TType : public IType {
 public:
  using Type = T;

  explicit TType(const IString& name, const TypeKind kind) : IType(name, sizeof(T), alignof(T), kind) {}

  void assign(void* destination, const void* source) override {
    if (destination != nullptr) {
      *static_cast<Type*>(destination) = *static_cast<const Type*>(source);
    }
  }

  void* create() noexcept override {
    void* memory = new std::byte[sizeof(T)];
    construct(memory);
    return memory;
  }

  void free(void* memory) noexcept override {
    if (memory == nullptr) {
      return;
    }
    destroy(memory);
    delete[] static_cast<std::byte*>(memory);
  }

  void construct(void* memory) noexcept override {
    std::construct_at<Type>(static_cast<Type*>(memory));
  }

  void destroy(void* memory) noexcept override {
    std::destroy_at<T>(static_cast<T*>(memory));
  }

  bool equals(const void* lhs, const void* rhs) const noexcept override {
    if (lhs == nullptr || rhs == nullptr) {
      return lhs == rhs;
    }
    return *static_cast<const Type*>(lhs) == *static_cast<const Type*>(rhs);
  }
};

}  // namespace core::rtti
