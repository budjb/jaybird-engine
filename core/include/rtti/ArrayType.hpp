#pragma once

#include "ContainerType.hpp"
#include "Export.hpp"
#include "INamePool.hpp"
#include "Iterator.hpp"
#include "TypeName.hpp"
#include "Vector.hpp"

namespace core::rtti {
/**
 * @brief Interface for array type descriptors that provides methods for accessing and manipulating array elements, as
 * well as querying array properties such as length and capacity.
 */
class JAYBIRD_API IArrayType : public IContainerType {
 public:
  /**
   * @brief Constructs an IArrayType with the given name, size, alignment, and inner type descriptor.
   *
   * @param name The interned string name of the array type.
   * @param size The size of the array type in bytes.
   * @param alignment The alignment requirement of the array type in bytes.
   * @param inner A pointer to the IType descriptor for the elements contained in the array.
   */
  explicit IArrayType(const IName& name, std::size_t size, std::size_t alignment, const IType* inner) noexcept;

  /**
   * @brief Virtual destructor for IArrayType.
   */
  ~IArrayType() override = default;

  /**
   * @brief Returns the number of elements currently stored in the provided array.
   *
   * @param array A pointer to the array instance for which to retrieve the length.
   * @return The number of elements currently stored in the array.
   */
  [[nodiscard]] virtual std::size_t length(const void* array) const noexcept = 0;

  /**
   * @brief Returns the total number of elements that the provided array can hold without requiring a resize.
   *
   * @param array A pointer to the array instance for which to retrieve the capacity.
   * @return The total number of elements that the array can hold without requiring a resize.
   */
  [[nodiscard]] virtual std::size_t capacity(const void* array) const noexcept = 0;

  /**
   * @brief Returns the maximum number of elements that the provided array can hold, which may be limited by system or
   * implementation constraints.
   *
   * @param array A pointer to the array instance for which to retrieve the maximum length.
   * @return The maximum number of elements that the array can hold, which may be limited by system or implementation
   * constraints.
   */
  [[nodiscard]] virtual std::size_t maxLength(const void* array) const noexcept = 0;

  /**
   * @brief Returns a pointer to the element at the specified index in the provided array.
   *
   * @param array A pointer to the array instance from which to retrieve the element.
   * @param index The index of the element to retrieve, which must be within the bounds of the array's length.
   * @return A pointer to the element at the specified index in the array, or nullptr if the index is out of bounds.
   */
  [[nodiscard]] virtual void* at(const void* array, std::size_t index) = 0;

  /**
   * @brief Returns a const pointer to the element at the specified index in the provided array.
   *
   * @param array A pointer to the array instance from which to retrieve the element.
   * @param index The index of the element to retrieve, which must be within the bounds of the array's length.
   * @return A const pointer to the element at the specified index in the array, or nullptr if the index is out of
   * bounds.
   */
  [[nodiscard]] virtual const void* at(const void* array, std::size_t index) const = 0;

  /**
   * @brief Template method that returns a pointer to the element at the specified index, cast to a type.
   *
   * The caller is responsible for ensuring the templated type matches the actual array element type. Type mismatches
   * lead to undefined behavior including memory corruption, crashes, or incorrect data access.
   *
   * @tparam T The type to which the element pointer should be cast.
   * @param array A pointer to the array instance from which to retrieve the element.
   * @param index The index of the element to retrieve, which must be within bounds.
   * @return A pointer to the element at the specified index, cast to type @c T, or nullptr if out of bounds.
   */
  template <typename T>
  [[nodiscard]] T* at(const void* array, const std::size_t index) {
    return static_cast<T*>(at(array, index));
  }

  /**
   * @brief Template method that returns a const pointer to the element at the specified index, cast to a type.
   *
   * The caller is responsible for ensuring the templated type matches the actual array element type. Type mismatches
   * lead to undefined behavior including memory corruption, crashes, or incorrect data access.
   *
   * @tparam T The type to which the element pointer should be cast.
   * @param array A pointer to the array instance from which to retrieve the element.
   * @param index The index of the element to retrieve, which must be within bounds.
   * @return A const pointer to the element at the specified index, cast to type @c T, or nullptr if out of bounds.
   */
  template <typename T>
  [[nodiscard]] const T* at(const void* array, const std::size_t index) const {
    return static_cast<const T*>(at(array, index));
  }

