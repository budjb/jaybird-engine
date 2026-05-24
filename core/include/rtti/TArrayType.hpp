#pragma once

#include <concepts>
#include <vector>

#include "IArrayType.hpp"
#include "Iterator.hpp"
#include "TType.hpp"

namespace core::rtti {
/**
 * @brief Concept to check if a given type is a valid inner type descriptor for a TArrayType.
 *
 * This concept ensures that the type is derived from IType and that it defines a nested Type alias that matches the
 * specified ElementType. This is used to enforce that the inner type descriptor provided to TArrayType is compatible
 * with the element type of the array, allowing for type-safe construction of TArrayType instances.
 */
template <typename InnerType, typename ElementType>
concept TypedInnerDescriptorFor = std::derived_from<InnerType, IType> && requires { typename InnerType::Type; } &&
                                  std::same_as<typename InnerType::Type, ElementType>;

/**
 * @brief A template class representing an array type descriptor in the RTTI system. This class provides methods for
 * accessing and manipulating array elements, as well as querying array properties such as length and capacity. The
 * TArrayType class is designed to work with std::vector as the underlying container for the array elements, and it
 * requires that the inner type descriptor provided to its constructor is compatible with the element type of the array.
 *
 * @tparam T The type of the elements in the array. This type must be compatible with the inner type descriptor provided
 * to the constructor, as enforced by the TypedInnerDescriptorFor concept.
 */
template <typename T>
class TArrayType : public IArrayType {
 public:
  /**
   * @brief Defines a type alias for the underlying array type, which is std::vector<T>. This allows users of TArrayType
   * to refer to the underlying array type using TArrayType<T>::Type.
   */
  using Type = std::vector<T>;

  /**
   * @brief Constructs a TArrayType with the given name and inner type descriptor. The size and alignment are
   * automatically determined based on the underlying array type, and the kind is set to TypeKind::ARRAY. The inner type
   * descriptor must be compatible with the element type T, as enforced by the TypedInnerDescriptorFor concept.
   *
   * @tparam InnerType The type of the inner type descriptor, which must satisfy the TypedInnerDescriptorFor concept
   * with respect to T.
   * @param name The interned string name of the array type.
   * @param inner A pointer to the IType descriptor for the elements contained in the array. This must be compatible
   * with the element type T.
   */
  template <typename InnerType>
    requires TypedInnerDescriptorFor<InnerType, T>
  explicit TArrayType(const IName& name, const InnerType* inner)
      : IArrayType(name, sizeof(Type), alignof(Type), static_cast<const IType*>(inner)) {}

  /**
   * @brief Assigns the value from the source pointer to the destination pointer. Both pointers are expected to point to
   * valid instances of the underlying array type (std::vector<T>). The behavior is undefined if either pointer does not
   * point to valid instances of the array type.
   *
   * @param destination A pointer to the destination instance of the array type where the value will be assigned.
   * @param source A pointer to the source instance of the array type from which the value will be copied.
   */
  void assign(void* destination, const void* source) override {
    if (destination != nullptr) {
      *static_cast<Type*>(destination) = *static_cast<const Type*>(source);
    }
  }

  /**
   * @brief Allocates memory for a vector containing type @code T@endcode. The actual allocation logic is defined in
   * derived classes, as it may involve specific memory management strategies depending on the type.
   *
   * The type is not constructed as part of this allocation process; it only allocates raw memory that can hold an
   * instance of the type.
   *
   * The caller is responsible for managing the allocated memory and ensuring that it is properly freed when no longer
   * needed.
   *
   * @return A pointer to the allocated memory for an instance of the type.
   */
  void* allocate() override {
    return operator new(sizeof(Type), static_cast<std::align_val_t>(alignof(Type)));
  }

