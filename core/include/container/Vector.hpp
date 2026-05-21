#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>

#include "IVector.hpp"
#include "Iterator.hpp"

namespace core::container {
/**
 * @brief A vector-like container that provides dynamic array functionality with support for custom allocators. This
 * class is designed to be a more flexible and customizable alternative to std::vector, allowing for different
 * allocation strategies and element types. It inherits from IVector, which provides the underlying storage and metadata
 * management, while Vector handles the type-specific construction, destruction, and element access.
 *
 * @tparam T The type of elements stored in the vector. This can be any type, including move-only types, as long as it
 * is compatible with the allocator used by the vector.
 * @tparam Alloc The allocator type used for memory management. This should be a standard allocator that meets the
 * requirements of the C++ allocator model, and it should be compatible with the element type T. By default, it uses
 * std::pmr::polymorphic_allocator<T>, which allows for flexible memory management strategies.
 */
template <typename T, typename Alloc = std::pmr::polymorphic_allocator<T>>
class Vector : public IVector {
 public:
  using allocator_type = Alloc;
  using traits = std::allocator_traits<allocator_type>;
  using value_type = traits::value_type;
  using size_type = traits::size_type;
  using difference_type = traits::difference_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = traits::pointer;
  using const_pointer = traits::const_pointer;
  using iterator = Iterator<value_type>;
  using const_iterator = Iterator<const value_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  static_assert(std::is_same_v<pointer, value_type*>,
                "Vector currently requires allocators that expose raw pointer storage");
  static_assert(std::is_same_v<const_pointer, const value_type*>,
                "Vector currently requires allocators that expose raw pointer storage");
  static_assert(std::is_same_v<size_type, std::size_t>,
                "Vector currently requires allocators that use std::size_t for size_type");
  static_assert(std::is_same_v<value_type, T>, "Vector allocator value_type must match the Vector element type");

  /**
   * @brief Default constructor. Constructs an empty vector with the default allocator. The default allocator is used to
   * manage the memory for the vector's storage, and it will be used for all memory allocation and deallocation
   * operations performed by the vector.
   *
   * If the allocator type has a nothrow default constructor, then this constructor will also be noexcept.
   */
  Vector() noexcept(std::is_nothrow_default_constructible_v<allocator_type>);

  /**
   * @brief Constructs an empty vector with the specified allocator. The provided allocator is used to manage the memory
   * for the vector's storage, and it will be used for all memory allocation and deallocation operations performed by
   * the vector. This allows for flexible memory management strategies, as the caller can provide a custom allocator
   * that implements a specific allocation strategy or tracks memory usage in a particular way.
   *
   * If the allocator type has a nothrow copy constructor, then this constructor will also be noexcept.
   *
   * @param allocator The allocator to use for managing the memory of the vector's storage.
   */
  explicit Vector(const allocator_type& allocator) noexcept(std::is_nothrow_copy_constructible_v<allocator_type>);

  /**
   * @brief Constructs a vector with the specified number of default-constructed elements, using the provided allocator.
   * The provided allocator is used to manage the memory for the vector's storage, and it will be used for all memory
   * allocation and deallocation operations performed by the vector.
   *
   * If the element type has a nothrow default constructor, then this constructor will also be noexcept.
   *
   * @param count The number of default-constructed elements to create in the vector.
   * @param allocator The allocator to use for managing the memory of the vector's storage.
   */
  explicit Vector(size_type count, const allocator_type& allocator = allocator_type());

  /**
   * @brief Constructs a vector with the specified number of copies of the given value, using the provided allocator.
   * The provided allocator is used to manage the memory for the vector's storage, and it will be used for all memory
   * allocation and deallocation operations performed by the vector.
   *
   * @param count The number of copies of the value to create in the vector.
   * @param value The value to copy into the vector.
   * @param allocator The allocator to use for managing the memory of the vector's storage.
   */
  explicit Vector(size_type count, const value_type& value, const allocator_type& allocator = allocator_type());

  /**
   * @brief Constructs a vector with elements copied from the range [first, last), using the provided allocator. The
   * provided allocator is used to manage the memory for the vector's storage, and it will be used for all memory
   * allocation and deallocation operations performed by the vector.
   *
   * @tparam InputIt The type of the input iterators.
   * @param first An input iterator pointing to the first element in the range to copy from.
   * @param last An input iterator pointing to one past the last element in the range to copy from.
   * @param allocator The allocator to use for managing the memory of the vector's storage.
   */
  template <std::input_iterator InputIt>
  explicit Vector(InputIt first, InputIt last, const allocator_type& allocator = allocator_type()) : Vector(allocator) {
    assign(first, last);
  }

  /**
   * @brief Constructs a vector with elements copied from the given initializer list, using the provided allocator. The
   * provided allocator is used to manage the memory for the vector's storage, and it will be used for all memory
   * allocation and deallocation operations performed by the vector.
   *
   * @param values An initializer list containing the values to copy into the vector.
   * @param allocator The allocator to use for managing the memory of the vector's storage.
   */
  explicit Vector(std::initializer_list<value_type> values, const allocator_type& allocator = allocator_type());

