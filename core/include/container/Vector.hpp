#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "memory/Allocator.hpp"

namespace core::container {
/**
 * @brief A simple implementation of a dynamic array (vector) that supports custom memory allocators. This class
 * provides basic functionality for managing a dynamic array of elements, including resizing, reserving capacity, and
 * accessing elements. The vector is designed to be compatible with the C++ Standard Library's allocator requirements,
 * allowing it to be used with custom allocators that satisfy the Allocator concept. The vector uses a simple growth
 * strategy to manage its capacity, and provides basic error handling for out-of-range access. This implementation is
 * intended for educational purposes and may not be optimized for performance or memory usage in production
 * environments.
 *
 * @tparam T The type of elements stored in the vector. This type will be used to determine the size of memory blocks to
 * allocate and deallocate, and to ensure that the vector is compatible with standard containers that require specific
 * types of allocators.
 * @tparam Alloc The type of allocator used for memory management. If no allocator is provided,the vector will default
 * to using the HeapAllocator<T> for all memory operations.
 */
template <typename T, memory::Allocator Alloc = memory::HeapAllocator<T>>
class Vector {
 public:
  /**
   * @brief A type alias for the allocator traits of the specified allocator type. This alias provides access to the
   * properties and member types of the allocator, such as the pointer type, reference type, and value type.
   *
   * The allocator traits are used to ensure that the vector is compatible with the requirements of the C++ Standard
   * Library's allocator model, and to allow for the use of custom allocators that satisfy the Allocator concept.
   *
   */
  using AllocatorTraits = std::allocator_traits<Alloc>;

  /**
   * @brief Constructs a new, empty Vector object with an optional custom allocator.
   *
   * If no allocator instance is provided, the vector will create an instance of the templated allocator.
   *
   * @param allocator An optional instance of the allocator to use for memory management.
   */
  explicit Vector(const Alloc& allocator = Alloc()) noexcept(std::is_nothrow_copy_constructible_v<Alloc>)
      : m_data(nullptr), m_allocator(allocator) {}

  /**
   * @brief Constructs a new Vector object with a specified initial capacity and an optional custom allocator.
   *
   * If no allocator instance is provided, the vector will create an instance of the templated allocator.
   *
   * @param capacity The initial capacity of the vector. This is the number of elements that the vector can hold
   * before it needs to grow its internal storage.
   * @param allocator An optional instance of the allocator to use for memory management.
   */
  explicit Vector(const std::size_t capacity,
                  const Alloc& allocator = Alloc()) noexcept(std::is_nothrow_copy_constructible_v<Alloc>)
      : Vector(allocator) {
    reserve(capacity);
  }

  /**
   * @brief Copy constructor for the Vector class. This constructor creates a new Vector object that is a copy of an
   * existing Vector object. The new Vector will have the same size, capacity, and elements as the original Vector, but
   * will use its own memory for storage.
   *
   * The copy constructor performs a deep copy of the elements from the original Vector to the new Vector, ensuring that
   * changes to one Vector do not affect the other. The copy constructor also copies the allocator from the original
   * Vector, allowing the new Vector to use the same allocator for memory management if desired.
   *
   * @param other The Vector object to copy.
   */
  Vector(const Vector& other) : Vector(AllocatorTraits::select_on_container_copy_construction(other.m_allocator)) {
    if (other.m_size) {
      reserve(other.m_size);

      for (std::size_t i = 0; i < other.m_size; ++i) {
        AllocatorTraits::construct(m_allocator, m_data + i, other.m_data[i]);
      }

      m_size = other.m_size;
    }
  }

  /**
   * @brief Move constructor for the Vector class. This constructor creates a new Vector object by taking ownership of
   * the resources of an existing Vector object. The new Vector will have the same size, capacity, and elements as the
   * original Vector, but will take ownership of the original Vector's memory for storage.
   *
   * @param other The Vector object to move.
   */
  Vector(Vector&& other) noexcept(std::is_nothrow_move_constructible_v<Alloc>)
      : m_data(std::exchange(other.m_data, nullptr)),
        m_size(std::exchange(other.m_size, 0)),
        m_capacity(std::exchange(other.m_capacity, 0)),
        m_allocator(std::move(other.m_allocator)) {}

