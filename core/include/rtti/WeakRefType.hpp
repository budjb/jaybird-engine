#pragma once
#include <memory>

#include "ContainerType.hpp"
#include "INamePool.hpp"
#include "TypeName.hpp"

namespace core::rtti {

/**
 * @brief Interface for weak reference pointer type descriptors in the RTTI system.
 *
 * This class describes types whose C++ representation is @c std::weak_ptr<T>, allowing the RTTI
 * system to reason about non-owning, expirable references through a common interface.
 */
class IWeakRefType : public IContainerType {
 public:
  /**
   * @brief Constructs an @c IWeakRefType with the given name, size, alignment, and inner type descriptor.
   *
   * @param name The interned string name of the weak reference type.
   * @param size The size of the weak pointer type in bytes.
   * @param alignment The alignment requirement of the weak pointer type in bytes.
   * @param inner A pointer to the @c IType descriptor for the referenced element type.
   */
  explicit IWeakRefType(const IName& name, const std::size_t size, const std::size_t alignment,
                        const IType* inner) noexcept
      : IContainerType(name, size, alignment, inner, TypeKind::WEAK_REF) {}

  /**
   * @brief Virtual destructor for @code IWeakRefType@endcode.
   */
  ~IWeakRefType() override = default;

  /**
   * @brief Resets the @c std::weak_ptr at @c instance, clearing its reference to the managed object.
   *
   * After a successful call, the weak pointer at @c instance is empty and expired. It is a no-op if
   * @c instance is @code nullptr@endcode. This does not affect the strong reference count of the managed
   * object.
   *
   * @param instance A pointer to the @c std::weak_ptr<T> instance to reset, or @code nullptr@endcode.
   */
  virtual void reset(void* instance) const = 0;

  /**
   * @brief Swaps the managed references of two @c std::weak_ptr instances.
   *
   * After a successful call, the weak pointer at @c lhs refers to the object previously referenced by @c rhs,
   * and vice versa. It is a no-op if either @c lhs or @c rhs is @code nullptr@endcode.
   *
   * @param lhs A pointer to the left-hand @c std::weak_ptr<T> instance, or @code nullptr@endcode.
   * @param rhs A pointer to the right-hand @c std::weak_ptr<T> instance, or @code nullptr@endcode.
   */
  virtual void swap(void* lhs, void* rhs) const = 0;

  /**
   * @brief Checks whether the managed object has been deleted.
   *
   * A weak pointer is expired if its managed object has no remaining strong owners, or if the weak pointer has
   * never been assigned ownership. Expired weak pointers lock to @code nullptr@endcode.
   *
   * @param instance A pointer to the @c std::weak_ptr<T> instance to check, or @code nullptr@endcode.
   * @return @c true if the weak pointer is expired or @c instance is @code nullptr@endcode; @c false if the
   * managed object still exists.
   */
  virtual bool expired(const void* instance) const = 0;

  /**
   * @brief Locks the @c std::weak_ptr at @c instance and stores the result in @c outSharedPtr.
   *
   * If the weak pointer is not expired, @c outSharedPtr receives ownership of the managed object and its use
   * count is incremented. If the weak pointer is expired or @c instance is @code nullptr@endcode, @c outSharedPtr
   * is set to an empty @c std::shared_ptr<T>.
   *
   * @param instance A pointer to the @c std::weak_ptr<T> instance to lock, or @code nullptr@endcode.
   * @param outSharedPtr A pointer to the @c std::shared_ptr<T> to receive the result. It is a no-op if
   * @code nullptr@endcode.
   */
  virtual void lock(const void* instance, void* outSharedPtr) const = 0;
};

/**
 * @brief Concrete RTTI descriptor for @c std::weak_ptr<T> weak reference types.
 *
 * It implements @c IWeakRefType over @c std::weak_ptr<T> and derives its type name from
 * @c GetPrefixedTypeName with @c TypeKind::WEAK_REF, producing a name such as @c "wref:MyType".
 * The inner type descriptor must satisfy @c TypedInnerDescriptorFor<InnerType, T>.
 *
 * Because @c std::weak_ptr<T> does not define @c operator==, this class provides its own
 * @c equals override that locks both weak pointers and compares the resulting @code std::shared_ptr@endcode
 * instances.
 *
 * @tparam T The element type referenced by the @c std::weak_ptr this descriptor represents.
 */
template <typename T>
class TWeakRefType : public TType<std::weak_ptr<T>, IWeakRefType> {
 public:
  /**
   * @brief Type alias for the underlying @c std::weak_ptr type described by this descriptor.
   */
  using Type = std::weak_ptr<T>;

