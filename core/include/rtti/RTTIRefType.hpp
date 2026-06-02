#pragma once

#include <memory>

#include "NamePool.hpp"
#include "rtti/RTTIContainerType.hpp"
#include "rtti/RTTIName.hpp"

namespace core::rtti {

/**
 * @brief Interface for reference-counted pointer type descriptors in the RTTI system.
 *
 * This class describes types whose C++ representation is @c std::shared_ptr<T>, allowing the RTTI system
 * to reason about shared ownership semantics through a common interface.
 */
class RTTIRefType : public RTTIContainerType {
 public:
  /**
   * @brief Constructs an @c RTTIRefType with the given name, size, alignment, and inner type descriptor.
   *
   * @param name The interned string name of the reference type.
   * @param size The size of the smart pointer type in bytes.
   * @param alignment The alignment requirement of the smart pointer type in bytes.
   * @param inner A pointer to the @c RTTIType descriptor for the referenced element type.
   */
  RTTIRefType(const Name& name, const std::size_t size, const std::size_t alignment, const RTTIType* inner) noexcept
      : RTTIContainerType(name, size, alignment, inner, RTTITypeKind::REF) {}

  /**
   * @brief Virtual destructor for @code RTTIRefType@endcode.
   */
  ~RTTIRefType() override = default;

  /**
   * @brief Retrieves the raw pointer managed by the @c std::shared_ptr instance at @c instance.
   *
   * The returned pointer is non-owning and shares the lifetime of the @c std::shared_ptr from which it was
   * obtained. If @c instance is @c nullptr, this method returns @code nullptr@endcode.
   *
   * @param instance A pointer to the @c std::shared_ptr<T> instance to query, or @code nullptr@endcode.
   * @return The raw pointer managed by the shared pointer, or @code nullptr@endcode if the shared pointer
   * is empty or @c instance is @code nullptr@endcode.
   */
  virtual void* get(void* instance) const = 0;

  /**
   * @brief Retrieves the raw pointer managed by the @c std::shared_ptr instance at @c instance, cast to @code
   * T*@endcode.
   *
   * This is a typed convenience wrapper around the virtual @c get that forwards to the virtual dispatch and
   * then @c static_cast<T*>s the result.
   *
   * @tparam T The type to which the raw managed pointer is cast.
   * @param instance A pointer to the @c std::shared_ptr<T> instance to query, or @code nullptr@endcode.
   * @return The managed raw pointer cast to @code T*@endcode, or @code nullptr@endcode if the shared pointer
   * is empty or @c instance is @code nullptr@endcode.
   */
  template <typename T>
  T* get(void* instance) const {
    return static_cast<T*>(get(instance));
  }

  /**
   * @brief Resets the @c std::shared_ptr at @c instance, releasing its ownership of the managed object.
   *
   * After a successful call, the shared pointer at @c instance is empty. It is a no-op if @c instance is
   * @code nullptr@endcode.
   *
   * @param instance A pointer to the @c std::shared_ptr<T> instance to reset, or @code nullptr@endcode.
   */
  virtual void reset(void* instance) const = 0;

  /**
   * @brief Returns the number of @c std::shared_ptr instances currently sharing ownership of the managed object.
   *
   * If @c instance is @c nullptr or the shared pointer at @c instance is empty, this method returns @c 0.
   *
   * @param instance A pointer to the @c std::shared_ptr<T> instance to query, or @code nullptr@endcode.
   * @return The strong reference count of the managed object, or @c 0 if @c instance is @c nullptr or the
   * shared pointer is empty.
   */
  virtual long useCount(const void* instance) const = 0;