  /**
   * @brief Copy assignment operator for the Vector class. This operator assigns the contents of one Vector object to
   * another by copying the elements from the source Vector to the target Vector. The target Vector will have the same
   * size, capacity, and elements as the source Vector after the copy assignment, but will use its own memory for
   * storage.
   *
   * @param other The Vector object to copy-assign from.
   * @return A reference to the target Vector object after the copy assignment.
   */
  Vector& operator=(const Vector& other) {
    if (this == &other) {
      return *this;
    }

    if constexpr (AllocatorTraits::propagate_on_container_copy_assignment::value) {
      reset();
      m_allocator = other.m_allocator;
    } else {
      if constexpr (AllocatorTraits::is_always_equal::value) {
        reset();
      } else {
      }
    }

    assign_from(other);

    return *this;
  }

  /**
   * @brief Move assignment operator for the Vector class. This operator assigns the contents of one Vector object to
   * another by taking ownership of the resources of the source Vector. The target Vector will have the same size,
   * capacity, and elements as the source Vector after the move assignment, but will take ownership of the source
   * Vector's memory for storage.
   *
   * @param other The Vector object to move-assign from.
   * @return A reference to the target Vector object after the move assignment.
   */
  Vector& operator=(Vector&& other) noexcept(AllocatorTraits::propagate_on_container_move_assignment::value ||
                                             AllocatorTraits::is_always_equal::value) {
    if (this == &other) {
      return *this;
    }

    if constexpr (AllocatorTraits::propagate_on_container_move_assignment::value) {
      reset();
      m_allocator = std::move(other.m_allocator);
      m_data = std::exchange(other.m_data, nullptr);
      m_size = std::exchange(other.m_size, 0);
      m_capacity = std::exchange(other.m_capacity, 0);
    } else if constexpr (AllocatorTraits::is_always_equal::value || m_allocator == other.m_allocator) {
      reset();
      m_data = std::exchange(other.m_data, nullptr);
      m_size = std::exchange(other.m_size, 0);
      m_capacity = std::exchange(other.m_capacity, 0);
    } else {
      moveFrom(other);
    }

    return *this;
  }

  /**
   * @brief Destructor for the Vector class that ensures that all resources are properly released when the Vector object
   * goes out of scope or is explicitly deleted. This prevents memory leaks and ensures that the destructor of each
   * element is called to perform any necessary cleanup for the elements stored in the vector.
   */
  ~Vector() noexcept {
    reset();
  }

  /**
   * @brief Provides access to the element at the specified index in the vector. This method performs bounds checking
   * and will throw a std::out_of_range exception if the index is out of bounds (i.e., if it is greater than or equal to
   * the size of the vector).
   *
   * If the index is valid, the method returns a reference to the element at the specified index, allowing for both
   * reading and writing of the element.
   *
   * @param index The index of the element to access. This index must be less than the size of the vector.
   * @return A reference to the element at the specified index in the vector.
   * @throws std::out_of_range If the index is out of bounds (i.e., if it is greater than or equal to the size of the
   * vector).
   */
  T& at(const std::size_t index) {
    if (index >= m_size) {
      throw std::out_of_range("Index out of range");
    }
    return m_data[index];
  }

  /**
   * @brief Provides read-only access to the element at the specified index in the vector. This method performs bounds
   * checking and will throw a std::out_of_range exception if the index is out of bounds (i.e., if it is greater than or
   * equal to the size of the vector).
   *
   * If the index is valid, the method returns a const reference to the element at the specified index, allowing for
   * read-only access to the element.
   *
   * @param index The index of the element to access. This index must be less than the size of the vector.
   * @return A const reference to the element at the specified index in the vector.
   * @throws std::out_of_range If the index is out of bounds (i.e., if it is greater than or equal to the size of the
   * vector).
   */
  const T& at(const std::size_t index) const {
    if (index >= m_size) {
      throw std::out_of_range("Index out of range");
    }
    return m_data[index];
  }