  /**
   * @brief Returns a pointer to the first element in the provided array, or nullptr if the array is empty.
   *
   * @param array A pointer to the array instance from which to retrieve the first element.
   * @return A pointer to the first element in the array, or nullptr if the array is empty.
   */
  [[nodiscard]] virtual void* front(const void* array) = 0;

  /**
   * @brief Returns a const pointer to the first element in the provided array, or nullptr if the array is empty.
   *
   * @param array A pointer to the array instance from which to retrieve the first element.
   * @return A const pointer to the first element in the array, or nullptr if the array is empty.
   */
  [[nodiscard]] virtual const void* front(const void* array) const = 0;

  /**
   * @brief Returns a pointer to the last element in the provided array, or nullptr if the array is empty.
   *
   * @param array A pointer to the array instance from which to retrieve the last element.
   * @return A pointer to the last element in the array, or nullptr if the array is empty.
   */
  [[nodiscard]] virtual void* back(const void* array) = 0;

  /**
   * @brief Returns a const pointer to the last element in the provided array, or nullptr if the array is empty.
   *
   * @param array A pointer to the array instance from which to retrieve the last element.
   * @return A const pointer to the last element in the array, or nullptr if the array is empty.
   */
  [[nodiscard]] virtual const void* back(const void* array) const = 0;

  /**
   * @brief Returns a type-erased iterator pointing to the first element in the provided array, or an iterator equal to
   * end() if the array is empty.
   *
   * @param array A pointer to the array instance from which to retrieve the iterator.
   * @return An iterator pointing to the first element in the array, or an iterator equal to end() if the array is
   * empty.
   */
  [[nodiscard]] virtual Iterator<> begin(void* array) noexcept = 0;

  /**
   * @brief Template method that returns an iterator pointing to the first element, cast to a type.
   *
   * The caller is responsible for ensuring the templated type matches the actual array element type. Type mismatches
   * lead to undefined behavior.
   *
   * @tparam T The type to which the iterator should be cast.
   * @param array A pointer to the array instance from which to retrieve the iterator.
   * @return A typed iterator pointing to the first element, or equal to end() if empty.
   */
  template <typename T>
  [[nodiscard]] Iterator<T> begin(void* array) noexcept {
    return Iterator<T>(begin(static_cast<T*>(array)));
  }

  /**
   * @brief Returns a type-erased iterator to one past the last element, serving as a sentinel.
   *
   * The returned iterator should only be used for comparison and should not be
   * dereferenced, as this leads to undefined behavior.
   *
   * @param array A pointer to the array instance from which to retrieve the end iterator.
   * @return An iterator pointing to one past the last element in the array.
   */
  [[nodiscard]] virtual Iterator<> end(void* array) noexcept = 0;

  /**
   * @brief Template method that returns a typed iterator to one past the last element, serving as a sentinel.
   *
   * The caller is responsible for ensuring the templated type matches the actual array element type. The returned
   * iterator should only be used for comparison.
   *
   * @tparam T The element type to which the iterator should be cast.
   * @param array A pointer to the array instance from which to retrieve the end iterator.
   * @return A typed iterator pointing to one past the last element in the array.
   */
  template <typename T>
  [[nodiscard]] Iterator<T> end(void* array) noexcept {
    return Iterator<T>(end(static_cast<T*>(array)));
  }

  /**
   * @brief Returns a type-erased reverse iterator pointing to the last element.
   *
   * The returned iterator serves as a sentinel for reverse iteration. Returns a
   * reverse end iterator if the array is empty.
   *
   * @param array A pointer to the array instance from which to retrieve the reverse iterator.
   * @return A reverse iterator pointing to the last element, or equal to rend() if empty.
   */
  [[nodiscard]] ReverseIterator<> rbegin(void* array) noexcept;

  /**
   * @brief Template method that returns a reverse iterator pointing to the last element, cast to a type.
   *
   * The caller is responsible for ensuring the templated type matches the actual array element type. Type mismatches
   * lead to undefined behavior.
   *
   * @tparam T The type to which the reverse iterator should be cast.
   * @param array A pointer to the array instance from which to retrieve the reverse iterator.
   * @return A reverse iterator pointing to the last element, or equal to rend() if empty.
   */
  template <typename T>
  [[nodiscard]] ReverseIterator<T> rbegin(void* array) noexcept {
    return ReverseIterator<T>(rbegin(static_cast<T*>(array)));
  }