  /**
   * @brief Constructs a @c TWeakRefType with the given inner type descriptor.
   *
   * The type name is automatically derived from @c GetPrefixedTypeName with @c TypeKind::WEAK_REF,
   * producing a name such as @c "wref:MyType".
   *
   * @tparam InnerType The concrete inner type descriptor, which must satisfy
   *   @c TypedInnerDescriptorFor<InnerType, T>.
   * @param inner A pointer to the @c IType descriptor for the element type @code T@endcode.
   */
  template <typename InnerType>
    requires TypedInnerDescriptorFor<InnerType, T>
  explicit TWeakRefType(const InnerType* inner) noexcept;

  /**
   * @brief Compares two @c std::weak_ptr instances for equality by locking both and comparing
   * the resulting @code std::shared_ptr@endcode values.
   *
   * Two expired weak pointers are considered equal (both lock to @c nullptr). Two non-expired
   * weak pointers are equal only if they refer to the same managed object. A null @c void* argument
   * is distinct from an expired weak pointer: if either argument is @c nullptr, the comparison
   * follows the same null-pointer semantics as @code TType@endcode.
   *
   * @param lhs A pointer to the left-hand @c std::weak_ptr instance, or @c nullptr.
   * @param rhs A pointer to the right-hand @c std::weak_ptr instance, or @c nullptr.
   * @return @c true if both weak pointers lock to the same managed object (or both to @c nullptr),
   * @c false otherwise.
   */
  bool equals(const void* lhs, const void* rhs) const noexcept override;

  /**
   * @brief Resets the @c std::weak_ptr<T> at @c instance, clearing its reference.
   *
   * It is a no-op if @c instance is @code nullptr@endcode. Resetting a weak pointer does not affect the strong
   * reference count of any managed object.
   *
   * @param instance A pointer to the @c std::weak_ptr<T> to reset, or @code nullptr@endcode.
   */
  void reset(void* instance) const override {
    if (instance) {
      static_cast<std::weak_ptr<T>*>(instance)->reset();
    }
  }

  /**
   * @brief Swaps the managed references of the two @c std::weak_ptr<T> instances.
   *
   * It is a no-op if either @c lhs or @c rhs is @code nullptr@endcode.
   *
   * @param lhs A pointer to the left-hand @c std::weak_ptr<T>, or @code nullptr@endcode.
   * @param rhs A pointer to the right-hand @c std::weak_ptr<T>, or @code nullptr@endcode.
   */
  void swap(void* lhs, void* rhs) const override {
    if (lhs && rhs) {
      static_cast<std::weak_ptr<T>*>(lhs)->swap(*static_cast<std::weak_ptr<T>*>(rhs));
    }
  }

  /**
   * @brief Checks whether the @c std::weak_ptr<T> at @c instance is expired.
   *
   * A weak pointer is expired if its managed object has been destroyed or if the weak pointer was never assigned
   * ownership. Expired weak pointers lock to @code nullptr@endcode.
   *
   * @param instance A pointer to the @c std::weak_ptr<T> to check, or @code nullptr@endcode.
   * @return @c true if the weak pointer is expired or @c instance is @code nullptr@endcode; @c false if the
   * managed object still exists.
   */
  bool expired(const void* instance) const override {
    if (instance) {
      return static_cast<const std::weak_ptr<T>*>(instance)->expired();
    }
    return true;
  }

  /**
   * @brief Attempts to lock the @c std::weak_ptr<T> at @c instance and store the result in @c outSharedPtr.
   *
   * If the weak pointer is not expired, @c outSharedPtr receives a strong reference to the managed object. If the
   * weak pointer is expired or @c instance is @code nullptr@endcode, @c outSharedPtr is set to an empty
   * @code std::shared_ptr<T>@endcode. If @c outSharedPtr is @code nullptr@endcode, this is a no-op.
   *
   * @param instance A pointer to the @c std::weak_ptr<T> to lock, or @code nullptr@endcode.
   * @param outSharedPtr A pointer to the @c std::shared_ptr<T> to receive the locked pointer, or
   * @code nullptr@endcode for a no-op.
   */
  void lock(const void* instance, void* outSharedPtr) const override {
    if (instance && outSharedPtr) {
      const auto locked = static_cast<const std::weak_ptr<T>*>(instance)->lock();
      *static_cast<std::shared_ptr<T>*>(outSharedPtr) = std::move(locked);
    }
  }
};

template <typename T>
template <typename InnerType>
  requires TypedInnerDescriptorFor<InnerType, T>
TWeakRefType<T>::TWeakRefType(const InnerType* inner) noexcept
    : TType<std::weak_ptr<T>, IWeakRefType>(INamePool::get().addName(GetPrefixedTypeName<TypeKind::WEAK_REF, T>()),
                                            static_cast<const IType*>(inner)) {}

template <typename T>
bool TWeakRefType<T>::equals(const void* lhs, const void* rhs) const noexcept {
  if (lhs == nullptr || rhs == nullptr) {
    return lhs == rhs;
  }
  const auto slhs = static_cast<const std::weak_ptr<T>*>(lhs)->lock();
  const auto srhs = static_cast<const std::weak_ptr<T>*>(rhs)->lock();
  return slhs == srhs;
}
}  // namespace core::rtti
