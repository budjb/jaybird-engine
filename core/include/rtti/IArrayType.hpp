#pragma once

#include "IContainerType.hpp"

namespace core::rtti {
template <typename>
class Iterator;

template <typename>
class ReverseIterator;

/**
 * @brief Interface for array type descriptors that provides methods for accessing and manipulating array elements, as
 * well as querying array properties such as length and capacity.
 */
class IArrayType : public IContainerType {
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
   * @brief Template method that returns a pointer to the element at the specified index in the provided array, cast to
   * the specified type.
   *
   * It is the responsibility of the caller to ensure that the templated type matches the templated type of the vector
   * pointed to by the given type-erased pointer. Mismatches may lead to undefined behavior, including but not limited
   * to memory corruption, crashes, or incorrect data access.
   *
   * @tparam T The type to which the element pointer should be cast.
   * @param array A pointer to the array instance from which to retrieve the element.
   * @param index The index of the element to retrieve, which must be within the bounds of the array's length.
   * @return A pointer to the element at the specified index in the array, cast to the specified type, or nullptr if the
   * index is out of bounds.
   */
  template <typename T>
  [[nodiscard]] T* at(const void* array, const std::size_t index) {
    return static_cast<T*>(at(array, index));
  }

  /**
   * @brief Template method that returns a const pointer to the element at the specified index in the provided array,
   * cast to the specified type.
   *
   * It is the responsibility of the caller to ensure that the templated type matches the templated type of the vector
   * pointed to by the given type-erased pointer. Mismatches may lead to undefined behavior, including but not limited
   * to memory corruption, crashes, or incorrect data access.
   *
   * @tparam T The type to which the element pointer should be cast.
   * @param array A pointer to the array instance from which to retrieve the element.
   * @param index The index of the element to retrieve, which must be within the bounds of the array's length.
   * @return A const pointer to the element at the specified index in the array, cast to the specified type, or nullptr
   * if the index is out of bounds.
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
  [[nodiscard]] virtual Iterator<void> begin(void* array) noexcept = 0;

  /**
   * @brief Template method that returns an iterator pointing to the first element in the provided array, cast to the
   * specified type, or an iterator equal to end() if the array is empty.
   *
   * It is the responsibility of the caller to ensure that the templated type matches the templated type of the vector
   * pointed to by the given type-erased pointer. Mismatches may lead to undefined behavior, including but not limited
   * to memory corruption, crashes, or incorrect data access.
   *
   * @tparam T The type to which the iterator should be cast.
   * @param array A pointer to the array instance from which to retrieve the iterator.
   * @return An iterator pointing to the first element in the array, cast to the specified type, or an iterator equal to
   * end() if the array is empty.
   */
  template <typename T>
  [[nodiscard]] Iterator<T> begin(void* array) noexcept {
    return Iterator<T>(begin(static_cast<T*>(array)));
  }

  /**
   * @brief Returns a type-erased iterator pointing to one past the last element in the provided array, which serves as
   * a sentinel value for the end of the array.
   *
   * The caller must ensure that the returned end iterator is only used for comparison purposes and is not dereferenced
   * or used to access elements in the array, as this can lead to undefined behavior.
   *
   * @param array A pointer to the array instance from which to retrieve the end iterator.
   * @return An iterator pointing to one past the last element in the array, which serves as a sentinel value for the
   * end of the array.
   */
  [[nodiscard]] virtual Iterator<void> end(void* array) noexcept = 0;

  /**
   * @brief Template method that returns an iterator pointing to one past the last element in the provided array, cast
   * to the specified type, which serves as a sentinel value for the end of the array.
   *
   * It is the responsibility of the caller to ensure that the templated type matches the templated type of the vector
   * pointed to by the given type-erased pointer. Mismatches may lead to undefined behavior, including but not limited
   * to memory corruption, crashes, or incorrect data access.
   *
   * The caller must ensure that the returned end iterator is only used for comparison purposes and is not dereferenced
   * or used to access elements in the array, as this can lead to undefined behavior.
   *
   * @tparam T
   * @param array
   * @return
   */
  template <typename T>
  [[nodiscard]] Iterator<T> end(void* array) noexcept {
    return Iterator<T>(end(static_cast<T*>(array)));
  }

