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

  /**
   * @brief Returns a reference to the first element in the vector. If the vector is empty a std::out_of_range exception
   * will be thrown.
   *
   * @return A reference to the first element in the vector.
   * @throws std::out_of_range If the vector is empty.
   */
  [[nodiscard]] reference front();

  /**
   * @brief Returns a const reference to the first element in the vector. If the vector is empty a std::out_of_range
   * exception will be thrown.
   *
   * @return A const reference to the first element in the vector.
   * @throws std::out_of_range If the vector is empty.
   */
  [[nodiscard]] const_reference front() const;

  /**
   * @brief Returns a reference to the last element in the vector. If the vector is empty a std::out_of_range exception
   * will be thrown.
   *
   * @return A reference to the last element in the vector.
   * @throws std::out_of_range If the vector is empty.
   */
  [[nodiscard]] reference back();

  /**
   * @brief Returns a const reference to the last element in the vector. If the vector is empty a std::out_of_range
   * exception will be thrown.
   *
   * @return A const reference to the last element in the vector.
   * @throws std::out_of_range If the vector is empty.
   */
  [[nodiscard]] const_reference back() const;

  /**
   * @brief Returns a pointer to the underlying array serving as element storage. The returned pointer is such that
   * range [data(), data() + size()) is valid and the elements in this range are stored contiguously.
   *
   * @return A pointer to the underlying array serving as element storage, or nullptr if the vector has no elements.
   */
  [[nodiscard]] pointer data() noexcept;

  /**
   * @brief Returns a const pointer to the underlying array serving as element storage. The returned pointer is such
   * that range [data(), data() + size()) is valid and the elements in this range are stored contiguously.
   *
   * @return A const pointer to the underlying array serving as element storage, or nullptr if the vector has no
   * elements.
   */
  [[nodiscard]] const_pointer data() const noexcept;

  /**
   * @brief Assigns new contents to the vector, replacing its current contents, and modifying its size accordingly. The
   * new contents are initialized as copies of the given value.
   *
   * @param count The number of elements to assign to the vector. The vector will be resized to contain exactly this
   * many elements after the assignment.
   * @param value The value to assign to the elements of the vector. Each element in the vector will be assigned a copy
   * of this value.
   */
  void assign(size_type count, const value_type& value);

  /**
   * @brief Assigns new contents to the vector, replacing its current contents, and modifying its size accordingly. The
   * new contents are initialized with the values in the range [first, last).
   *
   * @tparam InputIt The type of the input iterators.
   * @param first An input iterator pointing to the first element in the range to assign from.
   * @param last An input iterator pointing to one past the last element in the range to assign from. The elements in
   * the range [first, last) will be copied into the vector, replacing its current contents.
   */
  template <std::input_iterator InputIt>
  void assign(InputIt first, InputIt last);

  /**
   * @brief Assigns new contents to the vector, replacing its current contents, and modifying its size accordingly. The
   * new contents are initialized with the values in the given initializer list.
   *
   * @param values An initializer list containing the values to assign to the vector. The elements in this initializer
   * list will be copied into the vector, replacing its current contents.
   */
  void assign(std::initializer_list<value_type> values);

  /**
   * @brief Returns an iterator to the first element in the vector. If the vector is empty, this will return the same
   * value as end().
   *
   * @return An iterator to the first element in the vector, or end() if the vector is empty.
   */
  [[nodiscard]] iterator begin() noexcept;

  /**
   * @brief Returns a const iterator to the first element in the vector. If the vector is empty, this will return the
   * same value as end().
   *
   * @return A const iterator to the first element in the vector, or end() if the vector is empty.
   */
  [[nodiscard]] const_iterator begin() const noexcept;

  /**
   * @brief Returns a const iterator to the first element in the vector. If the vector is empty, this will return the
   * same value as end().
   *
   * @return A const iterator to the first element in the vector, or end() if the vector is empty.
   */
  [[nodiscard]] const_iterator cbegin() const noexcept;

  /**
   * @brief Returns an iterator to the element following the last element in the vector. This element acts as a
   * placeholder; it is not a valid element, and it should not be dereferenced. If the vector is empty, this will return
   * the same value as begin().
   *
   * @return An iterator to the element following the last element in the vector, or begin() if the vector is empty.
   */
  [[nodiscard]] iterator end() noexcept;

  /**
   * @brief Returns a const iterator to the element following the last element in the vector. This element acts as a
   * placeholder; it is not a valid element, and it should not be dereferenced. If the vector is empty, this will return
   * the same value as begin().
   *
   * @return A const iterator to the element following the last element in the vector, or begin() if the vector is
   * empty.
   */
  [[nodiscard]] const_iterator end() const noexcept;

  /**
   * @brief Returns a const iterator to the element following the last element in the vector. This element acts as a
   * placeholder; it is not a valid element, and it should not be dereferenced. If the vector is empty, this will return
   * the same value as begin().
   *
   * @return A const iterator to the element following the last element in the vector, or begin() if the vector is
   * empty.
   */
  [[nodiscard]] const_iterator cend() const noexcept;

  /**
   * @brief Returns a reverse iterator to the first element of the reversed vector. This iterator points to the last
   * element in the vector, and iterating from rbegin() to rend() will traverse the vector in reverse order. If the
   * vector is empty, rbegin() will return the same value as rend().
   *
   * @return A reverse iterator to the first element of the reversed vector, or rend() if the vector is empty.
   */
  [[nodiscard]] reverse_iterator rbegin() noexcept;

  /**
   * @brief Returns a const reverse iterator to the first element of the reversed vector. This iterator points to the
   * last element in the vector, and iterating from rbegin() to rend() will traverse the vector in reverse order. If the
   * vector is empty, rbegin() will return the same value as rend().
   *
   * @return A const reverse iterator to the first element of the reversed vector, or rend() if the vector is empty.
   */
  [[nodiscard]] const_reverse_iterator rbegin() const noexcept;

  /**
   * @brief Returns a const reverse iterator to the first element of the reversed vector. This iterator points to the
   * last element in the vector, and iterating from rbegin() to rend() will traverse the vector in reverse order. If the
   * vector is empty, rbegin() will return the same value as rend().
   *
   * @return A const reverse iterator to the first element of the reversed vector, or rend() if the vector is empty.
   */
  [[nodiscard]] const_reverse_iterator crbegin() const noexcept;

  /**
   * @brief Returns a reverse iterator to the element following the last element of the reversed vector. This element
   * acts as a placeholder; it is not a valid element, and it should not be dereferenced. If the vector is empty, rend()
   * will return the same value as rbegin().
   *
   * @return A reverse iterator to the element following the last element of the reversed vector, or rbegin() if the
   * vector is empty.
   */
  [[nodiscard]] reverse_iterator rend() noexcept;

  /**
   * @brief Returns a const reverse iterator to the element following the last element of the reversed vector. This
   * element acts as a placeholder; it is not a valid element, and it should not be dereferenced. If the vector is
   * empty, rend() will return the same value as rbegin().
   *
   * @return A const reverse iterator to the element following the last element of the reversed vector, or rbegin() if
   * the vector is empty.
   */
  [[nodiscard]] const_reverse_iterator rend() const noexcept;

  /**
   * @brief Returns a const reverse iterator to the element following the last element of the reversed vector. This
   * element acts as a placeholder; it is not a valid element, and it should not be dereferenced. If the vector is
   * empty, rend() will return the same value as rbegin().
   *
   * @return A const reverse iterator to the element following the last element of the reversed vector, or rbegin() if
   * the vector is empty.
   */
  [[nodiscard]] const_reverse_iterator crend() const noexcept;

  /**
   * @brief Constructs a new element in place at the end of the vector, using the provided arguments to construct the
   * element. The provided arguments are forwarded to the constructor of the element type, allowing for efficient
   * construction of new elements directly in the vector's storage.
   *
   * This method will ensure that the vector has enough capacity to accommodate the new element, and it will handle any
   * necessary reallocation and copying of existing elements if the current capacity is insufficient. After the new
   * element is constructed, it will be added to the end of the vector, and the size of the vector will be increased by
   * one.
   *
   * @tparam Args The types of the arguments to forward to the constructor of the element type. These can be any types
   * that are compatible with the constructor of the element type.
   * @param args The arguments to forward to the constructor of the element type. These will be perfectly forwarded,
   * allowing for efficient construction of the new element in place.
   * @return A reference to the newly constructed element at the end of the vector. This reference can be used to access
   * or modify the new element after it has been added to the vector.
   */
  template <typename... Args>
  reference emplaceBack(Args&&... args);

  /**
   * @brief Adds a new element to the end of the vector by copying the provided value. This method will ensure that the
   * vector has enough capacity to accommodate the new element, and it will handle any necessary reallocation and
   * copying of existing elements if the current capacity is insufficient. After the new element is added, the size of
   * the vector will be increased by one.
   *
   * @param value The value to copy into the new element at the end of the vector. This value will be copied, so it
   * should be a type that is copyable.
   */
  void pushBack(const_reference value);

  /**
   * @brief Adds a new element to the end of the vector by moving the provided value. This method will ensure that the
   * vector has enough capacity to accommodate the new element, and it will handle any necessary reallocation and
   * copying of existing elements if the current capacity is insufficient. After the new element is added, the size of
   * the vector will be increased by one.
   *
   * @param value The value to move into the new element at the end of the vector. This value will be moved, so it
   * should be a type that is moveable.
   */
  void pushBack(value_type&& value);

  /**
   * @brief Removes the last element from the vector. This method will destroy the last element in the vector and reduce
   * the size of the vector by one. If the vector is empty, this method will have no effect. After this method is
   * called, the last element will no longer be part of the vector, and any access to it will be undefined behavior.
   */
  void popBack();

  /**
   * @brief Constructs a new element in place at the specified position in the vector, using the provided arguments to
   * construct the element. The provided arguments are forwarded to the constructor of the element type, allowing for
   * efficient construction of new elements directly in the vector's storage.
   *
   * @tparam Args The types of the arguments to forward to the constructor of the element type. These can be any types
   * that are compatible with the constructor of the element type.
   * @param position An iterator pointing to the position in the vector where the new element should be constructed. The
   * new element will be inserted before the element currently at this position.
   * @param args The arguments to forward to the constructor of the element type. These will be perfectly forwarded,
   * allowing for efficient construction of the new element in place.
   * @return An iterator pointing to the newly constructed element in the vector. This iterator can be used to access or
   * modify the new element after it has been added to the vector.
   */
  template <typename... Args>
  iterator emplace(const_iterator position, Args&&... args);

  /**
   * @brief Inserts a new element into the vector at the specified position by copying the provided value. This method
   * will ensure that the vector has enough capacity to accommodate the new element, and it will handle any necessary
   * reallocation and copying of existing elements if the current capacity is insufficient. After the new element is
   * added, the size of the vector will be increased by one.
   *
   * @param position An iterator pointing to the position in the vector where the new element should be inserted. The
   * new element will be inserted before the element currently at this position.
   * @param value The value to copy into the new element at the specified position in the vector. This value will be
   * copied, so it should be a type that is copyable.
   * @return An iterator pointing to the newly inserted element in the vector. This iterator can be used to access or
   * modify the new element after it has been added to the vector.
   */
  iterator insert(const_iterator position, const_reference value);

  /**
   * @brief Inserts a new element into the vector at the specified position by moving the provided value. This method
   * will ensure that the vector has enough capacity to accommodate the new element, and it will handle any necessary
   * reallocation and copying of existing elements if the current capacity is insufficient. After the new element is
   * added, the size of the vector will be increased by one.
   *
   * @param position An iterator pointing to the position in the vector where the new element should be inserted. The
   * new element will be inserted before the element currently at this position.
   * @param value The value to move into the new element at the specified position in the vector. This value will be
   * moved, so it should be a type that is moveable.
   * @return An iterator pointing to the newly inserted element in the vector. This iterator can be used to access or
   * modify the new element after it has been added to the vector.
   */
  iterator insert(const_iterator position, value_type&& value);

  /**
   * @brief Inserts count copies of the given value into the vector at the specified position. This method will ensure
   * that the vector has enough capacity to accommodate the new elements, and it will handle any necessary reallocation
   * and copying of existing elements if the current capacity is insufficient. After the new elements are added, the
   * size of the vector will be increased by count.
   *
   * @param position An iterator pointing to the position in the vector where the new elements should be inserted. The
   * new elements will be inserted before the element currently at this position.
   * @param count The number of copies of the value to insert into the vector. The vector will be resized to contain
   * exactly this many new elements after the insertion.
   * @param value The value to copy into the new elements at the specified position in the vector. This value will be
   * copied, so it should be a type that is copyable.
   * @return An iterator pointing to the first of the newly inserted elements in the vector. This iterator can be used
   * to access or modify the new elements after they have been added to the vector.
   */
  iterator insert(const_iterator position, size_type count, const value_type& value);

  /**
   * @brief Inserts new elements into the vector at the specified position by copying the values from the range [first,
   * last). This method will ensure that the vector has enough capacity to accommodate the new elements, and it will
   * handle any necessary reallocation and copying of existing elements if the current capacity is insufficient. After
   * the new elements are added, the size of the vector will be increased by the number of elements in the range [first,
   * last).
   *
   * @tparam InputIt The type of the input iterators.
   * @param position An iterator pointing to the position in the vector where the new elements should be inserted. The
   * new elements will be inserted before the element currently at this position.
   * @param first An input iterator pointing to the first element in the range to copy from. The elements in the range
   * [first, last) will be copied into the vector, replacing its current contents.
   * @param last An input iterator pointing to one past the last element in the range to copy from. The elements in the
   * range [first, last) will be copied into the vector, replacing its current contents.
   * @return An iterator pointing to the first of the newly inserted elements in the vector. This iterator can be used
   * to access or modify the new elements after they have been added to the vector.
   */
  template <std::input_iterator InputIt>
  iterator insert(const_iterator position, InputIt first, InputIt last);

  /**
   * @brief Inserts new elements into the vector at the specified position by copying the values from the given
   * initializer list. This method will ensure that the vector has enough capacity to accommodate the new elements, and
   * it will handle any necessary reallocation and copying of existing elements if the current capacity is insufficient.
   * After the new elements are added, the size of the vector will be increased by the number of elements in the
   * initializer list.
   *
   * @param position An iterator pointing to the position in the vector where the new elements should be inserted. The
   * new elements will be inserted before the element currently at this position.
   * @param values An initializer list containing the values to copy into the vector. The elements in this initializer
   * list will be copied into the vector, replacing its current contents.
   * @return An iterator pointing to the first of the newly inserted elements in the vector. This iterator can be used
   * to access or modify the new elements after they have been added to the vector.
   */
  iterator insert(const_iterator position, std::initializer_list<value_type> values);

  /**
   * @brief Removes the element at the specified position in the vector. This method will destroy the element at the
   * given position and reduce the size of the vector by one. The elements following the erased element will be moved to
   * fill the gap left by the erased element, and the order of the remaining elements will be preserved. If the position
   * is out of range (i.e., not a valid iterator pointing to an element in the vector), the behavior is undefined. After
   * this method is called, the element at the specified position will no longer be part of the vector, and any access
   * to it will be undefined behavior.
   *
   * @param position An iterator pointing to the element in the vector that should be removed. This should be a valid
   * iterator pointing to an element in the vector; otherwise, the behavior is undefined.
   * @return An iterator pointing to the element that followed the erased element in the vector before the call to
   * erase. If the erased element was the last element in the vector, this will return end().
   * @throws std::out_of_range If the position is out of range (i.e., not a valid iterator pointing to an element in the
   * vector).
   */
  iterator erase(const_iterator position);

  /**
   * @brief Removes the elements in the range [first, last) from the vector. This method will destroy the elements in
   * the given range and reduce the size of the vector by the number of elements erased. The elements following the
   * erased elements will be moved to fill the gap left by the erased elements, and the order of the remaining elements
   * will be preserved. If the range [first, last) is out of range (i.e., not valid iterators pointing to elements in
   * the vector), the behavior is undefined. After this method is called, the elements in the specified range will no
   * longer be part of the vector, and any access to them will be undefined behavior.
   *
   * @param first An iterator pointing to the first element in the range to be removed from the vector. This should be a
   * valid iterator pointing to an element in the vector; otherwise, the behavior is undefined.
   * @param last An iterator pointing to one past the last element in the range to be removed from the vector. This
   * should be a valid iterator pointing to an element in the vector, and it should be greater than or equal to first;
   * otherwise, the behavior is undefined.
   * @return An iterator pointing to the element that followed the last erased element in the vector before the call to
   * erase. If the last erased element was the last element in the vector, this will return end().
   * @throws std::out_of_range If the range [first, last) is out of range (i.e., not valid iterators pointing to
   * elements in the vector).
   */
  iterator erase(const_iterator first, const_iterator last);

  /**
   * @brief Resizes the vector to contain newSize elements. If newSize is greater than the current size of the vector,
   * new elements will be default-constructed and added to the end of the vector until the size reaches newSize. If
   * newSize is less than the current size of the vector, the elements at the end of the vector will be destroyed until
   * the size reaches newSize. If newSize is equal to the current size of the vector, this method will have no effect.
   * After this method is called, the size of the vector will be newSize, and the elements in the vector will be
   * modified accordingly based on whether the size was increased or decreased.
   */
  using IVector::resize;

  /**
   * @brief Resizes the vector to contain newSize elements. If newSize is greater than the current size of the vector,
   * new elements will be copy-constructed from the provided value and added to the end of the vector until the size
   * reaches newSize. If newSize is less than the current size of the vector, the elements at the end of the vector will
   * be destroyed until the size reaches newSize. If newSize is equal to the current size of the vector, this method
   * will have no effect. After this method is called, the size of the vector will be newSize, and the elements in the
   * vector will be modified accordingly based on whether the size was increased or decreased.
   *
   * @param newSize The new size of the vector. If this value is greater than the current size of the vector, new
   * elements will be added; if it is less than the current size, elements will be removed. If it is equal to the
   * current size, the vector will remain unchanged.
   * @param value The value to copy into the new elements if the vector is resized to be larger. This value will be
   * copied, so it should be a type that is copyable.
   */
  void resize(size_type newSize, const_reference value);