  /**
   * @brief Returns a type-erased reverse iterator to one before the first element, serving as a sentinel.
   *
   * The returned iterator should only be used for comparison and should not be
   * dereferenced, as this leads to undefined behavior.
   *
   * @param array A pointer to the array instance from which to retrieve the reverse end iterator.
   * @return A reverse iterator pointing to one before the first element.
   */
  [[nodiscard]] ReverseIterator<> rend(void* array) noexcept;

  /**
   * @brief Template method that returns a reverse iterator to one before the first element, cast to a type.
   *
   * The caller is responsible for ensuring the templated type matches the actual array element type. The returned
   * iterator should only be used for comparison.
   *
   * @tparam T The type to which the reverse end iterator should be cast.
   * @param array A pointer to the array instance from which to retrieve the reverse end iterator.
   * @return A reverse iterator pointing to one before the first element, cast to type @c T.
   */
  template <typename T>
  [[nodiscard]] ReverseIterator<T> rend(void* array) noexcept {
    return ReverseIterator<T>(rend(static_cast<T*>(array)));
  }

  /**
   * @brief Removes the element at the specified index from the provided array, shifting subsequent elements to fill the
   * gap.
   *
   * @param array A pointer to the array instance from which to remove the element.
   * @param index The index of the element to remove, which must be within the bounds of the array's length.
   */
  virtual void erase(const void* array, std::size_t index) = 0;

  /**
   * @brief Inserts a new element with the specified value at the specified index, shifting subsequent elements.
   *
   * @param array A pointer to the array instance into which to insert the new element.
   * @param index The index at which to insert, within bounds or equal to the array's length.
   * @param value A pointer to a properly constructed value of the same type as array elements.
   */
  virtual void insert(const void* array, std::size_t index, const void* value) = 0;

  /**
   * @brief Appends a new element with the specified value to the end of the array.
   *
   * @param array A pointer to the array instance to which to append the new element.
   * @param value A pointer to a properly constructed value of the same type as array elements.
   */
  virtual void pushBack(const void* array, const void* value) = 0;

  /**
   * @brief Removes the last element from the provided array, reducing the array's length by one.
   *
   * @param array A pointer to the array instance from which to remove the last element.
   */
  virtual void popBack(const void* array) = 0;

  /**
   * @brief Removes the element at the specified index from the provided array, replacing it with the last element in
   * the array and reducing the array's length by one.
   *
   * @param array A pointer to the array instance from which to remove the element.
   * @param index The index of the element to remove, which must be within the bounds of the array's length.
   */
  virtual void remove(const void* array, std::size_t index) = 0;

  /**
   * @brief Replaces the element at the specified index in the provided array with the specified value, without changing
   * the array's length.
   *
   * @param array A pointer to the array instance in which to replace the element.
   * @param index The index of the element to replace, which must be within the bounds of the array's length.
   * @param value A pointer to the value to be used for replacement, which must be of the same type as the elements
   * contained in the array. The caller is responsible for ensuring that the value is properly constructed and valid for
   * insertion into the array.
   */
  virtual void replace(const void* array, std::size_t index, const void* value) = 0;

  /**
   * @brief Reserves enough memory to hold at least the specified number of elements in the provided array, without
   * changing the array's length. If the current capacity of the array is already sufficient to hold the specified
   * number of elements, this method may do nothing.
   *
   * @param array A pointer to the array instance for which to reserve capacity.
   * @param size The number of elements for which to reserve capacity, which must be greater than or equal to the
   * current length of the array.
   */
  virtual void reserve(void* array, std::size_t size) = 0;

  /**
   * @brief Resizes the provided array to hold exactly the specified number of elements. If the new size is greater than
   * the current length of the array, new elements will be default-constructed. If the new size is less than the current
   * length of the array, excess elements will be destroyed to reduce the array's length.
   *
   * @param array A pointer to the array instance to resize.
   * @param size The new number of elements that the array should hold, which must be greater than or equal to zero.
   */
  virtual void resize(void* array, std::size_t size) = 0;