  /**
   * @brief Resizes the vector to contain the specified number of elements. If the new size is greater than the current
   * size, the vector will be expanded and new elements will be default-constructed. If the new size is less than the
   * current size, the vector will be reduced and the excess elements will be destroyed.
   *
   * The resize operation may involve reallocating the internal storage of the vector if the new size exceeds the
   * current capacity. In this case, the existing elements will be moved to the new storage, and the old storage will be
   * deallocated. If the new size is within the current capacity, the existing storage will be reused, and only the
   * necessary elements will be constructed or destroyed to achieve the new size.
   *
   * @param newSize The new size of the vector. This is the number of elements that the vector should contain after the
   * resize operation.
   */
  void resize(const std::size_t newSize) {
    if (newSize < m_size) {
      destroy(newSize, m_size);
      m_size = newSize;
      return;
    }

    if (newSize > m_capacity) {
      reallocate(newSize);
    }

    std::size_t i = m_size;
    try {
      for (; i < newSize; ++i) {
        AllocatorTraits::construct(m_allocator, m_data + i);
      }
      m_size = newSize;
    } catch (...) {
      destroy(m_size, i);
      throw;
    }
  }

  /**
   * @brief Provides access to the element at the specified index in the vector. This method performs bounds checking
   * and will throw a std::out_of_range exception if the index is out of bounds (i.e., if it is greater than or equal to
   * the size of the vector).
   *
   * If the index is valid, the method returns a reference to the element at the specified index, allowing for both
   * reading and writing of the element.
   *
   * @param index The index of the element to access. This index must be less than the size of the vector.
   * @return A reference to the element at the specified index in the vector.
   * @throws std::out_of_range If the index is out of bounds (i.e., if it is greater than or equal to the size of the
   * vector).
   */
  T& operator[](const std::size_t index) {
    return at(index);
  }

  /**
   * @brief Provides read-only access to the element at the specified index in the vector. This method performs bounds
   * checking and will throw a std::out_of_range exception if the index is out of bounds (i.e., if it is greater than or
   * equal to the size of the vector).
   *
   * If the index is valid, the method returns a const reference to the element at the specified index, allowing for
   * read-only access to the element.
   *
   * @param index The index of the element to access. This index must be less than the size of the vector.
   * @return A const reference to the element at the specified index in the vector.
   * @throws std::out_of_range If the index is out of bounds (i.e., if it is greater than or equal to the size of the
   * vector).
   */
  const T& operator[](const std::size_t index) const {
    return at(index);
  }

  /**
   * @brief Returns the number of elements currently stored in the vector. This is the number of valid elements that
   * have been constructed and are accessible in the vector.
   *
   * The size of the vector may be less than or equal to the capacity of the vector, which is the total amount of memory
   * allocated for storing elements.
   *
   * @return The number of elements currently stored in the vector.
   */
  [[nodiscard]] std::size_t size() const noexcept {
    return m_size;
  }

  /**
   * @brief Returns the total capacity of the vector, which is the amount of memory allocated for storing elements.
   *
   * The capacity of the vector may be greater than or equal to the size of the vector, which is the number of valid
   * elements currently stored in the vector.
   *
   * @return The total capacity of the vector, which is the amount of memory allocated for storing elements.
   */
  [[nodiscard]] std::size_t capacity() const noexcept {
    return m_capacity;
  }

  /**
   * @brief Reserves storage for at least the specified number of elements. If the new capacity is greater than the
   * current capacity, the vector will reallocate its internal storage to accommodate the new capacity. If the new
   * capacity is less than or equal to the current capacity, the vector will not reallocate and will simply return
   * without making any changes.
   *
   * The reserve operation may involve reallocating the internal storage of the vector if the new capacity exceeds the
   * current capacity. In this case, the existing elements will be moved to the new storage, and the old storage will be
   * deallocated. If the new capacity is within the current capacity, the existing storage will be reused, and no
   * elements will be moved or copied.
   *
   * @param capacity The new capacity of the vector. This is the minimum number of elements that the vector should be
   * able to hold after the reserve operation.
   * @throws std::length_error If the new capacity exceeds the maximum size that the vector can support, which is
   * typically determined by the maximum value of std::size_t or the limitations of the allocator.
   */
  void reserve(const std::size_t capacity) {
    if (capacity > m_capacity) {
      reallocate(capacity);
    }
  }