#pragma endregion Element Accessors

#pragma region STL
  /**
   * @brief Constructs a new element in place at the end of the vector, using the provided arguments to construct the
   * element. The provided arguments are forwarded to the constructor of the element type, allowing for efficient
   * construction of new elements directly in the vector's storage.
   *
   * This method will ensure that the vector has enough capacity to accommodate the new element, and it will handle any
   * necessary reallocation and copying of existing elements if the current capacity is insufficient. After the new
   * element is constructed, it will be added to the end of the vector, and the size of the vector will be increased by
   * one.
   *
   * @tparam Args The types of the arguments to forward to the constructor of the element type. These can be any types
   * that are compatible with the constructor of the element type.
   * @param args The arguments to forward to the constructor of the element type. These will be perfectly forwarded,
   * allowing for efficient construction of the new element in place.
   * @return A reference to the newly constructed element at the end of the vector. This reference can be used to access
   * or modify the new element after it has been added to the vector.
   */
  template <typename... Args>
  reference emplace_back(Args&&... args) {
    return emplaceBack(std::forward<Args>(args)...);
  }

  /**
   * @brief Adds a new element to the end of the vector by copying the provided value. This method will ensure that the
   * vector has enough capacity to accommodate the new element, and it will handle any necessary reallocation and
   * copying of existing elements if the current capacity is insufficient. After the new element is added, the size of
   * the vector will be increased by one.
   *
   * @param value The value to copy into the new element at the end of the vector. This value will be copied, so it
   * should be a type that is copyable.
   */
  void push_back(const_reference value) {
    pushBack(value);
  }

  /**
   * @brief Adds a new element to the end of the vector by moving the provided value. This method will ensure that the
   * vector has enough capacity to accommodate the new element, and it will handle any necessary reallocation and
   * copying of existing elements if the current capacity is insufficient. After the new element is added, the size of
   * the vector will be increased by one.
   *
   * @param value The value to move into the new element at the end of the vector. This value will be moved, so it
   * should be a type that is moveable.
   */
  void push_back(value_type&& value) {
    pushBack(std::move(value));
  }

  /**
   * @brief Removes the last element from the vector. This method will destroy the last element in the vector and reduce
   * the size of the vector by one. If the vector is empty, this method will have no effect. After this method is
   * called, the last element will no longer be part of the vector, and any access to it will be undefined behavior.
   */
  void pop_back() {
    popBack();
  }

  /**
   * @brief Returns a copy of the allocator used by the vector.
   *
   * @return A copy of the allocator used by the vector.
   */
  [[nodiscard]] allocator_type get_allocator() const noexcept {
    return m_allocator;
  }