  /**
   * @brief Copy constructor. Constructs a vector by copying the elements from another vector, using the provided
   * allocator. The provided allocator is used to manage the memory for the vector's storage, and it will be used for
   * all memory allocation and deallocation operations performed by the vector.
   *
   * If the allocator's select_on_container_copy_construction trait is true, then the allocator from the other vector
   * will be used for the new vector; otherwise, the allocator from the current vector will be used.
   *
   * @param other The vector to copy from. The elements of this vector will be copied into the new vector, and the
   * allocator used for the new vector will be determined based on the allocator traits of the other vector's allocator.
   */
  explicit Vector(const Vector& other);

  /**
   * @brief Move constructor. Constructs a vector by moving the elements from another vector, using the provided
   * allocator. The provided allocator is used to manage the memory for the vector's storage, and it will be used for
   * all memory allocation and deallocation operations performed by the vector.
   *
   * @param other The vector to move from. The elements of this vector will be moved into the new vector, and the
   * allocator used for the new vector will be determined based on the allocator traits of the other vector's allocator
   * and whether the allocators are equal. If the allocators are equal or if the allocator's
   * propagate_on_container_move_assignment trait is true, then the new vector will take ownership of the other vector's
   * storage; otherwise, the elements will be moved individually into the new vector's storage.
   */
  explicit Vector(Vector&& other) noexcept(std::is_nothrow_move_constructible_v<allocator_type>);

  /**
   * @brief Move constructor with allocator. Constructs a vector by moving the elements from another vector, using the
   * provided allocator. The provided allocator is used to manage the memory for the vector's storage, and it will be
   * used for all memory allocation and deallocation operations performed by the vector.
   *
   * @param other The vector to move from. The elements of this vector will be moved into the new vector, and the
   * allocator used for the new vector will be the one provided as an argument.
   * @param allocator The allocator to use for managing the memory of the vector's storage.
   */
  explicit Vector(Vector&& other, const allocator_type& allocator);

  /**
   * @brief Copy assignment operator. Assigns the contents of another vector to this vector by copying the elements,
   * using the provided allocator. The provided allocator is used to manage the memory for the vector's storage, and it
   * will be used for all memory allocation and deallocation operations performed by the vector.
   *
   * @param other The vector to copy from. The elements of this vector will be copied into this vector, and the
   * allocator used for this vector will be determined based on the allocator traits of the other vector's allocator and
   * whether the allocators are equal.
   * @return A reference to this vector after the assignment is complete.
   */
  Vector& operator=(const Vector& other);

  /**
   * @brief Move assignment operator. Assigns the contents of another vector to this vector by moving the elements,
   * using the provided allocator. The provided allocator is used to manage the memory for the vector's storage, and it
   * will be used for all memory allocation and deallocation operations performed by the vector.
   *
   * @param other The vector to move from. The elements of this vector will be moved into this vector, and the allocator
   * used for this vector will be determined based on the allocator traits of the other vector's allocator and whether
   * the allocators are equal.
   * @return A reference to this vector after the assignment is complete.
   */
  Vector& operator=(Vector&& other) noexcept(traits::propagate_on_container_move_assignment::value ||
                                             traits::is_always_equal::value);

  /**
   * @brief Initializer list assignment operator. Assigns the contents of an initializer list to this vector by copying
   * the elements, using the provided allocator. The provided allocator is used to manage the memory for the vector's
   * storage, and it will be used for all memory allocation and deallocation operations performed by the vector.
   *
   * @param values An initializer list containing the values to copy into this vector. The elements of this initializer
   * list will be copied into this vector, replacing its current contents.
   * @return A reference to this vector after the assignment is complete.
   */
  Vector& operator=(std::initializer_list<value_type> values);

  /**
   * @brief Destructor. Destroys the vector and deallocates its storage. This will destroy all elements currently stored
   * in the vector and free the memory used for the vector's storage. The destructor will ensure that all resources are
   * properly released, and it will call the appropriate destroy and deallocate methods to clean up the vector's
   * storage. After the destructor is called, the vector will be in a valid but unspecified state, and any access to its
   * elements or storage will be undefined behavior.
   *
   * If the element type has a nothrow destructor and the allocator's deallocate method is noexcept, then this
   * destructor will also be noexcept.
   */
  ~Vector() noexcept override;

#pragma region Element Accessors

  /**
   * @brief Returns a reference to the element at the specified index in the vector, with bounds checking. If the index
   * is out of range (i.e., greater than or equal to the current size of the vector), this method will throw a
   * std::out_of_range exception. This method provides safe access to the elements of the vector, ensuring that any
   * attempt to access an element outside the valid range will be properly handled with an exception.
   *
   * @param index The index of the element to access. This should be a value between 0 and size() - 1, inclusive. If
   * this value is out of range, a std::out_of_range exception will be thrown.
   * @return A reference to the element at the specified index in the vector. If the index is valid, this will return a
   * reference to the element; otherwise, an exception will be thrown.
   * @throws std::out_of_range If the index is out of range (i.e., greater than or equal to the current size of the
   * vector).
   */
  [[nodiscard]] reference at(size_type index);

