#pragma once

#include "IName.hpp"
#include "RTTIType.hpp"

namespace core::rtti {
/**
 * @brief Generic implementation base for RTTI descriptors.
 *
 * The @c TBase parameter determines which polymorphic RTTI interface is implemented. This lets concrete descriptors
 * reuse the same value-semantics implementation without forwarding wrappers.
 *
 * @tparam T The underlying type described by this RTTI object.
 * @tparam TBase The polymorphic RTTI base interface being implemented.
 */
template <typename T, typename TBase>
class RTTITType : public TBase {
 public:
  /**
   * @brief Defines a type alias for the underlying type @code T@endcode.
   */
  using Type = T;

  /**
   * @brief Constructs a descriptor that implements @code RTTIType@endcode.
   *
   * @param name The name of the type.
   * @param kind The kind of the type.
   */
  explicit RTTITType(const IName& name, const RTTITypeKind kind)
    requires std::same_as<TBase, RTTIType>
      : TBase(name, sizeof(Type), alignof(Type), kind) {}

  /**
   * @brief Constructs a descriptor that implements @code IFundamentalType@endcode.
   *
   * @param name The name of the type.
   */
  explicit RTTITType(const IName& name)
    requires std::same_as<TBase, RTTIFundamentalType>
      : TBase(name, sizeof(Type), alignof(Type)) {}

  /**
   * @brief Constructs a descriptor that implements @code RTTIClassType@endcode.
   *
   * @param name The name of the type.
   */
  explicit RTTITType(const IName& name)
    requires std::same_as<TBase, RTTIClassType>
      : TBase(name, sizeof(Type), alignof(Type)) {}

  /**
   * @brief Constructs a descriptor that implements @code RTTIContainerType@endcode.
   *
   * @param name The name of the container type.
   * @param inner A pointer to the inner type descriptor.
   */
  explicit RTTITType(const IName& name, const RTTIType* inner)
    requires std::derived_from<TBase, RTTIContainerType>
      : TBase(name, sizeof(Type), alignof(Type), inner) {}

  /**
   * @brief Assigns the value from the source pointer to the destination pointer.
   *
   * Both pointers are expected to point to valid instances of the underlying type @code T@endcode.
   * The behavior is undefined if either pointer does not point to valid instances of the type.
   *
   * @param destination A pointer to the destination instance of the type where the value will be assigned.
   * @param source A pointer to the source instance of the type from which the value will be copied.
   */
  void assign(void* destination, const void* source) const override {
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
   * @return A pointer to the allocated memory for an instance of the type.
   */
  [[nodiscard]] void* allocate() const noexcept override {
    return operator new(sizeof(Type), static_cast<std::align_val_t>(alignof(Type)));
  }

  /**
   * @brief Frees the memory allocated for an instance of type @code T@endcode.
   *
   * The provided memory pointer should have been allocated by @c allocate().
   *
   * @param memory A pointer to the memory that should be freed.
   */
  void deallocate(void* memory) const noexcept override {
    operator delete(memory, sizeof(Type), static_cast<std::align_val_t>(alignof(Type)));
  }

  /**
   * @brief Constructs an instance of type @c T in the provided memory location.
   *
   * The memory should be pre-allocated and large enough to hold an instance of the type.
   *
   * @param memory A pointer to the memory where the instance should be constructed.
   */
  void construct(void* memory) const noexcept override {
    if (memory) {
      std::construct_at<Type>(static_cast<Type*>(memory));
    }
  }

  /**
   * @brief Destructs an instance of type @c T located at the given memory location.
   *
   * @param memory A pointer to the memory where the instance should be destructed.
   */
  void destruct(void* memory) const noexcept override {
    std::destroy_at<Type>(static_cast<Type*>(memory));
  }

  /**
   * @brief Creates a new instance of the type by allocating the required memory and constructing it.
   *
   * @return A pointer to the allocated memory containing a default-constructed instance of the type.
   */
  [[nodiscard]] void* create() const noexcept override {
    void* memory = allocate();
    if (memory) {
      construct(memory);
    }
    return memory;
  }

  /**
   * @brief Destroys the provided instance by calling its destructor and deallocating its memory.
   *
   * @param memory A pointer to the memory that should be freed.
   */
  void destroy(void* memory) const noexcept override {
    if (memory != nullptr) {
      destruct(memory);
      deallocate(memory);
    }
  }

  /**
   * @brief Compares two instances of the type for equality.
   *
   * @param lhs A pointer to the first instance of the type to compare.
   * @param rhs A pointer to the second instance of the type to compare.
   * @return @c true if the instances are considered equal according to the type's equality semantics, @c false
   * otherwise.
   */
  bool equals(const void* lhs, const void* rhs) const noexcept override {
    if (lhs == nullptr || rhs == nullptr) {
      return lhs == rhs;
    }

    if constexpr (requires(const Type& a, const Type& b) { a == b; }) {
      return *static_cast<const Type*>(lhs) == *static_cast<const Type*>(rhs);
    } else {
      return false;
    }
  }
};
}  // namespace core::rtti