  /**
   * @brief Reduces the capacity of the provided array to match its current length, freeing any excess memory that is
   * not needed to hold the existing elements. This can be used to optimize memory usage after a series of insertions
   * and deletions that may have left the array with more capacity than necessary.
   *
   * After calling this method, the capacity of the array will be equal to its length, and any attempts to insert new
   * elements beyond the current length will require a resize operation to increase the capacity again.
   *
   * @param array A pointer to the array instance for which to shrink the capacity to fit the current length.
   */
  virtual void shrinkToFit(void* array) = 0;

  /**
   * @brief Removes all elements from the provided array, resetting its length to zero while maintaining its capacity.
   * This can be used to quickly clear the contents of the array without deallocating memory, allowing for efficient
   * reuse of the array instance for future insertions.
   *
   * @param array A pointer to the array instance to clear. After calling this method, the array will be empty, but its
   * capacity will remain unchanged, allowing for efficient reuse of the array instance for future insertions.
   */
  virtual void clear(void* array) = 0;
};

/**
 * @brief A template class representing an array type descriptor in the RTTI system. This class provides methods for
 * accessing and manipulating array elements, as well as querying array properties such as length and capacity. The
 * TArrayType class is designed to work with @c Vector as the underlying container for the array elements, and it
 * requires that the inner type descriptor provided to its constructor is compatible with the element type of the array.
 *
 * @tparam T The type of the elements in the array. This type must be compatible with the inner type descriptor provided
 * to the constructor, as enforced by the TypedInnerDescriptorFor concept.
 */
template <typename T>
class TArrayType : public TType<Vector<T>, IArrayType> {
 public:
  /**
   * @brief Defines a type alias for the underlying array type, which is @c Vector<T>. This allows users of TArrayType
   * to refer to the underlying array type using TArrayType<T>::Type.
   */
  using Type = Vector<T>;

  /**
   * @brief Constructs a TArrayType with the given name and inner type descriptor. The size and alignment are
   * automatically determined based on the underlying array type, and the kind is set to TypeKind::ARRAY. The inner type
   * descriptor must be compatible with the element type T, as enforced by the TypedInnerDescriptorFor concept.
   *
   * @tparam InnerType The type of the inner type descriptor, which must satisfy the TypedInnerDescriptorFor concept
   * with respect to T.
   * @param inner A pointer to the IType descriptor for the elements contained in the array. This must be compatible
   * with the element type T.
   */
  template <typename InnerType>
    requires is_same_element_v<InnerType, T>
  explicit TArrayType(const InnerType* inner)
      : TType<Vector<T>, IArrayType>(INamePool::get().addName(GetPrefixedTypeName<TypeKind::ARRAY, T>()),
                                     static_cast<const IType*>(inner)) {}

  /**
   * @brief Returns the number of elements currently stored in the array pointed to by the parameter. The behavior is
   * undefined if the pointer does not point to a valid instance of the array type.
   *
   * @param array A pointer to the array instance for which to retrieve the length.
   * @return The number of elements currently stored in the array, or 0 if the pointer is null.
   */
  [[nodiscard]] std::size_t length(const void* array) const noexcept override {
    if (array != nullptr) {
      return static_cast<const Type*>(array)->size();
    }
    return 0;
  }

  /**
   * @brief Returns the total number of elements that the array pointed to by the parameter can hold without requiring a
   * resize. The behavior is undefined if the pointer does not point to a valid instance of the array type.
   *
   * @param array A pointer to the array instance for which to retrieve the capacity.
   * @return The total number of elements that the array can hold without requiring a resize, or 0 if the pointer is
   * null.
   */
  [[nodiscard]] std::size_t capacity(const void* array) const noexcept override {
    if (array != nullptr) {
      return static_cast<const Type*>(array)->capacity();
    }
    return 0;
  }

  /**
   * @brief Returns the maximum number of elements that the array pointed to by the parameter can hold, which may be
   * limited by system or implementation constraints. The behavior is undefined if the pointer does not point to a valid
   * instance of the array type.
   *
   * @param array A pointer to the array instance for which to retrieve the maximum length.
   * @return The maximum number of elements that the array can hold, which may be limited by system or implementation
   * constraints, or 0 if the pointer is null.
   */
  [[nodiscard]] std::size_t maxLength(const void* array) const noexcept override {
    if (array != nullptr) {
      return static_cast<const Type*>(array)->maxSize();
    }
    return 0;
  }