  /**
   * @brief Reduces the capacity of the vector to fit its current size. If the current size of the vector is less than
   * its capacity, the vector will reallocate its internal storage to match the current size, effectively freeing any
   * excess memory that was previously allocated. If the current size is equal to the capacity, the vector will not
   * reallocate and will simply return without making any changes.
   *
   * The shrink operation may involve reallocating the internal storage of the vector if the current size is less than
   * the current capacity. In this case, the existing elements will be moved to the new storage, and the old storage
   * will be deallocated. If the current size is equal to the current capacity, the existing storage will be reused, and
   * no elements will be moved or copied.
   */
  void shrink() {
    if (m_size < m_capacity) {
      reallocate(m_size);
    }
  }

  /**
   * @brief Clears the contents of the vector, destroying all elements and resetting the size to zero.
   *
   * This method ensures that the destructor of each element is called to perform any necessary cleanup for the elements
   * stored in the vector, and that the size of the vector is properly updated to reflect the cleared state. After
   * calling clear(), the vector will be empty and ready to accept new elements without needing to reallocate memory, as
   * long as the new size does not exceed the current capacity.
   *
   * The clear operation does not deallocate the internal storage of the vector, allowing for efficient reuse of the
   * existing memory if new elements are added after clearing. If the vector needs to grow beyond its current capacity
   * after being cleared, it will reallocate its internal storage as needed to accommodate the new size.
   */
  void clear() noexcept {
    destroy(0, m_size);
    m_size = 0;
  }

  /**
   * @brief Returns a reference to the allocator used by the vector for memory management. This allows users of the
   * vector to access the allocator and use it for custom memory management operations if needed. The allocator can be
   * used to allocate and deallocate memory for elements in the vector, and to construct and destroy elements in the
   * vector using the allocator's construct and destroy methods.
   *
   * @return A reference to the allocator used by the vector for memory management.
   */
  [[nodiscard]] Alloc& allocator() const noexcept {
    return m_allocator;
  }

 private:
  /**
   * @brief Helper method to copy the contents of another Vector object into this Vector. This method is used by the
   * copy constructor and the copy assignment operator to perform the actual copying of elements from the source Vector
   * to the target Vector.
   *
   * @param other The Vector object to copy from.
   */
  void copyFrom(const Vector& other) {
    clear();

    if (other.m_size > m_capacity) {
      reallocate(other.m_size);
    }

    for (std::size_t i = 0; i < other.m_size; ++i) {
      AllocatorTraits::construct(m_allocator, m_data + i, other.m_data[i]);
    }
    m_size = other.m_size;
  }

  /**
   * @brief Helper method to move the contents of another Vector object into this Vector. This method is used by the
   * move constructor and the move assignment operator to perform the actual moving of elements from the source Vector
   * to the target Vector. After the move, the source Vector is left in a valid but unspecified state, and the resources
   * are now owned by the target Vector.
   *
   * @param other The Vector object to move from.
   * @throws std::length_error If the size of the source Vector exceeds the maximum size that the target Vector can
   * support, which is typically determined by the maximum value of std::size_t or the limitations of the allocator.
   */
  void moveFrom(Vector& other) {
    clear();

    if (other.m_size > m_capacity) {
      reallocate(other.m_size);
    }

    for (std::size_t i = 0; i < other.m_size; ++i) {
      AllocatorTraits::construct(m_allocator, m_data + i, std::move(other.m_data[i]));
      AllocatorTraits::destroy(other.m_allocator, other.m_data + i);
    }

    m_size = other.m_size;

    AllocatorTraits::deallocate(other.m_allocator, other.m_data, other.m_capacity);

    other.m_data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;
  }