  /**
   * @brief Frees the memory allocated for an instance of a vector containing type @code T@endcode. The actual
   * deallocation logic is defined in derived classes, as it may involve specific memory management strategies depending
   * on the type.
   *
   * The type is not destructed as part of this allocation process; it only allocates raw memory that can hold an
   * instance of the type.
   *
   * The provided memory pointer should have been allocated by the allocate() function of this type, and it is the
   * caller's responsibility to ensure that the memory is properly freed when no longer needed.
   *
   * @param ptr A pointer to the memory that should be freed.
   */
  void deallocate(void* ptr) override {
    if (ptr != nullptr) {
      operator delete(ptr, sizeof(Type), static_cast<std::align_val_t>(alignof(Type)));
    }
  }

  /**
   * @brief Constructs an instance of a vector containing type @c T in the provided memory location. The memory should
   * be pre-allocated and large enough to hold an instance of the type.
   *
   * The behavior is undefined if the memory pointer does not point to a valid memory location that can hold an instance
   * of the type.
   *
   * @param memory A pointer to the memory where the instance should be constructed. This memory should be pre-allocated
   * and large enough to hold an instance of the type.
   */
  void construct(void* memory) noexcept override {
    if (memory != nullptr) {
      std::construct_at<Type>(static_cast<Type*>(memory));
    }
  }

  /**
   * @brief Destructs an instance of a vector containing type @c T located at the given memory location.
   *
   * The behavior is undefined if the memory pointer does not point to a valid instance of the type or was already
   * destructed.
   *
   * @param memory A pointer to the memory where the instance should be destructed.
   */
  void destruct(void* memory) noexcept override {
    if (memory != nullptr) {
      std::destroy_at<Type>(static_cast<Type*>(memory));
    }
  }

  /**
   * @brief Creates a new instance of a vector containing type @c T by allocating the required memory and constructing
   * the object using its default constructor.
   *
   * It is the caller's responsibility to manage the memory and ensure that it is properly freed when no longer needed.
   *
   * @return A pointer to the allocated memory containing a default-constructed instance of the type.
   */
  void* create() override {
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
   * @param memory A pointer to the memory that should be freed.
   */
  void destroy(void* memory) override {
    if (memory != nullptr) {
      destruct(memory);
      deallocate(memory);
    }
  }

  /**
   * @brief Compares two instances of the array type for equality. The behavior is undefined if either pointer does not
   * point to valid instances of the array type. The comparison is performed using the equality operator defined for the
   * underlying array type (std::vector<T>), which compares the contents of the arrays for equality.
   *
   * @param lhs A pointer to the first instance of the array type to compare.
   * @param rhs A pointer to the second instance of the array type to compare.
   * @return true if the instances are considered equal according to the equality semantics of the underlying array
   * type, false otherwise.
   */
  bool equals(const void* lhs, const void* rhs) const noexcept override {
    if (lhs == nullptr || rhs == nullptr) {
      return lhs == rhs;
    }
    return *static_cast<const Type*>(lhs) == *static_cast<const Type*>(rhs);
  }

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
      return static_cast<const Type*>(array)->max_size();
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
      return &static_cast<Type*>(const_cast<void*>(array))->at(index);
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
      return &static_cast<const Type*>(array)->at(index);
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
      return &static_cast<Type*>(const_cast<void*>(array))->front();
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
      return &static_cast<const Type*>(array)->front();
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
      return &static_cast<Type*>(const_cast<void*>(array))->back();
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
      return &static_cast<const Type*>(array)->back();
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
      return Iterator<>(this, nullptr);
    }
    auto* vec = static_cast<Type*>(ptr);
    return Iterator<>(this, static_cast<void*>(vec->data()));
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
      return Iterator<>(this, nullptr);
    }
    auto* vec = static_cast<Type*>(ptr);
    return Iterator<>(this, static_cast<void*>(vec->data() + vec->size()));
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
      static_cast<Type*>(const_cast<void*>(array))->push_back(*static_cast<const T*>(value));
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
      static_cast<Type*>(const_cast<void*>(array))->pop_back();
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
      vector->erase(vector->begin() + static_cast<std::ptrdiff_t>(index));
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
      static_cast<Type*>(array)->shrink_to_fit();
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