  /**
   * @brief Returns a pointer to the element at the specified index in the array pointed to by the parameter. The
   * behavior is undefined if the pointer does not point to a valid instance of the array type or if the index is out of
   * bounds for the array. The returned pointer is cast to void* and should be interpreted as a pointer to the element
   * type T.
   *
   * @param array A pointer to the array instance from which to retrieve the element.
   * @param index The index of the element to retrieve, which must be within the bounds of the array's length.
   * @return A pointer to the element at the specified index in the array, cast to void*, or nullptr if the pointer is
   * null or if the index is out of bounds.
   */
  [[nodiscard]] void* at(const void* array, std::size_t index) override {
    if (array != nullptr) {
      auto* vector = static_cast<Type*>(const_cast<void*>(array));
      if (index < vector->size()) {
        return &(*vector)[index];
      }
    }
    return nullptr;
  }

  /**
   * @brief Returns a const pointer to the element at the specified index in the array pointed to by the parameter. The
   * behavior is undefined if the pointer does not point to a valid instance of the array type or if the index is out of
   * bounds for the array. The returned pointer is cast to const void* and should be interpreted as a pointer to the
   * element type T.
   *
   * @param array A pointer to the array instance from which to retrieve the element.
   * @param index The index of the element to retrieve, which must be within the bounds of the array's length.
   * @return A const pointer to the element at the specified index in the array, cast to const void*, or nullptr if the
   * pointer is null or if the index is out of bounds.
   */
  [[nodiscard]] const void* at(const void* array, std::size_t index) const override {
    if (array != nullptr) {
      const auto* vector = static_cast<const Type*>(array);
      if (index < vector->size()) {
        return &(*vector)[index];
      }
    }
    return nullptr;
  }

  /**
   * @brief Returns a pointer to the first element in the array pointed to by the parameter, or nullptr if the array is
   * empty. The behavior is undefined if the pointer does not point to a valid instance of the array type. The returned
   * pointer is cast to void* and should be interpreted as a pointer to the element type T.
   *
   * @param array A pointer to the array instance from which to retrieve the first element.
   * @return A pointer to the first element in the array, cast to void*, or nullptr if the pointer is null or if the
   * array is empty.
   */
  [[nodiscard]] void* front(const void* array) override {
    if (array != nullptr) {
      auto* vector = static_cast<Type*>(const_cast<void*>(array));
      if (!vector->empty()) {
        return &vector->front();
      }
    }
    return nullptr;
  }

  /**
   * @brief Returns a const pointer to the first element in the array pointed to by the parameter, or nullptr if the
   * array is empty. The behavior is undefined if the pointer does not point to a valid instance of the array type. The
   * returned pointer is cast to const void* and should be interpreted as a pointer to the element type T.
   *
   * @param array A pointer to the array instance from which to retrieve the first element.
   * @return A const pointer to the first element in the array, cast to const void*, or nullptr if the pointer is null
   * or if the array is empty.
   */
  [[nodiscard]] const void* front(const void* array) const override {
    if (array != nullptr) {
      const auto* vector = static_cast<const Type*>(array);
      if (!vector->empty()) {
        return &vector->front();
      }
    }
    return nullptr;
  }

  /**
   * @brief Returns a pointer to the last element in the array pointed to by the parameter, or nullptr if the array is
   * empty. The behavior is undefined if the pointer does not point to a valid instance of the array type. The returned
   * pointer is cast to void* and should be interpreted as a pointer to the element type T.
   *
   * @param array A pointer to the array instance from which to retrieve the last element.
   * @return A pointer to the last element in the array, cast to void*, or nullptr if the pointer is null or if the
   * array is empty.
   */
  [[nodiscard]] void* back(const void* array) override {
    if (array != nullptr) {
      auto* vector = static_cast<Type*>(const_cast<void*>(array));
      if (!vector->empty()) {
        return &vector->back();
      }
    }
    return nullptr;
  }

  /**
   * @brief Returns a const pointer to the last element in the array pointed to by the parameter, or nullptr if the
   * array is empty. The behavior is undefined if the pointer does not point to a valid instance of the array type. The
   * returned pointer is cast to const void* and should be interpreted as a pointer to the element type T.
   *
   * @param array A pointer to the array instance from which to retrieve the last element.
   * @return A const pointer to the last element in the array, cast to const void*, or nullptr if the pointer is null or
   * if the array is empty.
   */
  [[nodiscard]] const void* back(const void* array) const override {
    if (array != nullptr) {
      const auto* vector = static_cast<const Type*>(array);
      if (!vector->empty()) {
        return &vector->back();
      }
    }
    return nullptr;
  }