  /**
   * @brief Helper method to reallocate the internal storage of the vector to a new capacity.
   *
   * Reallocation will only occur if the new capacity is greater than the current size of the vector. If the new
   * capacity is less than or equal to the current size, the method will return without making any changes, as the
   * existing storage can accommodate the current elements without needing to reallocate.
   *
   * The reallocate method allocates new memory for the specified capacity, moves the existing elements to the new
   * storage, and deallocates the old storage. If the new capacity is zero, the method will deallocate all existing
   * storage and set the data pointer to nullptr, effectively clearing the vector.
   *
   * If the new capacity is greater than zero, the method will allocate new storage and move the existing elements to
   * the new storage, ensuring that the size and capacity of the vector are properly updated to reflect the changes.
   *
   * @param capacity
   */
  void reallocate(const std::size_t capacity) noexcept {
    if (capacity <= m_size) {
      return;
    }

    auto* newData = AllocatorTraits::allocate(m_allocator, capacity);

    for (std::size_t i = 0; i < m_size; ++i) {
      AllocatorTraits::construct(m_allocator, newData + i, std::move_if_noexcept(m_data[i]));
    }

    AllocatorTraits::deallocate(m_allocator, m_data, m_capacity);

    m_data = newData;
    m_capacity = capacity;
  }

  /**
   * @brief Helper method to reset the vector to an empty state, destroying all elements and deallocating all memory.
   * After calling reset(), the vector will be empty and all memory will be deallocated, allowing for efficient cleanup
   * of resources and preventing memory leaks.
   *
   */
  void reset() noexcept {
    if (!m_data) {
      return;
    }

    clear();

    AllocatorTraits::deallocate(m_allocator, m_data, m_capacity);

    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
  }

  /**
   * @brief Helper method to destroy all elements in the vector. This method calls the allocator's destroy method for
   * each element in the vector, ensuring that the destructor of each element is called to perform any necessary cleanup
   * for the elements stored in the vector.
   *
   * After calling this method, all elements in the vector will be destroyed, but the vector's size is not modified and
   * the memory for the elements will still be allocated and can be reused for new elements if needed.
   */
  void destroy() noexcept {
    destroy(0, m_size);
  }

  /**
   * @brief Helper method to destroy a range of elements in the vector. This method calls the allocator's destroy method
   * for each element in the specified range, ensuring that the destructor of each element is called to perform any
   * necessary cleanup for the elements stored in the vector.
   *
   * The method will destroy the elements in the range [begin, end), meaning that it will destroy the element at index
   * begin and all subsequent elements up to, but not including, the element at index end.
   *
   * @param begin The index of the first element to destroy. This index is inclusive, meaning that the element at this
   * index will be destroyed.
   * @param end The index of the first element to not destroy. This index is exclusive, meaning that the element at this
   * index will not be destroyed, and all elements up to, but not including, this index will be destroyed.
   */
  void destroy(const std::size_t begin, const std::size_t end) noexcept {
    for (std::size_t i = begin; i < end; ++i) {
      AllocatorTraits::destroy(m_allocator, m_data + i);
    }
  }

  /**
   * @brief Pointer to the dynamically allocated array of elements. This pointer is managed by the vector and is used to
   * store the elements of the vector.
   *
   * The memory for this array is allocated and deallocated using the allocator provided to the vector, and the elements
   * are constructed and destroyed using the allocator's construct and destroy methods.
   */
  T* m_data = nullptr;

  /**
   * @brief The number of valid elements currently stored in the vector. This is the number of elements that have been
   * constructed and are accessible in the vector. The size of the vector may be less than or equal to the capacity of
   * the vector, which is the total amount of memory allocated for storing elements.
   */
  std::size_t m_size{};

  /**
   * @brief The total capacity of the vector, which is the amount of memory allocated for storing elements. The capacity
   * of the vector may be greater than or equal to the size of the vector, which is the number of valid elements
   * currently stored in the vector. The capacity is managed by the vector and is updated as needed when the vector
   * grows or shrinks its internal storage to accommodate changes in size.
   *
   * The capacity is allocated and deallocated using the allocator provided to the vector, and is used to determine when
   * the vector needs to reallocate its internal storage to accommodate changes in size.
   */
  std::size_t m_capacity{};

  /**
   * @brief The allocator used by the vector for memory management. This allocator is responsible for allocating and
   * deallocating memory for the elements stored in the vector, and for constructing and destroying the elements using
   * the allocator's construct and destroy methods.
   *
   * The allocator is provided to the vector as a template parameter, and can be customized to use different memory
   * management strategies as needed. The allocator is stored as a member variable in the vector, allowing it to be
   * accessed and used for memory management operations throughout the vector's implementation.
   */
  Alloc m_allocator;
};
}  // namespace core::container