#pragma endregion STL

  /**
   * @brief Exchanges the contents of this vector with those of another vector. After this operation, the contents of
   * this vector will be replaced by the contents of the other vector, and the contents of the other vector will be
   * replaced by the contents of this vector.
   *
   * This method will swap the internal storage and size information of the two vectors, allowing for efficient exchange
   * of their contents without the need for copying individual elements. The behavior of this method is undefined if the
   * allocators of the two vectors are not compatible (i.e., if they do not compare equal or if the allocator traits do
   * not allow for propagation on container swap).
   *
   * @param other The other vector to swap contents with. This should be a vector of the same type and with a compatible
   * allocator; otherwise, the behavior is undefined.
   */
  void swap(Vector& other) noexcept(traits::propagate_on_container_swap::value || traits::is_always_equal::value);

  /**
   * @brief Exchanges the contents of two vectors. After this operation, the contents of the left vector will be
   * replaced by the contents of the right vector, and the contents of the right vector will be replaced by the contents
   * of the left vector.
   *
   * @param left The first vector to swap contents with. This should be a vector of the same type and with a compatible
   * allocator; otherwise, the behavior is undefined.
   * @param right The second vector to swap contents with. This should be a vector of the same type and with a
   * compatible allocator; otherwise, the behavior is undefined.
   */
  friend void swap(Vector& left, Vector& right) noexcept(noexcept(left.swap(right))) {
    left.swap(right);
  }

  /**
   * @brief Compares two vectors for equality. This operator returns true if the two vectors have the same size and
   * contain the same elements in the same order; otherwise, it returns false. The behavior of this operator is
   * undefined if the allocators of the two vectors are not compatible (i.e., if they do not compare equal or if the
   * allocator traits do not allow for propagation on container swap).
   *
   * @param left The first vector to compare for equality. This should be a vector of the same type and with a
   * compatible allocator; otherwise, the behavior is undefined.
   * @param right The second vector to compare for equality. This should be a vector of the same type and with a
   * compatible allocator; otherwise, the behavior is undefined.
   * @return true if the two vectors are equal (i.e., they have the same size and contain the same elements in the same
   * order), or false otherwise.
   */
  friend bool operator==(const Vector& left, const Vector& right) {
    return left.size() == right.size() && std::equal(left.begin(), left.end(), right.begin());
  }

  /**
   * @brief Compares two vectors for inequality. This operator returns true if the two vectors do not have the same size
   * or do not contain the same elements in the same order; otherwise, it returns false. The behavior of this operator
   * is undefined if the allocators of the two vectors are not compatible (i.e., if they do not compare equal or if the
   * allocator traits do not allow for propagation on container swap).
   *
   * @param left The first vector to compare for inequality. This should be a vector of the same type and with a
   * compatible allocator; otherwise, the behavior is undefined.
   * @param right The second vector to compare for inequality. This should be a vector of the same type and with a
   * compatible allocator; otherwise, the behavior is undefined.
   * @return true if the two vectors are not equal (i.e., they do not have the same size or do not contain the same
   * elements in the same order), or false otherwise.
   */
  friend bool operator!=(const Vector& left, const Vector& right) {
    return !(left == right);
  }

  /**
   * @brief Compares two vectors for lexicographical order. This operator returns true if the elements of the left
   * vector are lexicographically less than the elements of the right vector; otherwise, it returns false. The behavior
   * of this operator is undefined if the allocators of the two vectors are not compatible (i.e., if they do not compare
   * equal or if the allocator traits do not allow for propagation on container swap).
   *
   * @param left The first vector to compare for lexicographical order. This should be a vector of the same type and
   * with a compatible allocator; otherwise, the behavior is undefined.
   * @param right The second vector to compare for lexicographical order. This should be a vector of the same type and
   * with a compatible allocator; otherwise, the behavior is undefined.
   * @return true if the elements of the left vector are lexicographically less than the elements of the right vector,
   * or false otherwise.
   */
  friend bool operator<(const Vector& left, const Vector& right) {
    return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
  }

  /**
   * @brief Compares two vectors for lexicographical order. This operator returns true if the elements of the left
   * vector are lexicographically less than or equal to the elements of the right vector; otherwise, it returns false.
   * The behavior of this operator is undefined if the allocators of the two vectors are not compatible (i.e., if they
   * do not compare equal or if the allocator traits do not allow for propagation on container swap).
   *
   * @param left The first vector to compare for lexicographical order. This should be a vector of the same type and
   * with a compatible allocator; otherwise, the behavior is undefined.
   * @param right The second vector to compare for lexicographical order. This should be a vector of the same type and
   * with a compatible allocator; otherwise, the behavior is undefined.
   * @return true if the elements of the left vector are lexicographically less than or equal to the elements of the
   * right vector, or false otherwise.
   */
  friend bool operator<=(const Vector& left, const Vector& right) {
    return !(right < left);
  }

  /**
   * @brief Compares two vectors for lexicographical order. This operator returns true if the elements of the left
   * vector are lexicographically greater than the elements of the right vector; otherwise, it returns false. The
   * behavior of this operator is undefined if the allocators of the two vectors are not compatible (i.e., if they do
   * not compare equal or if the allocator traits do not allow for propagation on container swap).
   *
   * @param left The first vector to compare for lexicographical order. This should be a vector of the same type and
   * with a compatible allocator; otherwise, the behavior is undefined.
   * @param right The second vector to compare for lexicographical order. This should be a vector of the same type and
   * with a compatible allocator; otherwise, the behavior is undefined.
   * @return true if the elements of the left vector are lexicographically greater than the elements of the right
   * vector, or false otherwise.
   */
  friend bool operator>(const Vector& left, const Vector& right) {
    return right < left;
  }

  /**
   * @brief Compares two vectors for lexicographical order. This operator returns true if the elements of the left
   * vector are lexicographically greater than or equal to the elements of the right vector; otherwise, it returns
   * false. The behavior of this operator is undefined if the allocators of the two vectors are not compatible (i.e., if
   * they do not compare equal or if the allocator traits do not allow for propagation on container swap).
   *
   * @param left The first vector to compare for lexicographical order. This should be a vector of the same type and
   * with a compatible allocator; otherwise, the behavior is undefined.
   * @param right The second vector to compare for lexicographical order. This should be a vector of the same type and
   * with a compatible allocator; otherwise, the behavior is undefined.
   * @return true if the elements of the left vector are lexicographically greater than or equal to the elements of the
   * right vector, or false otherwise.
   */
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
  /**
   * @brief Constructs a new element in place at the specified destination in the vector's storage. This method is
   * responsible for constructing an element of the vector's value type at the given memory location, using the
   * allocator's construct method.
   *
   * The behavior of this method is undefined if the destination does not point to a valid memory location within the
   * vector's storage or if the memory at the destination is not properly aligned for the element type. After this
   * method is called, the memory at the destination will contain a validly constructed element of the vector's value
   * type, and it can be accessed and used as part of the vector's contents.
   *
   * @param destination A pointer to the memory location where the new element should be constructed. This should point
   * to a valid memory location within the vector's storage, and it should be properly aligned for the element type;
   * otherwise, the behavior is undefined.
   */
  void constructAt(void* destination) noexcept override;

  /**
   * @brief Constructs a new element in place at the specified destination in the vector's storage, using the provided
   * source value. This method is responsible for constructing an element of the vector's value type at the given memory
   * location, using the allocator's construct method and the provided source value.
   *
   * @param destination A pointer to the memory location where the new element should be constructed. This should point
   * to a valid memory location within the vector's storage, and it should be properly aligned for the element type;
   * otherwise, the behavior is undefined.
   * @param source A pointer to the value that should be used to construct the new element at the destination. This
   * should point to a valid value of the vector's element type; otherwise, the behavior is undefined.
   */
  void copyConstructAt(void* destination, const void* source) override;

  /**
   * @brief Constructs a new element in place at the specified destination in the vector's storage, using the provided
   * source value. This method is responsible for constructing an element of the vector's value type at the given memory
   * location, using the allocator's construct method and the provided source value.
   *
   * The behavior of this method is undefined if the destination does not point to a valid memory location within the
   * vector's storage or if the memory at the destination is not properly aligned for the element type. After this
   * method is called, the memory at the destination will contain a validly constructed element of the vector's value
   * type, initialized with the provided source value, and it can be accessed and used as part of the vector's contents.
   *
   * @param destination A pointer to the memory location where the new element should be constructed. This should point
   * to a valid memory location within the vector's storage, and it should be properly aligned for the element type;
   * otherwise, the behavior is undefined.
   * @param source A pointer to the value that should be used to construct the new element at the destination. This
   * should point to a valid value of the vector's element type; otherwise, the behavior is undefined.
   */
  void constructAt(void* destination, void* source) noexcept override;

  /**
   * @brief Destroys the element at the specified memory location in the vector's storage. This method is responsible
   * for destroying an element of the vector's value type at the given memory location, using the allocator's destroy
   * method.
   *
   * @param element A pointer to the memory location where the element to be destroyed is located.
   */
  void destroyAt(void* element) noexcept override;

  /**
   * @brief Allocates raw storage for the specified number of elements in the vector. This method is responsible for
   * allocating uninitialized memory that can hold the specified number of elements of the vector's value type, using
   * the allocator's allocate method.
   *
   * @param capacity The number of elements for which to allocate storage. This should be a non-negative value, and the
   * behavior is undefined if it exceeds the maximum size allowed by the allocator.
   * @return A pointer to the allocated raw storage that can hold the specified number of elements. This pointer should
   * point to a valid memory location that is properly aligned for the element type, and it should be used to construct
   * elements in place using the constructAt methods. The memory returned by this method is uninitialized, and it should
   * be properly deallocated using the deallocateStorage method when it is no longer needed.
   */
  void* allocateStorage(std::size_t capacity) override;

  /**
   * @brief Deallocates raw storage that was previously allocated for the vector's elements. This method is responsible
   * for deallocating memory that was allocated using the allocateStorage method, using the allocator's deallocate
   * method.
   *
   * @param data  A pointer to the raw storage that should be deallocated. This should point to a valid memory location
   * that was previously allocated using the allocateStorage method; otherwise, the behavior is undefined.
   * @param capacity The number of elements for which the storage was allocated. This should be the same value that was
   * used when the storage was allocated; otherwise, the behavior is undefined.
   */
  void deallocateStorage(void* data, std::size_t capacity) noexcept override;

 private:
  /**
   * @brief Constructs a new vector by copying the contents of another vector, using the specified allocator. This
   * constructor is responsible for creating a new vector that contains the same elements as the other vector, but it
   * uses the provided allocator for memory management instead of the allocator of the other vector. The behavior of
   * this constructor is undefined if the allocators of the two vectors are not compatible (i.e., if they do not compare
   * equal or if the allocator traits do not allow for propagation on container swap).
   *
   * @param other The other vector to copy from. This should be a vector of the same type and with a compatible
   * allocator; otherwise, the behavior is undefined. After this constructor is called, the new vector will contain the
   * same elements as the other vector, but it will use the provided allocator for memory management.
   * @param allocator The allocator to use for the new vector. This allocator will be used for memory management in the
   * new vector, and it should be compatible with the allocator of the other vector; otherwise, the behavior is
   * undefined. After this constructor is called, the new vector will use this allocator for all memory management
   * operations.
   */
  Vector(const Vector& other, const allocator_type& allocator);

  /**
   * @brief Determines the appropriate allocator to use for copy assignment from another vector. This method takes into
   * account the allocator of the other vector and the allocator traits to determine whether the current allocator
   * should be used, whether the other allocator should be used, or whether a new allocator should be created for the
   * copy assignment operation.
   *
   * @param other The other vector from which to copy assign. This should be a vector of the same type and with a
   * compatible allocator; otherwise, the behavior is undefined.
   * @return The allocator that should be used for the copy assignment operation. This will be either a copy of the
   * current allocator, a copy of the other vector's allocator, or a new allocator created for the copy assignment,
   * depending on the allocator traits and the relationship between the two allocators.
   */
  [[nodiscard]] allocator_type copyAssignmentAllocator(const Vector& other) const;

  /**
   * @brief Replaces the contents of this vector with the contents of another vector. This method is responsible for
   * replacing the internal storage and size information of this vector with that of the other vector, effectively
   * making this vector contain the same elements as the other vector. The behavior of this method is undefined if the
   * allocators of the two vectors are not compatible (i.e., if they do not compare equal or if the allocator traits do
   * not allow for propagation on container swap).
   *
   * @param other The other vector whose contents should replace the contents of this vector. This should be a vector of
   * the same type and with a compatible allocator; otherwise, the behavior is undefined.
   */
  void replaceWith(Vector& other);

  /**
   * @brief Exchanges the internal storage and size information of this vector with that of another vector. This method
   * is responsible for swapping the raw storage pointers, size, and capacity of the two vectors, allowing for efficient
   * exchange of their contents without the need for copying individual elements. The behavior of this method is
   * undefined if the allocators of the two vectors are not compatible (i.e., if they do not compare equal or if the
   * allocator traits do not allow for propagation on container swap).
   *
   * @param other The other vector to swap storage with. This should be a vector of the same type and with a compatible
   * allocator; otherwise, the behavior is undefined.
   */
  void swapStorage(Vector& other) noexcept;

  /**
   * @brief Exchanges the internal storage, size information, and allocator of this vector with that of another vector.
   * This method is responsible for swapping the raw storage pointers, size, capacity, and allocator of the two vectors,
   * allowing for efficient exchange of their contents and allocator without the need for copying individual elements.
   * The behavior of this method is undefined if the allocators of the two vectors are not compatible (i.e., if they do
   * not compare equal or if the allocator traits do not allow for propagation on container swap).
   *
   * @param other The other vector to swap storage and allocator with. This should be a vector of the same type and with
   * a compatible allocator; otherwise, the behavior is undefined.
   */
  void swapAll(Vector& other) noexcept(std::is_nothrow_swappable_v<allocator_type>);

  /**
   * @brief Adopts the internal storage and size information of another vector. This method is responsible for taking
   * ownership of the raw storage pointer, size, and capacity of the other vector and making this vector use that
   * storage as its own. After this method is called, this vector will contain the same elements as the other vector,
   * and the other vector will be left in a valid but unspecified state (typically empty). The behavior of this method
   * is undefined if the allocators of the two vectors are not compatible (i.e., if they do not compare equal or if the
   * allocator traits do not allow for propagation on container swap).
   *
   * @param other The other vector whose storage should be adopted by this vector. This should be a vector of the same
   * type and with a compatible allocator; otherwise, the behavior is undefined. After this method is called, this
   * vector will take ownership of the storage of the other vector, and the other vector will be left in a valid but
   * unspecified state (typically empty).
   */
  void adoptStorage(Vector& other) noexcept;

  /**
   * @brief Checks whether the given index is within the valid range of indices for the vector. This method is
   * responsible for verifying that the provided index is less than the current size of the vector, ensuring that any
   * access to elements at that index will be valid. If the index is out of range (i.e., greater than or equal to the
   * size of the vector), this method will throw a std::out_of_range exception, indicating that the index is invalid for
   * accessing elements in the vector. This method is typically used internally by element access methods (such as
   * operator[] and at()) to ensure safe access to the elements of the vector.
   *
   * @param index The index to check for validity. This should be a non-negative value, and the behavior is undefined if
   * it is negative. If this index is greater than or equal to the current size of the vector, this method will throw a
   * std::out_of_range exception.
   */
  void checkIndex(size_type index) const;

  /**
   * @brief Checks whether the addition of two size_type values would overflow. This method is responsible for verifying
   * that the sum of the two provided size_type values does not exceed the maximum value representable by size_type,
   * which would indicate an overflow. If the addition of the two values would overflow, this method will throw a
   * std::overflow_error exception, indicating that the operation cannot be performed safely. This method is typically
   * used internally by methods that need to calculate new sizes or capacities for the vector, ensuring that they do not
   * perform arithmetic operations that would result in overflow and lead to undefined behavior.
   *
   * @param lhs The first size_type value to add. This should be a non-negative value, and the behavior is undefined if
   * it is negative.
   * @param rhs The second size_type value to add. This should be a non-negative value, and the behavior is undefined
   * if it is negative. If the sum of lhs and rhs exceeds the maximum value representable by size_type, this method will
   * throw a std::overflow_error exception.
   * @return The sum of lhs and rhs if it does not overflow. If the addition overflows, this method will throw a
   * std::overflow_error exception instead of returning a value.
   */
  [[nodiscard]] size_type checkedAdd(size_type lhs, size_type rhs) const;

  /**
   * @brief Ensures that the vector has enough capacity to accommodate the addition of a specified number of elements.
   * This method is responsible for checking whether the current capacity of the vector is sufficient to hold the
   * existing elements plus the additional number of elements specified by the parameter. If the current capacity is
   * insufficient, this method will trigger a reallocation of the vector's storage to increase its capacity to
   * accommodate the new elements. The behavior of this method is undefined if the additional number of elements is
   * negative or if the resulting capacity would exceed the maximum size allowed by the allocator.
   *
   * @param additional The number of additional elements that need to be accommodated in the vector. This should be a
   * non-negative value, and the behavior is undefined if it is negative. If the current capacity of the vector is
   * insufficient to hold the existing elements plus this additional number of elements, this method will trigger a
   * reallocation to increase the vector's capacity.
   */
  void ensureGrowthFor(size_type additional);

  /**
   * @brief Calculates the recommended capacity for the vector based on a specified minimum capacity. This method is
   * responsible for determining an appropriate new capacity for the vector when it needs to grow, based on the provided
   * minimum capacity. The recommended capacity is typically calculated using a growth strategy (such as doubling the
   * current capacity) to ensure that the vector can accommodate future additions without needing to reallocate again
   * immediately. The behavior of this method is undefined if the minimum capacity is negative or if the resulting
   * recommended capacity would exceed the maximum size allowed by the allocator.
   *
   * @param minimum The minimum capacity that the recommended capacity should be at least equal to. This should be a
   * non-negative value, and the behavior is undefined if it is negative. The recommended capacity returned by this
   * method will be greater than or equal to this minimum value, ensuring that the vector can accommodate at least this
   * many elements after growth.
   * @return The recommended capacity for the vector, which will be greater than or equal to the provided minimum
   * capacity. This recommended capacity is calculated based on a growth strategy to allow for efficient future
   * additions to the vector without needing immediate reallocation again. If the minimum capacity is negative or if the
   * resulting recommended capacity would exceed the maximum size allowed by the allocator, this method may throw an
   * exception or result in undefined behavior.
   */
  [[nodiscard]] size_type recommendedCapacity(size_type minimum) const;

  /**
   * @brief Calculates the new capacity for the vector when inserting a specified number of additional elements. This
   * method is responsible for determining the new capacity that the vector should have when it needs to grow to
   * accommodate the addition of a specified number of elements. The new capacity is typically calculated using a growth
   * strategy (such as doubling the current capacity) to ensure that the vector can accommodate future additions without
   * needing to reallocate again immediately. The behavior of this method is undefined if the additional number of
   * elements is negative or if the resulting new capacity would exceed the maximum size allowed by the allocator.
   *
   * @param additional The number of additional elements that need to be accommodated in the vector. This should be a
   * non-negative value, and the behavior is undefined if it is negative. The new capacity returned by this method will
   * be calculated based on the current capacity and this additional number of elements, using a growth strategy to
   * allow for efficient future additions to the vector without needing immediate reallocation again. If the additional
   * number of elements is negative or if the resulting new capacity would exceed the maximum size allowed by the
   * allocator, this method may throw an exception or result in undefined behavior.
   * @return The new capacity for the vector, which will be calculated based on the current capacity and the provided
   * additional number of elements. This new capacity is determined using a growth strategy to ensure that the vector
   * can accommodate future additions without needing immediate reallocation again. If the additional number of elements
   * is negative or if the resulting new capacity would exceed the maximum size allowed by the allocator, this method
   * may throw an exception or result in undefined behavior.
   */
  [[nodiscard]] size_type insertCapacity(size_type additional) const;

  /**
   * @brief Reallocates the vector's storage to a new capacity and constructs a new element in place at the specified
   * index using the provided arguments. This method is responsible for reallocating the vector's storage to accommodate
   * a new capacity, and then constructing a new element at the specified index using the provided arguments. The
   * behavior of this method is undefined if the index is greater than the current size of the vector or if the new
   * capacity is less than the current size of the vector. After this method is called, the vector will have the new
   * capacity, and a new element will be constructed at the specified index using the provided arguments.
   *
   * @tparam Args The types of the arguments to forward to the constructor of the element type. These can be any types
   * that are compatible with the constructor of the element type.
   * @param index The index at which to construct the new element. This should be a non-negative value, and the behavior
   * is undefined if it is greater than the current size of the vector.
   * @param newCapacity The new capacity to which the vector's storage should be reallocated. This should be a
   * non-negative value, and the behavior is undefined if it is less than the current size of the vector.
   * @param args The arguments to forward to the constructor of the element type. These will be perfectly forwarded,
   * allowing for efficient construction of the new element in place at the specified index after the reallocation.
   * @return An iterator pointing to the newly constructed element at the specified index after the reallocation. This
   * iterator can be used to access or modify the new element after it has been added to the vector.
   */
  template <typename... Args>
  iterator reallocateAndEmplace(size_type index, size_type newCapacity, Args&&... args);

  /**
   * @brief Rebuilds the vector's storage to a new capacity and constructs a new element in place at the specified index
   * using the provided arguments. This method is responsible for rebuilding the vector's storage to accommodate a new
   * capacity, which may involve allocating new storage, moving existing elements to the new storage, and then
   * constructing a new element at the specified index using the provided arguments. The behavior of this method is
   * undefined if the index is greater than the current size of the vector or if the new capacity is less than the
   * current size of the vector. After this method is called, the vector will have the new capacity, and a new element
   * will be constructed at the specified index using the provided arguments, with existing elements moved as necessary
   * to accommodate the new element and the new capacity.
   *
   * @tparam Args The types of the arguments to forward to the constructor of the element type. These can be any types
   * that are compatible with the constructor of the element type.
   * @param index The index at which to construct the new element. This should be a non-negative value, and the behavior
   * is undefined if it is greater than the current size of the vector.
   * @param newCapacity The new capacity to which the vector's storage should be rebuilt. This should be a non-negative
   * value, and the behavior is undefined if it is less than the current size of the vector.
   * @param args The arguments to forward to the constructor of the element type. These will be perfectly forwarded,
   * allowing for efficient construction of the new element in place at the specified index after the rebuilding of the
   * storage.
   * @return An iterator pointing to the newly constructed element at the specified index after the rebuilding of the
   * storage. This iterator can be used to access or modify the new element after it has been added to the vector.
   */
  template <typename... Args>
  iterator rebuildAndEmplace(size_type index, size_type newCapacity, Args&&... args);

  /**
   * @brief Determines whether a single element can be inserted in place at the end of the vector without needing to
   * reallocate or move existing elements. This method checks whether the current capacity of the vector is sufficient
   * to accommodate one more element, and whether the insertion can be performed without needing to move existing
   * elements (i.e., if the insertion is at the end of the vector). If this method returns true, it indicates that a
   * single element can be inserted in place at the end of the vector without needing to reallocate or move existing
   * elements; if it returns false, it indicates that a reallocation or movement of existing elements would be necessary
   * to insert a single element at the end of the vector.
   *
   * @return true if a single element can be inserted in place at the end of the vector without needing to reallocate or
   * move existing elements, or false otherwise.
   */
  static consteval bool canShiftSingleInsertInPlace();

  /**
   * @brief Calculates the index in the vector corresponding to a given iterator position. This method is responsible
   * for determining the index of the element in the vector that corresponds to the provided iterator position. The
   * behavior of this method is undefined if the provided iterator position does not point to a valid element within the
   * vector or if it points to an element that is not part of the vector's storage. If the allowEnd parameter is true,
   * this method may also allow the position to point to one past the last element of the vector (i.e., the end
   * iterator), in which case it would return the index equal to the current size of the vector. If the allowEnd
   * parameter is false, the position must point to a valid element within the vector, and the method will return the
   * corresponding index of that element. This method is typically used internally by methods that need to convert
   * iterator positions to indices for accessing or modifying elements in the vector.
   *
   * @param position The iterator position for which to calculate the corresponding index in the vector. This should be
   * a valid iterator pointing to an element within the vector's storage, or (if allowEnd is true) it may also point to
   * one past the last element of the vector; otherwise, the behavior is undefined.
   * @param allowEnd A boolean flag indicating whether the position is allowed to point to one past the last element of
   * the vector (i.e., the end iterator). If this is true, the method may return an index equal to the current size of
   * the vector if the position points to the end iterator; if this is false, the position must point to a valid element
   * within the vector, and the method will return the corresponding index of that element.
   * @return The index in the vector corresponding to the given iterator position. This will be a non-negative value
   * less than the current size of the vector if allowEnd is false, or it may be equal to the current size of the vector
   * if allowEnd is true and the position points to the end iterator. If the position does not point to a valid element
   * within the vector or (if allowEnd is false) if it points to the end iterator, the behavior of this method is
   * undefined.
   */
  [[nodiscard]] size_type indexForPosition(const_iterator position, bool allowEnd) const;

  /**
   * @brief The allocator used by the vector to manage memory for its elements.
   */
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
void Vector<T, Alloc>::copyConstructAt(void* destination, const void* source) {
  if constexpr (std::is_copy_constructible_v<value_type>) {
    const_reference ref = *static_cast<const_pointer>(source);
    traits::construct(m_allocator, static_cast<pointer>(destination), ref);
  } else {
    throw std::logic_error("Vector element type is not copy constructible");
  }
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