  /**
   * @brief Returns a type-erased iterator pointing to the first element in the array pointed to by the parameter, or an
   * iterator equal to end() if the array is empty. The behavior is undefined if the pointer does not point to a valid
   * instance of the array type. The returned iterator is type-erased and should be interpreted as an iterator over
   * elements of type T.
   *
   * @param ptr A pointer to the array instance from which to retrieve the iterator.
   * @return An iterator pointing to the first element in the array, or an iterator equal to end() if the array is
   * empty.
   */
  Iterator<> begin(void* ptr) noexcept override {
    if (ptr == nullptr) {
      return Iterator<>(this->inner()->size(), nullptr);
    }
    auto* vec = static_cast<Type*>(ptr);
    return Iterator<>(this->inner()->size(), static_cast<void*>(vec->data()));
  }

  /**
   * @brief Returns a type-erased iterator pointing to one past the last element in the array pointed to by the
   * parameter, which serves as a sentinel value for the end of the array. The behavior is undefined if the pointer does
   * not point to a valid instance of the array type. The returned iterator is type-erased and should be interpreted as
   * an iterator over elements of type T.
   *
   * @param ptr A pointer to the array instance from which to retrieve the end iterator.
   * @return An iterator pointing to one past the last element in the array, which serves as a sentinel value for the
   * end of the array.
   */
  Iterator<> end(void* ptr) noexcept override {
    if (ptr == nullptr) {
      return Iterator<>(this->inner()->size(), nullptr);
    }
    auto* vec = static_cast<Type*>(ptr);
    return Iterator<>(this->inner()->size(), static_cast<void*>(vec->data() + vec->size()));
  }

  /**
   * @brief Removes the element at the specified index from the provided array, shifting subsequent elements to fill the
   * gap. The behavior is undefined if the pointer does not point to a valid instance of the array type or if the index
   * is out of bounds for the array.
   *
   * @param array A pointer to the array instance from which to remove the element.
   * @param index The index of the element to remove, which must be within the bounds of the array's length.
   */
  void erase(const void* array, const std::size_t index) override {
    if (array != nullptr) {
      auto* vector = static_cast<Type*>(const_cast<void*>(array));
      vector->erase(vector->begin() + static_cast<std::ptrdiff_t>(index));
    }
  }

  /**
   * @brief Inserts a new element with the specified value at the specified index in the provided array, shifting
   * subsequent elements to make room for the new element. The behavior is undefined if the pointer does not point to a
   * valid instance of the array type, if the index is out of bounds for the array, or if the value pointer does not
   * point to a valid instance of the element type T.
   *
   * @param array A pointer to the array instance into which to insert the new element.
   * @param index The index at which to insert the new element, which must be within the bounds of the array's length or
   * equal to the array's length to insert at the end.
   * @param value A pointer to the value to be inserted, which must be of the same type as the elements contained in the
   * array. The caller is responsible for ensuring that the value is properly constructed and valid for insertion into
   * the array.
   */
  void insert(const void* array, const std::size_t index, const void* value) override {
    if (array != nullptr && value != nullptr) {
      auto* vector = static_cast<Type*>(const_cast<void*>(array));
      vector->insert(vector->begin() + static_cast<std::ptrdiff_t>(index), *static_cast<const T*>(value));
    }
  }

  /**
   * @brief Appends a new element with the specified value to the end of the provided array, increasing the array's
   * length by one. The behavior is undefined if the pointer does not point to a valid instance of the array type or if
   * the value pointer does not point to a valid instance of the element type T.
   *
   * @param array A pointer to the array instance to which to append the new element.
   * @param value A pointer to the value to be appended, which must be of the same type as the elements contained in the
   * array. The caller is responsible for ensuring that the value is properly constructed and valid for insertion into
   * the array.
   */
  void pushBack(const void* array, const void* value) override {
    if (array != nullptr && value != nullptr) {
      static_cast<Type*>(const_cast<void*>(array))->pushBack(*static_cast<const T*>(value));
    }
  }

