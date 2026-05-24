#pragma once

#include <memory>

#include "IType.hpp"

namespace core::rtti {
/**
 * @brief A template class representing a non-container type in the RTTI system.
 *
 * @tparam T The underlying type that this TType represents.
 */
template <typename T>
class TType : public IType {
 public:
  /**
   * @brief Defines a type alias for the underlying type T. This allows users of TType to refer to the underlying type
   * using TType<T>::Type.
   */
  using Type = T;

  /**
   * @brief Constructs a TType with the given name and kind. The size and alignment are automatically determined based
   * on the underlying type T.
   *
   * @param name The name of the type, represented as an IName.
   * @param kind The kind of the type, represented as a TypeKind enumeration value.
   */
  explicit TType(const IName& name, const TypeKind kind) : IType(name, sizeof(T), alignof(T), kind) {}

  /**
   * @brief Assigns the value from the source pointer to the destination pointer. Both pointers are expected to point to
   * valid instances of the underlying type T. The behavior is undefined if either pointer does not point to valid
   * instances of the type.
   *
   * If the underlying type T is trivially copyable, the assignment is performed using std::memcpy for efficiency.
   * Otherwise, the assignment is performed using the copy assignment operator of the underlying type T.
   *
   * @param destination A pointer to the destination instance of the type where the value will be assigned.
   * @param source A pointer to the source instance of the type from which the value will be copied.
   */
  void assign(void* destination, const void* source) override {
    if (destination == nullptr) {
      return;
    }

    if constexpr (std::is_trivially_copyable_v<Type>) {
      std::memcpy(destination, source, sizeof(Type));
    } else {
      *static_cast<Type*>(destination) = *static_cast<const Type*>(source);
    }
  }

  /**
   * @brief Allocates memory for an instance of type @code T@endcode.
   *
   * The type is not constructed as part of this allocation process; it only allocates raw memory that can hold an
   * instance of the type.
   *
   * The caller is responsible for managing the allocated memory and ensuring that it is properly freed when no longer
   * needed.
   *
   * @return A pointer to the allocated memory for an instance of the type.
   */
  void* allocate() noexcept override {
    return operator new(sizeof(Type), static_cast<std::align_val_t>(alignof(Type)));
  }

  /**
   * @brief Frees the memory allocated for an instance of type @code T@endcode.
   *
   * The type is not destructed as part of this allocation process; it only allocates raw memory that can hold an
   * instance of the type.
   *
   * The provided memory pointer should have been allocated by the allocate() function of this type, and it is the
   * caller's responsibility to ensure that the memory is properly freed when no longer needed.
   *
   * @param memory A pointer to the memory that should be freed.
   */
  void deallocate(void* memory) noexcept override {
    operator delete(memory, sizeof(Type), static_cast<std::align_val_t>(alignof(Type)));
  }

  /**
   * @brief Constructs an instance of type @c T in the provided memory location. The memory should be pre-allocated and
   * large enough to hold an instance of the type.
   *
   * The behavior is undefined if the memory pointer does not point to a valid memory location that can hold an instance
   * of the type.
   *
   * @param memory A pointer to the memory where the instance should be constructed. This memory should be pre-allocated
   * and large enough to hold an instance of the type.
   */
  void construct(void* memory) noexcept override {
    if (memory) {
      std::construct_at<Type>(static_cast<Type*>(memory));
    }
  }

  /**
   * @brief Destructs an instance of type @c T located at the given memory location.
   *
   * The behavior is undefined if the memory pointer does not point to a valid instance of type @c T or was already
   * destructed.
   *
   * @param memory A pointer to the memory where the instance should be destructed.
   */
  void destruct(void* memory) noexcept override {
    std::destroy_at<T>(static_cast<T*>(memory));
  }

  /**
   * @brief Creates a new instance of the type by allocating the required memory and constructing the object using its
   * default constructor.
   *
   * It is the caller's responsibility to manage the memory and ensure that it is properly freed when no longer needed.
   *
   * @return A pointer to the allocated memory containing a default-constructed instance of the type.
   */
  void* create() noexcept override {
    void* memory = allocate();
    if (memory) {
      construct(memory);
    }
    return memory;
  }

  /**
   * @brief Destroys the provided instance by calling its destructor and deallocating its memory.
   *
   * The memory pointed to by the parameter should have been allocated by the create() function of this type, and it is
   * the caller's responsibility to ensure that the memory is properly freed when no longer needed.
   *
   * @param memory A pointer to the memory that should be freed. This memory should have been allocated by the create()
   * function of this type.
   */
  void destroy(void* memory) noexcept override {
    if (memory != nullptr) {
      destruct(memory);
      deallocate(memory);
    }
  }

  /**
   * @brief Compares two instances of the type for equality. The behavior is undefined if either pointer does not point
   * to valid instances of the type.
   *
   * @param lhs A pointer to the first instance of the type to compare.
   * @param rhs A pointer to the second instance of the type to compare.
   * @return true if the instances are considered equal according to the type's equality semantics, false otherwise.
   */
  bool equals(const void* lhs, const void* rhs) const noexcept override {
    if (lhs == nullptr || rhs == nullptr) {
      return lhs == rhs;
    }
    return *static_cast<const Type*>(lhs) == *static_cast<const Type*>(rhs);
  }
};

}  // namespace core::rtti