  /**
   * @brief Swaps the managed objects of two @c std::shared_ptr instances.
   *
   * After a successful call, the shared pointer at @c lhs manages the object previously held by @c rhs, and
   * vice versa. It is a no-op if either @c lhs or @c rhs is @code nullptr@endcode.
   *
   * @param lhs A pointer to the left-hand @c std::shared_ptr<T> instance, or @code nullptr@endcode.
   * @param rhs A pointer to the right-hand @c std::shared_ptr<T> instance, or @code nullptr@endcode.
   */
  virtual void swap(void* lhs, void* rhs) const = 0;
};

/**
 * @brief Concrete RTTI descriptor for @c std::shared_ptr<T> reference types.
 *
 * It implements @c IRefType over @c std::shared_ptr<T> and derives its type name from @c GetPrefixedRTTIName,
 * producing a name such as @c "ref:MyType". The inner type descriptor must satisfy
 * @c TypedInnerDescriptorFor<InnerType, T>.
 *
 * @tparam T The element type held by the @c std::shared_ptr this descriptor represents.
 */
template <typename T>
class TypedRTTIRefType : public TypedRTTIType<std::shared_ptr<T>, RTTIRefType> {
 public:
  /**
   * @brief Type alias for the underlying @c std::shared_ptr type described by this descriptor.
   */
  using Type = std::shared_ptr<T>;

  /**
   * @brief Constructs a @c TRefType with the given inner type descriptor.
   *
   * The type name is automatically derived from @c GetPrefixedRTTIName with @c RTTITypeKind::REF,
   * producing a name such as @c "ref:MyType".
   *
   * @tparam I The concrete inner type descriptor, which must satisfy  @code is_same_element_v<I, T>@endcode.
   * @param inner A pointer to the @c RTTIType descriptor for the element type @code T@endcode.
   */
  template <typename I>
    requires is_same_element_v<I, T>
  explicit TypedRTTIRefType(const I* inner);

  /**
   * @brief Retrieves the raw pointer from the @c std::shared_ptr<T> at @c instance.
   *
   * It returns @code nullptr@endcode if @c instance is @c nullptr or the shared pointer is empty.
   *
   * @param instance A pointer to the @c std::shared_ptr<T> to query, or @code nullptr@endcode.
   * @return The raw pointer managed by the shared pointer, or @code nullptr@endcode.
   */
  void* get(void* instance) const override {
    if (instance) {
      return static_cast<std::shared_ptr<T>*>(instance)->get();
    }
    return nullptr;
  }

  /**
   * @brief Resets the @c std::shared_ptr<T> at @c instance, releasing its managed object.
   *
   * It is a no-op if @c instance is @code nullptr@endcode.
   *
   * @param instance A pointer to the @c std::shared_ptr<T> to reset, or @code nullptr@endcode.
   */
  void reset(void* instance) const override {
    if (instance) {
      static_cast<std::shared_ptr<T>*>(instance)->reset();
    }
  }

  /**
   * @brief Returns the strong reference count of the @c std::shared_ptr<T> at @c instance.
   *
   * It returns @c 0 if @c instance is @c nullptr or the shared pointer is empty.
   *
   * @param instance A pointer to the @c std::shared_ptr<T> to query, or @code nullptr@endcode.
   * @return The use count of the managed object, or @c 0 if @c instance is @code nullptr@endcode.
   */
  long useCount(const void* instance) const override {
    if (instance) {
      return static_cast<const std::shared_ptr<T>*>(instance)->use_count();
    }
    return 0;
  }

  /**
   * @brief Swaps the managed objects of the two @c std::shared_ptr<T> instances.
   *
   * It is a no-op if either @c lhs or @c rhs is @code nullptr@endcode.
   *
   * @param lhs A pointer to the left-hand @c std::shared_ptr<T>, or @code nullptr@endcode.
   * @param rhs A pointer to the right-hand @c std::shared_ptr<T>, or @code nullptr@endcode.
   */
  void swap(void* lhs, void* rhs) const override {
    if (lhs && rhs) {
      static_cast<std::shared_ptr<T>*>(lhs)->swap(*static_cast<std::shared_ptr<T>*>(rhs));
    }
  }
};

template <typename T>
template <typename InnerType>
  requires is_same_element_v<InnerType, T>
TypedRTTIRefType<T>::TypedRTTIRefType(const InnerType* inner)
    : TypedRTTIType<std::shared_ptr<T>, RTTIRefType>(
          NamePool::get().addName(GetPrefixedRTTIName<RTTITypeKind::REF, T>()), static_cast<const RTTIType*>(inner)) {}
}  // namespace core::rtti