  /**
   * @brief Removes the last element from the provided array, reducing the array's length by one. The behavior is
   * undefined if the pointer does not point to a valid instance of the array type or if the array is empty.
   *
   * @param array A pointer to the array instance from which to remove the last element.
   */
  void popBack(const void* array) override {
    if (array != nullptr) {
      if (auto* vector = static_cast<Type*>(const_cast<void*>(array)); !vector->empty()) {
        vector->popBack();
      }
    }
  }

  /**
   * @brief Removes the element at the specified index from the provided array, replacing it with the last element in
   * the array and reducing the array's length by one. The behavior is undefined if the pointer does not point to a
   * valid instance of the array type, if the index is out of bounds for the array, or if the array is empty.
   *
   * @param array A pointer to the array instance from which to remove the element.
   * @param index The index of the element to remove, which must be within the bounds of the array's length.
   */
  void remove(const void* array, const std::size_t index) override {
    if (array != nullptr) {
      auto* vector = static_cast<Type*>(const_cast<void*>(array));
      if (index < vector->size()) {
        if (index + 1 < vector->size()) {
          (*vector)[index] = vector->back();
        }
        vector->popBack();
      }
    }
  }

  /**
   * @brief Replaces the element at the specified index in the provided array with the specified value, without changing
   * the array's length. The behavior is undefined if the pointer does not point to a valid instance of the array type,
   * if the index is out of bounds for the array, or if the value pointer does not point to a valid instance of the
   * element type T.
   *
   * @param array A pointer to the array instance in which to replace the element.
   * @param index The index of the element to replace, which must be within the bounds of the array's length.
   * @param value A pointer to the value to be used for replacement, which must be of the same type as the elements
   * contained in the array. The caller is responsible for ensuring that the value is properly constructed and valid for
   * insertion into the array.
   */
  void replace(const void* array, std::size_t index, const void* value) override {
    if (array != nullptr && value != nullptr) {
      static_cast<Type*>(const_cast<void*>(array))->at(index) = *static_cast<const T*>(value);
    }
  }

  /**
   * @brief Resizes the array pointed to by the parameter to contain the specified number of elements. If the new size
   * is greater than the current size, new elements are default-constructed. If the new size is less than the current
   * size, elements are removed from the end of the array. The behavior is undefined if the pointer does not point to a
   * valid instance of the array type or if the specified size is greater than the maximum size of the array type.
   *
   * @param array A pointer to the array instance to resize.
   * @param size The new size for the array, which must be greater than or equal to 0 and less than or equal to the
   * maximum size of the array type.
   */
  void resize(void* array, std::size_t size) override {
    if (array != nullptr) {
      static_cast<Type*>(array)->resize(size);
    }
  }

  /**
   * @brief Reserves enough memory to hold at least the specified number of elements in the array pointed to by the
   * parameter, without changing the array's length. If the current capacity of the array is already sufficient to hold
   * the specified number of elements, this method may do nothing. The behavior is undefined if the pointer does not
   * point to a valid instance of the array type or if the specified size is greater than the maximum size of the array
   * type.
   *
   * @param array A pointer to the array instance for which to reserve capacity.
   * @param size The number of elements for which to reserve capacity, which must be greater than or equal to the
   * current length of the array and less than or equal to the maximum size of the array type.
   */
  void reserve(void* array, std::size_t size) override {
    if (array != nullptr) {
      static_cast<Type*>(array)->reserve(size);
    }
  }

  /**
   * @brief Reduces the capacity of the array pointed to by the parameter to fit its current size, if possible. This may
   * involve freeing unused memory. The behavior is undefined if the pointer does not point to a valid instance of the
   * array type.
   *
   * @param array A pointer to the array instance for which to shrink the capacity to fit the current size.
   */
  void shrinkToFit(void* array) override {
    if (array != nullptr) {
      static_cast<Type*>(array)->shrinkToFit();
    }
  }

  /**
   * @brief Clears all elements from the array pointed to by the parameter, reducing its size to zero. The behavior is
   * undefined if the pointer does not point to a valid instance of the array type.
   *
   * @param array A pointer to the array instance to clear.
   */
  void clear(void* array) override {
    if (array != nullptr) {
      static_cast<Type*>(array)->clear();
    }
  }
};
}  // namespace core::rtti