  /**
   * @brief Returns a const reference to the element at the specified index in the vector, with bounds checking. If the
   * index is out of range (i.e., greater than or equal to the current size of the vector), this method will throw a
   * std::out_of_range exception. This method provides safe access to the elements of the vector, ensuring that any
   * attempt to access an element outside the valid range will be properly handled with an exception.
   *
   * @param index The index of the element to access. This should be a value between 0 and size() - 1, inclusive. If
   * this value is out of range, a std::out_of_range exception will be thrown.
   * @return A const reference to the element at the specified index in the vector. If the index is valid, this will
   * return a const reference to the element; otherwise, an exception will be thrown.
   * @throws std::out_of_range If the index is out of range (i.e., greater than or equal to the current size of the
   * vector).
   */
  [[nodiscard]] const_reference at(size_type index) const;

  /**
   * @brief Returns a reference to the element at the specified index in the vector, without bounds checking. The
   * behavior of this method is undefined if the index is out of range (i.e., greater than or equal to the current size
   * of the vector). This method provides fast access to the elements of the vector, but it should only be used when the
   * caller is certain that the index is valid.
   *
   * @param index The index of the element to access. This should be a value between 0 and size() - 1, inclusive. If
   * this value is out of range, the behavior is undefined.
   * @return A reference to the element at the specified index in the vector. If the index is valid, this will return a
   * reference to the element; otherwise, the behavior is undefined.
   */
  [[nodiscard]] reference operator[](size_type index) noexcept;

  /**
   * @brief Returns a const reference to the element at the specified index in the vector, without bounds checking. The
   * behavior of this method is undefined if the index is out of range (i.e., greater than or equal to the current size
   * of the vector). This method provides fast access to the elements of the vector, but it should only be used when the
   * caller is certain that the index is valid.
   *
   * @param index The index of the element to access. This should be a value between 0 and size() - 1, inclusive. If
   * this value is out of range, the behavior is undefined.
   * @return A const reference to the element at the specified index in the vector. If the index is valid, this will
   * return a const reference to the element; otherwise, the behavior is undefined.
   */
  [[nodiscard]] const_reference operator[](size_type index) const noexcept;

  [[nodiscard]] reference front();

  [[nodiscard]] const_reference front() const;

  [[nodiscard]] reference back();

  [[nodiscard]] const_reference back() const;

  [[nodiscard]] pointer data() noexcept;

  [[nodiscard]] const_pointer data() const noexcept;

  void assign(size_type count, const value_type& value);

  template <std::input_iterator InputIt>
  void assign(InputIt first, InputIt last);

  void assign(std::initializer_list<value_type> values);

  [[nodiscard]] iterator begin() noexcept;

  [[nodiscard]] const_iterator begin() const noexcept;

  [[nodiscard]] const_iterator cbegin() const noexcept;

  [[nodiscard]] iterator end() noexcept;

  [[nodiscard]] const_iterator end() const noexcept;

  [[nodiscard]] const_iterator cend() const noexcept;

  [[nodiscard]] reverse_iterator rbegin() noexcept;

  [[nodiscard]] const_reverse_iterator rbegin() const noexcept;

  [[nodiscard]] const_reverse_iterator crbegin() const noexcept;

  [[nodiscard]] reverse_iterator rend() noexcept;

  [[nodiscard]] const_reverse_iterator rend() const noexcept;

  [[nodiscard]] const_reverse_iterator crend() const noexcept;

  template <typename... Args>
  reference emplaceBack(Args&&... args);

  template <typename... Args>
  reference emplace_back(Args&&... args) {
    return emplaceBack(std::forward<Args>(args)...);
  }

  void pushBack(const_reference value);

  void push_back(const_reference value) {
    pushBack(value);
  }

  void pushBack(value_type&& value);

  void push_back(value_type&& value) {
    pushBack(std::move(value));
  }

  void popBack();

  void pop_back() {
    popBack();
  }

  template <typename... Args>
  iterator emplace(const_iterator position, Args&&... args);

  iterator insert(const_iterator position, const_reference value);

  iterator insert(const_iterator position, value_type&& value);

  iterator insert(const_iterator position, size_type count, const value_type& value);

  template <std::input_iterator InputIt>
  iterator insert(const_iterator position, InputIt first, InputIt last);

  iterator insert(const_iterator position, std::initializer_list<value_type> values);

  iterator erase(const_iterator position);

  iterator erase(const_iterator first, const_iterator last);

  using IVector::resize;

  void resize(size_type newSize, const_reference value);

#pragma endregion Element Accessors

#pragma region STL
  /**
   * @brief Returns a copy of the allocator used by the vector.
   *
   * @return A copy of the allocator used by the vector.
   */
  [[nodiscard]] allocator_type get_allocator() const noexcept {
    return m_allocator;
  }
#pragma endregion STL