  /**
   * @brief Returns a type-erased reverse iterator pointing to the last element in the provided array, or a reverse
   * iterator equal to rend() if the array is empty.
   *
   * @param array A pointer to the array instance from which to retrieve the reverse iterator.
   * @return A reverse iterator pointing to the last element in the array, or a reverse iterator equal to rend() if the
   * array is empty.
   */
  [[nodiscard]] ReverseIterator<void> rbegin(void* array) noexcept;

  /**
   * @brief Template method that returns a reverse iterator pointing to the last element in the provided array, cast to
   * the specified type, or a reverse iterator equal to rend() if the array is empty.
   *
   * It is the responsibility of the caller to ensure that the templated type matches the templated type of the vector
   * pointed to by the given type-erased pointer. Mismatches may lead to undefined behavior, including but not limited
   * to memory corruption, crashes, or incorrect data access.
   *
   * @tparam T The type to which the reverse iterator should be cast.
   * @param array A pointer to the array instance from which to retrieve the reverse iterator.
   * @return A reverse iterator pointing to the last element in the array, cast to the specified type, or a reverse
   * iterator equal to rend() if the array is empty.
   */
  template <typename T>
  [[nodiscard]] ReverseIterator<T> rbegin(void* array) noexcept {
    return ReverseIterator<T>(rbegin(static_cast<T*>(array)));
  }

  /**
   * @brief Returns a type-erased reverse iterator pointing to one before the first element in the provided array, which
   * serves as a sentinel value for the reverse end of the array.
   *
   * The caller must ensure that the returned rend iterator is only used for comparison purposes and is not dereferenced
   * or used to access elements in the array, as this can lead to undefined behavior.
   *
   * @param array A pointer to the array instance from which to retrieve the reverse end iterator.
   * @return A reverse iterator pointing to one before the first element in the array, which serves as a sentinel value
   * for the reverse end of the array.
   */
  [[nodiscard]] ReverseIterator<void> rend(void* array) noexcept;

  /**
   * @brief Template method that returns a reverse iterator pointing to one before the first element in the provided
   * array, cast to the specified type, which serves as a sentinel value for the reverse end of the array.
   *
   * It is the responsibility of the caller to ensure that the templated type matches the templated type of the vector
   * pointed to by the given type-erased pointer. Mismatches may lead to undefined behavior, including but not limited
   * to memory corruption, crashes, or incorrect data access.
   *
   * The caller must ensure that the returned rend iterator is only used for comparison purposes and is not dereferenced
   * or used to access elements in the array, as this can lead to undefined behavior.
   *
   * @tparam T The type to which the reverse end iterator should be cast.
   * @param array A pointer to the array instance from which to retrieve the reverse end iterator.
   * @return A reverse iterator pointing to one before the first element in the array, cast to the specified type, which
   * serves as a sentinel value for the reverse end of the array.
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
   * @brief Inserts a new element with the specified value at the specified index in the provided array, shifting
   * subsequent elements to make room for the new element.
   *
   * @param array A pointer to the array instance into which to insert the new element.
   * @param index The index at which to insert the new element, which must be within the bounds of the array's length or
   * equal to the array's length to insert at the end.
   * @param value A pointer to the value to be inserted, which must be of the same type as the elements contained in the
   * array. The caller is responsible for ensuring that the value is properly constructed and valid for insertion into
   * the array.
   */
  virtual void insert(const void* array, std::size_t index, const void* value) = 0;

  /**
   * @brief Appends a new element with the specified value to the end of the provided array, increasing the array's
   * length by one.
   *
   * @param array A pointer to the array instance to which to append the new element.
   * @param value A pointer to the value to be appended, which must be of the same type as the elements contained in the
   * array. The caller is responsible for ensuring that the value is properly constructed and valid for insertion into
   * the array.
   *
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

}  // namespace core::rtti