  void swap(Vector& other) noexcept(traits::propagate_on_container_swap::value || traits::is_always_equal::value);

  friend void swap(Vector& left, Vector& right) noexcept(noexcept(left.swap(right))) {
    left.swap(right);
  }

  friend bool operator==(const Vector& left, const Vector& right) {
    return left.size() == right.size() && std::equal(left.begin(), left.end(), right.begin());
  }

  friend bool operator!=(const Vector& left, const Vector& right) {
    return !(left == right);
  }

  friend bool operator<(const Vector& left, const Vector& right) {
    return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
  }

  friend bool operator<=(const Vector& left, const Vector& right) {
    return !(right < left);
  }

  friend bool operator>(const Vector& left, const Vector& right) {
    return right < left;
  }

  friend bool operator>=(const Vector& left, const Vector& right) {
    return !(left < right);
  }

  /**
   * @brief Returns the maximum number of elements that the vector can hold, based on the allocator's max_size and the
   * size of the element type. This method calculates the maximum number of elements that can be stored in the vector by
   * dividing the maximum size allowed by the allocator by the size of the element type.
   *
   * @return The maximum number of elements that the vector can hold.
   */
  [[nodiscard]] std::size_t maxSize() const noexcept override;

 protected:
#pragma region Virtuals
  void constructAt(void* destination) noexcept override;
  void constructAt(void* destination, void* source) noexcept override;
  void destroyAt(void* element) noexcept override;
  void* allocateStorage(std::size_t capacity) override;
  void deallocateStorage(void* data, std::size_t capacity) noexcept override;
#pragma endregion Virtuals

 private:
  Vector(const Vector& other, const allocator_type& allocator);

  [[nodiscard]] allocator_type copyAssignmentAllocator(const Vector& other) const;

  void replaceWith(Vector& other);

  void swapStorage(Vector& other) noexcept;

  void swapAll(Vector& other) noexcept(std::is_nothrow_swappable_v<allocator_type>);

  void adoptStorage(Vector& other) noexcept;

  void checkIndex(size_type index) const;

  [[nodiscard]] size_type checkedAdd(size_type lhs, size_type rhs) const;

  void ensureGrowthFor(size_type additional);

  [[nodiscard]] size_type recommendedCapacity(size_type minimum) const;

  [[nodiscard]] size_type insertCapacity(size_type additional) const;

  template <typename... Args>
  iterator reallocateAndEmplace(size_type index, size_type newCapacity, Args&&... args);

  template <typename... Args>
  iterator rebuildAndEmplace(size_type index, size_type newCapacity, Args&&... args);

  static consteval bool canShiftSingleInsertInPlace();

  [[nodiscard]] size_type indexForPosition(const_iterator position, bool allowEnd) const;

  allocator_type m_allocator;
};

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector() noexcept(std::is_nothrow_default_constructible_v<allocator_type>)
    : Vector(allocator_type()) {}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(const allocator_type& allocator) noexcept(std::is_nothrow_copy_constructible_v<allocator_type>)
    : IVector(sizeof(T), alignof(T)), m_allocator(allocator) {}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(const size_type count, const allocator_type& allocator) : Vector(allocator) {
  IVector::resize(count);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(const size_type count, const value_type& value, const allocator_type& allocator)
    : Vector(allocator) {
  assign(count, value);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::initializer_list<value_type> values, const allocator_type& allocator)
    : Vector(values.begin(), values.end(), allocator) {}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector& other)
    : Vector(other, traits::select_on_container_copy_construction(other.m_allocator)) {}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector&& other) noexcept(std::is_nothrow_move_constructible_v<allocator_type>)
    : IVector(std::exchange(other.m_data, nullptr), std::exchange(other.m_size, 0), std::exchange(other.m_capacity, 0),
              sizeof(T), alignof(T)),
      m_allocator(std::move(other.m_allocator)) {}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector&& other, const allocator_type& allocator) : Vector(allocator) {
  if constexpr (traits::is_always_equal::value) {
    adoptStorage(other);
    return;
  }

  if (m_allocator == other.m_allocator) {
    adoptStorage(other);
    return;
  }

  reserve(other.m_size);
  size_type i = 0;

  try {
    auto p = reinterpret_cast<pointer>(m_data);
    auto other_p = reinterpret_cast<pointer>(other.m_data);
    for (; i < other.m_size; ++i) {
      traits::construct(m_allocator, p + i, std::move_if_noexcept(other_p[i]));
    }
    m_size = other.m_size;
  } catch (...) {
    auto p = reinterpret_cast<pointer>(m_data);
    for (; i > 0; --i) {
      traits::destroy(m_allocator, p + (i - 1));
    }
    IVector::deallocateStorage();
    m_data = nullptr;
    m_capacity = 0;
    throw;
  }

  other.clear();
}

template <typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator=(const Vector& other) {
  if (this == &other) {
    return *this;
  }

  allocator_type targetAllocator = copyAssignmentAllocator(other);
  Vector tmp(other, targetAllocator);

  reset();
  if constexpr (traits::propagate_on_container_copy_assignment::value) {
    m_allocator = std::move(targetAllocator);
  }
  adoptStorage(tmp);
  return *this;
}

template <typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator=(Vector&& other) noexcept(
    traits::propagate_on_container_move_assignment::value || traits::is_always_equal::value) {
  if (this == &other) {
    return *this;
  }

  if constexpr (traits::propagate_on_container_move_assignment::value) {
    reset();
    m_allocator = std::move(other.m_allocator);
    adoptStorage(other);
    return *this;
  }

  if constexpr (traits::is_always_equal::value) {
    reset();
    adoptStorage(other);
    return *this;
  }

  if (m_allocator == other.m_allocator) {
    reset();
    adoptStorage(other);
    return *this;
  }

  Vector tmp(m_allocator);
  tmp.reserve(other.m_size);
  auto other_p = reinterpret_cast<pointer>(other.m_data);
  for (size_type i = 0; i < other.m_size; ++i) {
    tmp.emplaceBack(std::move_if_noexcept(other_p[i]));
  }

  reset();
  adoptStorage(tmp);
  other.clear();
  return *this;
}

template <typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator=(std::initializer_list<value_type> values) {
  assign(values);
  return *this;
}

template <typename T, typename Alloc>
Vector<T, Alloc>::~Vector() noexcept {
  reset();
}

template <typename T, typename Alloc>
Vector<T, Alloc>::reference Vector<T, Alloc>::at(const size_type index) {
  checkIndex(index);
  return reinterpret_cast<pointer>(m_data)[index];
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_reference Vector<T, Alloc>::at(const size_type index) const {
  checkIndex(index);
  return reinterpret_cast<const_pointer>(m_data)[index];
}

template <typename T, typename Alloc>
Vector<T, Alloc>::reference Vector<T, Alloc>::operator[](const size_type index) noexcept {
  return reinterpret_cast<pointer>(m_data)[index];
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_reference Vector<T, Alloc>::operator[](const size_type index) const noexcept {
  return reinterpret_cast<const_pointer>(m_data)[index];
}

template <typename T, typename Alloc>
Vector<T, Alloc>::reference Vector<T, Alloc>::front() {
  if (empty()) {
    throw std::out_of_range("Vector is empty");
  }
  return reinterpret_cast<pointer>(m_data)[0];
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_reference Vector<T, Alloc>::front() const {
  if (empty()) {
    throw std::out_of_range("Vector is empty");
  }
  return reinterpret_cast<const_pointer>(m_data)[0];
}

template <typename T, typename Alloc>
Vector<T, Alloc>::reference Vector<T, Alloc>::back() {
  if (empty()) {
    throw std::out_of_range("Vector is empty");
  }
  return reinterpret_cast<pointer>(m_data)[m_size - 1];
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_reference Vector<T, Alloc>::back() const {
  if (empty()) {
    throw std::out_of_range("Vector is empty");
  }
  return reinterpret_cast<const_pointer>(m_data)[m_size - 1];
}

template <typename T, typename Alloc>
Vector<T, Alloc>::pointer Vector<T, Alloc>::data() noexcept {
  return reinterpret_cast<pointer>(m_data);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_pointer Vector<T, Alloc>::data() const noexcept {
  return reinterpret_cast<const_pointer>(m_data);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::assign(const size_type count, const value_type& value) {
  Vector tmp(m_allocator);
  if (count > 0) {
    tmp.reserve(count);
    for (size_type i = 0; i < count; ++i) {
      tmp.emplaceBack(value);
    }
  }
  replaceWith(tmp);
}

template <typename T, typename Alloc>
template <std::input_iterator InputIt>
void Vector<T, Alloc>::assign(InputIt first, InputIt last) {
  Vector tmp(m_allocator);
  if constexpr (std::forward_iterator<InputIt>) {
    const auto count = static_cast<size_type>(std::distance(first, last));
    if (count > 0) {
      tmp.reserve(count);
    }
  }

  for (; first != last; ++first) {
    tmp.emplaceBack(*first);
  }

  replaceWith(tmp);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::assign(std::initializer_list<value_type> values) {
  assign(values.begin(), values.end());
}

template <typename T, typename Alloc>
Vector<T, Alloc>::iterator Vector<T, Alloc>::begin() noexcept {
  return iterator(reinterpret_cast<pointer>(m_data));
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_iterator Vector<T, Alloc>::begin() const noexcept {
  return const_iterator(reinterpret_cast<const_pointer>(m_data));
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_iterator Vector<T, Alloc>::cbegin() const noexcept {
  return const_iterator(reinterpret_cast<const_pointer>(m_data));
}

template <typename T, typename Alloc>
Vector<T, Alloc>::iterator Vector<T, Alloc>::end() noexcept {
  return iterator(reinterpret_cast<pointer>(m_data) + m_size);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_iterator Vector<T, Alloc>::end() const noexcept {
  return const_iterator(reinterpret_cast<const_pointer>(m_data) + m_size);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_iterator Vector<T, Alloc>::cend() const noexcept {
  return const_iterator(reinterpret_cast<const_pointer>(m_data) + m_size);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::reverse_iterator Vector<T, Alloc>::rbegin() noexcept {
  return reverse_iterator(end());
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::rbegin() const noexcept {
  return const_reverse_iterator(end());
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::crbegin() const noexcept {
  return const_reverse_iterator(cend());
}

template <typename T, typename Alloc>
Vector<T, Alloc>::reverse_iterator Vector<T, Alloc>::rend() noexcept {
  return reverse_iterator(begin());
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::rend() const noexcept {
  return const_reverse_iterator(begin());
}

template <typename T, typename Alloc>
Vector<T, Alloc>::const_reverse_iterator Vector<T, Alloc>::crend() const noexcept {
  return const_reverse_iterator(cbegin());
}

template <typename T, typename Alloc>
template <typename... Args>
Vector<T, Alloc>::reference Vector<T, Alloc>::emplaceBack(Args&&... args) {
  ensureGrowthFor(1);
  traits::construct(m_allocator, reinterpret_cast<pointer>(m_data) + m_size, std::forward<Args>(args)...);
  ++m_size;
  return back();
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::pushBack(const_reference value) {
  emplaceBack(value);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::pushBack(value_type&& value) {
  emplaceBack(std::move(value));
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::popBack() {
  if (empty()) {
    throw std::out_of_range("Vector is empty");
  }
  --m_size;
  std::destroy_at(reinterpret_cast<pointer>(m_data) + m_size);
}

template <typename T, typename Alloc>
template <typename... Args>
Vector<T, Alloc>::iterator Vector<T, Alloc>::emplace(const_iterator position, Args&&... args) {
  const size_type index = indexForPosition(position, true);
  if (index == m_size) {
    emplaceBack(std::forward<Args>(args)...);
    return iterator(reinterpret_cast<pointer>(m_data) + (m_size - 1));
  }

  if (m_size == m_capacity) {
    return reallocateAndEmplace(index, recommendedCapacity(checkedAdd(m_size, 1)), std::forward<Args>(args)...);
  }

  if constexpr (canShiftSingleInsertInPlace()) {
    auto p = reinterpret_cast<pointer>(m_data);
    value_type value(std::forward<Args>(args)...);
    traits::construct(m_allocator, p + m_size, std::move_if_noexcept(p[m_size - 1]));

    for (size_type i = m_size - 1; i > index; --i) {
      p[i] = std::move_if_noexcept(p[i - 1]);
    }

    p[index] = std::move_if_noexcept(value);
    ++m_size;
    return iterator(p + index);
  }

  return rebuildAndEmplace(index, insertCapacity(1), std::forward<Args>(args)...);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(const_iterator position, const_reference value) {
  return emplace(position, value);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(const_iterator position, value_type&& value) {
  return emplace(position, std::move(value));
}

template <typename T, typename Alloc>
Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(const_iterator position, const size_type count,
                                                    const value_type& value) {
  const size_type index = indexForPosition(position, true);
  if (count == 0) {
    return iterator(reinterpret_cast<pointer>(m_data) + index);
  }

  Vector tmp(m_allocator);
  tmp.reserve(insertCapacity(count));

  for (size_type i = 0; i < index; ++i) {
    tmp.emplaceBack(std::move_if_noexcept(reinterpret_cast<pointer>(m_data)[i]));
  }

  for (size_type i = 0; i < count; ++i) {
    tmp.emplaceBack(value);
  }

  for (size_type i = index; i < m_size; ++i) {
    tmp.emplaceBack(std::move_if_noexcept(reinterpret_cast<pointer>(m_data)[i]));
  }

  replaceWith(tmp);
  return iterator(reinterpret_cast<pointer>(m_data) + index);
}

template <typename T, typename Alloc>
template <std::input_iterator InputIt>
Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(const_iterator position, InputIt first, InputIt last) {
  const size_type index = indexForPosition(position, true);
  Vector tmp(m_allocator);

  if constexpr (std::forward_iterator<InputIt>) {
    const auto count = static_cast<size_type>(std::distance(first, last));
    tmp.reserve(insertCapacity(count));
  } else {
    tmp.reserve(m_capacity);
  }

  for (size_type i = 0; i < index; ++i) {
    tmp.emplaceBack(std::move_if_noexcept(reinterpret_cast<pointer>(m_data)[i]));
  }

  for (; first != last; ++first) {
    tmp.emplaceBack(*first);
  }

  for (size_type i = index; i < m_size; ++i) {
    tmp.emplaceBack(std::move_if_noexcept(reinterpret_cast<pointer>(m_data)[i]));
  }

  replaceWith(tmp);
  return iterator(reinterpret_cast<pointer>(m_data) + index);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(const_iterator position, std::initializer_list<value_type> values) {
  return insert(position, values.begin(), values.end());
}

template <typename T, typename Alloc>
Vector<T, Alloc>::iterator Vector<T, Alloc>::erase(const_iterator position) {
  const size_type index = indexForPosition(position, false);
  return erase(cbegin() + static_cast<difference_type>(index), cbegin() + static_cast<difference_type>(index + 1));
}

template <typename T, typename Alloc>
Vector<T, Alloc>::iterator Vector<T, Alloc>::erase(const_iterator first, const_iterator last) {
  const size_type beginIndex = indexForPosition(first, true);
  const size_type endIndex = indexForPosition(last, true);

  if (endIndex < beginIndex) {
    throw std::out_of_range("Vector erase range is invalid");
  }

  if (beginIndex == endIndex) {
    return iterator(reinterpret_cast<pointer>(m_data) + beginIndex);
  }

  const size_type count = endIndex - beginIndex;

  if constexpr (std::is_nothrow_move_assignable_v<value_type> || std::is_nothrow_copy_assignable_v<value_type>) {
    auto p = reinterpret_cast<pointer>(m_data);
    for (size_type i = beginIndex; i + count < m_size; ++i) {
      p[i] = std::move_if_noexcept(p[i + count]);
    }

    IVector::destroyRange(m_data, m_size - count, m_size);
    m_size -= count;
    return iterator(p + beginIndex);
  }

  Vector tmp(m_allocator);
  tmp.reserve(m_capacity);
  auto p = reinterpret_cast<pointer>(m_data);

  for (size_type i = 0; i < beginIndex; ++i) {
    tmp.emplaceBack(std::move_if_noexcept(p[i]));
  }

  for (size_type i = endIndex; i < m_size; ++i) {
    tmp.emplaceBack(std::move_if_noexcept(p[i]));
  }

  replaceWith(tmp);
  return iterator(reinterpret_cast<pointer>(m_data) + beginIndex);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::resize(const size_type newSize, const_reference value) {
  IVector::resizeAndConstruct(
      newSize, [&](void* destination) { traits::construct(m_allocator, static_cast<pointer>(destination), value); });
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::swap(Vector& other) noexcept(traits::propagate_on_container_swap::value ||
                                                    traits::is_always_equal::value) {
  if (this == &other) {
    return;
  }

  if constexpr (traits::propagate_on_container_swap::value) {
    swapAll(other);
    return;
  }

  if constexpr (traits::is_always_equal::value) {
    swapStorage(other);
    return;
  }

  if (m_allocator == other.m_allocator) {
    swapStorage(other);
    return;
  }

  Vector tmp(std::move(other), m_allocator);
  other = std::move(*this);
  *this = std::move(tmp);
}

template <typename T, typename Alloc>
std::size_t Vector<T, Alloc>::maxSize() const noexcept {
  return traits::max_size(m_allocator);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::constructAt(void* destination) noexcept {
  traits::construct(m_allocator, static_cast<pointer>(destination));
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::constructAt(void* destination, void* source) noexcept {
  reference ref = *static_cast<pointer>(source);
  traits::construct(m_allocator, static_cast<pointer>(destination), std::move_if_noexcept(ref));
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::destroyAt(void* element) noexcept {
  traits::destroy(m_allocator, static_cast<pointer>(element));
}

template <typename T, typename Alloc>
void* Vector<T, Alloc>::allocateStorage(const std::size_t capacity) {
  return traits::allocate(m_allocator, capacity);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::deallocateStorage(void* data, std::size_t capacity) noexcept {
  traits::deallocate(m_allocator, reinterpret_cast<pointer>(data), capacity);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector& other, const allocator_type& allocator) : Vector(allocator) {
  if (other.m_size == 0) {
    return;
  }

  reserve(other.m_size);

  size_type i = 0;
  try {
    auto p = reinterpret_cast<pointer>(m_data);
    auto other_p = reinterpret_cast<const_pointer>(other.m_data);
    for (; i < other.m_size; ++i) {
      traits::construct(m_allocator, p + i, other_p[i]);
    }
    m_size = other.m_size;
  } catch (...) {
    IVector::destroyRange(m_data, 0, i);
    IVector::deallocateStorage();
    m_data = nullptr;
    m_capacity = 0;
    throw;
  }
}

template <typename T, typename Alloc>
Vector<T, Alloc>::allocator_type Vector<T, Alloc>::copyAssignmentAllocator(const Vector& other) const {
  if constexpr (traits::propagate_on_container_copy_assignment::value) {
    return other.m_allocator;
  }

  return m_allocator;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::replaceWith(Vector& other) {
  reset();
  adoptStorage(other);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::swapStorage(Vector& other) noexcept {
  using std::swap;
  swap(m_data, other.m_data);
  swap(m_size, other.m_size);
  swap(m_capacity, other.m_capacity);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::swapAll(Vector& other) noexcept(std::is_nothrow_swappable_v<allocator_type>) {
  using std::swap;
  swapStorage(other);
  swap(m_allocator, other.m_allocator);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::adoptStorage(Vector& other) noexcept {
  m_data = std::exchange(other.m_data, nullptr);
  m_size = std::exchange(other.m_size, 0);
  m_capacity = std::exchange(other.m_capacity, 0);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::checkIndex(const size_type index) const {
  if (index >= m_size) {
    throw std::out_of_range("Vector index out of range");
  }
}

template <typename T, typename Alloc>
Vector<T, Alloc>::size_type Vector<T, Alloc>::checkedAdd(const size_type lhs, const size_type rhs) const {
  if (rhs > maxSize() - lhs) {
    throw std::length_error("Vector size exceeds max_size");
  }
  return lhs + rhs;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::ensureGrowthFor(const size_type additional) {
  const size_type required = checkedAdd(m_size, additional);
  ensureBaseCapacityRequest(required);
  if (required > m_capacity) {
    reallocate(recommendedCapacity(required));
  }
}

template <typename T, typename Alloc>
Vector<T, Alloc>::size_type Vector<T, Alloc>::recommendedCapacity(const size_type minimum) const {
  ensureBaseCapacityRequest(minimum);

  size_type max_sz = maxSize();
  size_type result = m_capacity == 0 ? 1 : m_capacity;
  while (result < minimum) {
    const size_type doubled = result > max_sz / 2 ? max_sz : result * 2;
    if (doubled <= result) {
      return minimum;
    }
    result = doubled;
  }

  return result;
}

template <typename T, typename Alloc>
Vector<T, Alloc>::size_type Vector<T, Alloc>::insertCapacity(const size_type additional) const {
  const size_type required = checkedAdd(m_size, additional);
  if (required <= m_capacity) {
    return m_capacity;
  }
  return recommendedCapacity(required);
}

template <typename T, typename Alloc>
template <typename... Args>
Vector<T, Alloc>::iterator Vector<T, Alloc>::reallocateAndEmplace(const size_type index, const size_type newCapacity,
                                                                  Args&&... args) {
  pointer data = traits::allocate(m_allocator, newCapacity);
  size_type constructed = 0;
  auto p = reinterpret_cast<pointer>(m_data);

  try {
    for (; constructed < index; ++constructed) {
      traits::construct(m_allocator, data + constructed, std::move_if_noexcept(p[constructed]));
    }

    traits::construct(m_allocator, data + index, std::forward<Args>(args)...);
    ++constructed;

    for (size_type i = index; i < m_size; ++i, ++constructed) {
      traits::construct(m_allocator, data + constructed, std::move_if_noexcept(p[i]));
    }
  } catch (...) {
    for (; constructed > 0; --constructed) {
      traits::destroy(m_allocator, data + (constructed - 1));
    }
    traits::deallocate(m_allocator, data, newCapacity);
    throw;
  }

  for (size_type i = m_size; i > 0; --i) {
    traits::destroy(m_allocator, p + (i - 1));
  }
  IVector::deallocateStorage();
  m_data = data;
  m_size = constructed;
  m_capacity = newCapacity;
  return iterator(data + index);
}

template <typename T, typename Alloc>
template <typename... Args>
Vector<T, Alloc>::iterator Vector<T, Alloc>::rebuildAndEmplace(const size_type index, const size_type newCapacity,
                                                               Args&&... args) {
  Vector tmp(m_allocator);
  tmp.reserve(newCapacity);
  auto p = reinterpret_cast<pointer>(m_data);

  for (size_type i = 0; i < index; ++i) {
    tmp.emplaceBack(std::move_if_noexcept(p[i]));
  }

  tmp.emplaceBack(std::forward<Args>(args)...);

  for (size_type i = index; i < m_size; ++i) {
    tmp.emplaceBack(std::move_if_noexcept(p[i]));
  }

  replaceWith(tmp);
  return iterator(reinterpret_cast<pointer>(m_data) + index);
}

template <typename T, typename Alloc>
consteval bool Vector<T, Alloc>::canShiftSingleInsertInPlace() {
  return (std::is_nothrow_move_constructible_v<value_type> || std::is_nothrow_copy_constructible_v<value_type>) &&
         (std::is_nothrow_move_assignable_v<value_type> || std::is_nothrow_copy_assignable_v<value_type>);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::size_type Vector<T, Alloc>::indexForPosition(const const_iterator position,
                                                               const bool allowEnd) const {
  const_pointer raw = position.base();
  auto p = reinterpret_cast<const_pointer>(m_data);

  if (p == nullptr) {
    if (allowEnd && raw == nullptr) {
      return 0;
    }
    throw std::out_of_range("Vector iterator position is invalid");
  }

  const_pointer beginPtr = p;
  const_pointer endPtr = p + m_size;
  if (raw < beginPtr || raw > endPtr || (!allowEnd && raw == endPtr)) {
    throw std::out_of_range("Vector iterator position is invalid");
  }

  return static_cast<size_type>(raw - beginPtr);
}
}  // namespace core::container
