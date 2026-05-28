#pragma once

#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace core {
/**
 * @brief A contiguous, allocator-aware dynamic array with vector-like semantics.
 *
 * The class owns a single contiguous buffer and grows it on demand. It provides the element-access, capacity,
 * modifier, and iterator APIs needed by the RTTI array subsystem while remaining usable as a general-purpose
 * container.
 *
 * @tparam T The element type stored in the container.
 * @tparam Allocator The allocator type used for storage management.
 */
template <typename T, typename Allocator = std::allocator<T>>
class Vector {
 public:
  /**
   * @brief The value type stored in the container.
   */
  using value_type = T;
  /**
   * @brief The allocator type used for storage management.
   */
  using allocator_type = Allocator;
  /**
   * @brief The size type used by the container.
   */
  using size_type = std::size_t;
  /**
   * @brief The difference type used by iterators.
   */
  using difference_type = std::ptrdiff_t;
  /**
   * @brief The reference type to an element.
   */
  using reference = value_type&;
  /**
   * @brief The const reference type to an element.
   */
  using const_reference = const value_type&;
  /**
   * @brief The pointer type to an element.
   */
  using pointer = value_type*;
  /**
   * @brief The const pointer type to an element.
   */
  using const_pointer = const value_type*;
  /**
   * @brief The iterator type.
   */
  using iterator = pointer;
  /**
   * @brief The const iterator type.
   */
  using const_iterator = const_pointer;
  /**
   * @brief The reverse iterator type.
   */
  using reverse_iterator = std::reverse_iterator<iterator>;
  /**
   * @brief The const reverse iterator type.
   */
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /**
   * @brief Constructs an empty vector with a default-constructed allocator.
   */
  Vector() noexcept(std::is_nothrow_default_constructible_v<Allocator>) = default;

  /**
   * @brief Constructs an empty vector with the supplied allocator.
   *
   * @param allocator The allocator instance that should manage the storage.
   */
  explicit Vector(const Allocator& allocator) noexcept : m_allocator(allocator) {}

  /**
   * @brief Constructs a vector with @c count copies of @code value@endcode.
   *
   * @param count The number of elements to create.
   * @param value The value to copy into each element.
   * @param allocator The allocator instance that should manage the storage.
   */
  Vector(const size_type count, const T& value, const Allocator& allocator = Allocator()) : Vector(allocator) {
    resize(count, value);
  }

  /**
   * @brief Constructs a vector with @c count default-initialized elements.
   *
   * @param count The number of elements to create.
   * @param allocator The allocator instance that should manage the storage.
   */
  explicit Vector(const size_type count, const Allocator& allocator = Allocator()) : Vector(allocator) {
    resize(count);
  }

  /**
   * @brief Constructs a vector from an initializer list.
   *
   * @param values The elements that should be copied into the vector.
   * @param allocator The allocator instance that should manage the storage.
   */
  Vector(std::initializer_list<T> values, const Allocator& allocator = Allocator()) : Vector(allocator) {
    reserve(values.size());
    for (const auto& value : values) {
      pushBack(value);
    }
  }

  /**
   * @brief Constructs a vector from a pair of iterators.
   *
   * @tparam InputIt The iterator type used to traverse the source range.
   * @param first The first element in the source range.
   * @param last One past the last element in the source range.
   * @param allocator The allocator instance that should manage the storage.
   */
  template <typename InputIt>
    requires std::input_iterator<InputIt> && std::constructible_from<T, std::iter_reference_t<InputIt>>
  Vector(InputIt first, InputIt last, const Allocator& allocator = Allocator()) : Vector(allocator) {
    for (; first != last; ++first) {
      pushBack(static_cast<T>(*first));
    }
  }

  /**
   * @brief Constructs a vector by copying another vector.
   *
   * @param other The vector to copy.
   */
  Vector(const Vector& other)
      : Vector(std::allocator_traits<Allocator>::select_on_container_copy_construction(other.m_allocator)) {
    reserve(other.size());
    for (const auto& value : other) {
      pushBack(value);
    }
  }

  /**
   * @brief Constructs a vector by moving another vector.
   *
   * @param other The vector to move from.
   */
  Vector(Vector&& other) noexcept
      : m_allocator(std::move(other.m_allocator)),
        m_data(other.m_data),
        m_size(other.m_size),
        m_capacity(other.m_capacity) {
    other.m_data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;
  }

  /**
   * @brief Destroys the vector and releases all owned storage.
   */
  ~Vector() {
    release();
  }

  /**
   * @brief Assigns the contents of another vector to this one.
   *
   * @param other The vector to copy.
   * @return A reference to this vector.
   */
  Vector& operator=(const Vector& other) {
    if (this != &other) {
      Vector tmp(other);
      swap(tmp);
    }
    return *this;
  }

  /**
   * @brief Assigns the contents of another vector by moving from it.
   *
   * @param other The vector to move from.
   * @return A reference to this vector.
   */
  Vector& operator=(Vector&& other) noexcept {
    if (this != &other) {
      release();
      m_allocator = std::move(other.m_allocator);
      m_data = other.m_data;
      m_size = other.m_size;
      m_capacity = other.m_capacity;
      other.m_data = nullptr;
      other.m_size = 0;
      other.m_capacity = 0;
    }
    return *this;
  }

  /**
   * @brief Assigns the vector from an initializer list.
   *
   * @param values The values that should replace the current contents.
   * @return A reference to this vector.
   */
  Vector& operator=(std::initializer_list<T> values) {
    Vector tmp(values, m_allocator);
    swap(tmp);
    return *this;
  }

  /**
   * @brief Returns the allocator used by the container.
   *
   * @return A copy of the allocator.
   */
  [[nodiscard]] allocator_type allocator() const noexcept {
    return m_allocator;
  }

  /**
   * @brief Returns the number of elements stored in the vector.
   *
   * @return The current number of stored elements.
   */
  [[nodiscard]] size_type size() const noexcept {
    return m_size;
  }

  /**
   * @brief Returns the current storage capacity of the vector.
   *
   * @return The number of elements the vector can store without reallocating.
   */
  [[nodiscard]] size_type capacity() const noexcept {
    return m_capacity;
  }

  /**
   * @brief Returns the largest number of elements that can be stored.
   *
   * @return The maximum supported element count.
   */
  [[nodiscard]] size_type maxSize() const noexcept {
    return std::allocator_traits<Allocator>::max_size(m_allocator);
  }

  /**
   * @brief Returns whether the vector is empty.
   *
   * @return @c true when the vector contains no elements.
   */
  [[nodiscard]] bool empty() const noexcept {
    return m_size == 0;
  }

  /**
   * @brief Returns a pointer to the first element.
   *
   * @return A pointer to the first element, or @c nullptr if the vector is empty.
   */
  [[nodiscard]] pointer data() noexcept {
    return m_data;
  }

  /**
   * @brief Returns a const pointer to the first element.
   *
   * @return A const pointer to the first element, or @c nullptr if the vector is empty.
   */
  [[nodiscard]] const_pointer data() const noexcept {
    return m_data;
  }

  /**
   * @brief Returns a reference to the element at the specified index.
   *
   * @param index The zero-based element index.
   * @return A reference to the element at @code index@endcode.
   * @throws std::out_of_range if @c index is not less than @code size()@endcode.
   */
  [[nodiscard]] reference at(size_type index) {
    if (index >= m_size) {
      throw std::out_of_range("core::Vector::at index out of range");
    }
    return m_data[index];
  }

  /**
   * @brief Returns a const reference to the element at the specified index.
   *
   * @param index The zero-based element index.
   * @return A const reference to the element at @code index@endcode.
   * @throws std::out_of_range if @c index is not less than @code size()@endcode.
   */
  [[nodiscard]] const_reference at(size_type index) const {
    if (index >= m_size) {
      throw std::out_of_range("core::Vector::at index out of range");
    }
    return m_data[index];
  }

  /**
   * @brief Returns a reference to the element at the specified index without bounds checking.
   *
   * @param index The zero-based element index.
   * @return A reference to the element at @code index@endcode.
   */
  [[nodiscard]] reference operator[](size_type index) noexcept {
    return m_data[index];
  }

  /**
   * @brief Returns a const reference to the element at the specified index without bounds checking.
   *
   * @param index The zero-based element index.
   * @return A const reference to the element at @code index@endcode.
   */
  [[nodiscard]] const_reference operator[](size_type index) const noexcept {
    return m_data[index];
  }

  /**
   * @brief Returns a reference to the first element.
   *
   * @return A reference to the first element.
   * @throws std::out_of_range if the vector is empty.
   */
  [[nodiscard]] reference front() {
    if (empty()) {
      throw std::out_of_range("core::Vector::front on empty vector");
    }
    return m_data[0];
  }

  /**
   * @brief Returns a const reference to the first element.
   *
   * @return A const reference to the first element.
   * @throws std::out_of_range if the vector is empty.
   */
  [[nodiscard]] const_reference front() const {
    if (empty()) {
      throw std::out_of_range("core::Vector::front on empty vector");
    }
    return m_data[0];
  }

  /**
   * @brief Returns a reference to the last element.
   *
   * @return A reference to the last element.
   * @throws std::out_of_range if the vector is empty.
   */
  [[nodiscard]] reference back() {
    if (empty()) {
      throw std::out_of_range("core::Vector::back on empty vector");
    }
    return m_data[m_size - 1];
  }

  /**
   * @brief Returns a const reference to the last element.
   *
   * @return A const reference to the last element.
   * @throws std::out_of_range if the vector is empty.
   */
  [[nodiscard]] const_reference back() const {
    if (empty()) {
      throw std::out_of_range("core::Vector::back on empty vector");
    }
    return m_data[m_size - 1];
  }

  /**
   * @brief Returns a pointer to the first element.
   *
   * @return A pointer to the first element, or @c end() when the vector is empty.
   */
  [[nodiscard]] iterator begin() noexcept {
    return m_data;
  }

  /**
   * @brief Returns a const pointer to the first element.
   *
   * @return A const pointer to the first element, or @c end() when the vector is empty.
   */
  [[nodiscard]] const_iterator begin() const noexcept {
    return m_data;
  }

  /**
   * @brief Returns a const pointer to the first element.
   *
   * @return A const pointer to the first element, or @c end() when the vector is empty.
   */
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return begin();
  }

  /**
   * @brief Returns a pointer one past the last element.
   *
   * @return A pointer one past the last element.
   */
  [[nodiscard]] iterator end() noexcept {
    return m_data ? m_data + m_size : nullptr;
  }

  /**
   * @brief Returns a const pointer one past the last element.
   *
   * @return A const pointer one past the last element.
   */
  [[nodiscard]] const_iterator end() const noexcept {
    return m_data ? m_data + m_size : nullptr;
  }

  /**
   * @brief Returns a const pointer one past the last element.
   *
   * @return A const pointer one past the last element.
   */
  [[nodiscard]] const_iterator cend() const noexcept {
    return end();
  }

  /**
   * @brief Returns a reverse iterator to the last element.
   *
   * @return A reverse iterator to the last element.
   */
  [[nodiscard]] reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }

  /**
   * @brief Returns a const reverse iterator to the last element.
   *
   * @return A const reverse iterator to the last element.
   */
  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }

  /**
   * @brief Returns a const reverse iterator to the last element.
   *
   * @return A const reverse iterator to the last element.
   */
  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return rbegin();
  }

  /**
   * @brief Returns a reverse iterator one before the first element.
   *
   * @return A reverse iterator one before the first element.
   */
  [[nodiscard]] reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }

  /**
   * @brief Returns a const reverse iterator one before the first element.
   *
   * @return A const reverse iterator one before the first element.
   */
  [[nodiscard]] const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }

  /**
   * @brief Returns a const reverse iterator one before the first element.
   *
   * @return A const reverse iterator one before the first element.
   */
  [[nodiscard]] const_reverse_iterator crend() const noexcept {
    return rend();
  }

  /**
   * @brief Releases all elements while preserving capacity.
   */
  void clear() noexcept {
    destroyRange(m_data, m_size);
    m_size = 0;
  }

  /**
   * @brief Reserves storage for at least @c newCapacity elements.
   *
   * @param newCapacity The minimum capacity the vector should provide after the call.
   */
  void reserve(const size_type newCapacity) {
    if (newCapacity > m_capacity) {
      reallocate(newCapacity);
    }
  }

  /**
   * @brief Shrinks the capacity so it matches the current size.
   */
  void shrinkToFit() {
    if (m_size < m_capacity) {
      reallocate(m_size);
    }
  }

  /**
   * @brief Resizes the vector to contain @c count elements.
   *
   * @param count The desired element count.
   */
  void resize(const size_type count) {
    resizeImpl(count, nullptr, false);
  }

  /**
   * @brief Resizes the vector to contain @c count elements, filling new entries with @code value@endcode.
   *
   * @param count The desired element count.
   * @param value The value used to initialize any new elements.
   */
  void resize(const size_type count, const T& value) {
    resizeImpl(count, &value, true);
  }

  /**
   * @brief Appends a copy of @c value to the vector.
   *
   * @param value The value to append.
   */
  void pushBack(const T& value) {
    emplaceBack(value);
  }

  /**
   * @brief Appends a moved value to the vector.
   *
   * @param value The value to append.
   */
  void pushBack(T&& value) {
    emplaceBack(std::move(value));
  }

  /**
   * @brief Removes the last element from the vector.
   *
   * @throws std::out_of_range if the vector is empty.
   */
  void popBack() {
    if (empty()) {
      throw std::out_of_range("core::Vector::popBack on empty vector");
    }
    traits::destroy(m_allocator, m_data + (m_size - 1));
    --m_size;
  }

  /**
   * @brief Constructs a new element in place at the end of the vector.
   *
   * @tparam Args The construction argument pack.
   * @param args The arguments used to construct the new element.
   * @return A reference to the inserted element.
   */
  template <typename... Args>
  reference emplaceBack(Args&&... args) {
    ensureCapacityForAppend();
    std::allocator_traits<Allocator>::construct(m_allocator, m_data + m_size, std::forward<Args>(args)...);
    ++m_size;
    return back();
  }

  /**
   * @brief Inserts a copy of @c value before @code pos@endcode.
   *
   * @param pos The position at which the element should be inserted.
   * @param value The value to insert.
   * @return An iterator pointing to the inserted element.
   */
  iterator insert(const_iterator pos, const T& value) {
    return insertImpl(pos, value);
  }

  /**
   * @brief Inserts a moved value before @code pos@endcode.
   *
   * @param pos The position at which the element should be inserted.
   * @param value The value to insert.
   * @return An iterator pointing to the inserted element.
   */
  iterator insert(const_iterator pos, T&& value) {
    return insertImpl(pos, std::move(value));
  }

  /**
   * @brief Erases the element at @c pos and returns an iterator to the following element.
   *
   * @param pos The position of the element to erase.
   * @return An iterator to the element that followed the erased one, or @c end() if the last element was erased.
   */
  iterator erase(const const_iterator pos) {
    return eraseImpl(pos);
  }

  /**
   * @brief Swaps the contents of this vector with another vector.
   *
   * @param other The vector to swap with.
   */
  void swap(Vector& other) noexcept {
    using std::swap;
    swap(m_allocator, other.m_allocator);
    swap(m_data, other.m_data);
    swap(m_size, other.m_size);
    swap(m_capacity, other.m_capacity);
  }

  /**
   * @brief Compares two vectors for element-wise equality.
   *
   * @param other The vector to compare with this one.
   * @return @c true if the vectors have the same size and equal elements.
   */
  bool operator==(const Vector& other) const {
    if (m_size != other.m_size) {
      return false;
    }
    return std::equal(cbegin(), cend(), other.cbegin());
  }

  /**
   * @brief Compares two vectors for element-wise inequality.
   *
   * @param other The vector to compare with this one.
   * @return @c true if the vectors differ.
   */
  bool operator!=(const Vector& other) const {
    return !(*this == other);
  }

 private:
  /**
   * @brief Traits type used for allocator operations.
   */
  using traits = std::allocator_traits<Allocator>;

  /**
   * @brief Releases owned storage and resets the vector.
   */
  void release() noexcept {
    clear();
    if (m_data != nullptr) {
      traits::deallocate(m_allocator, m_data, m_capacity);
      m_data = nullptr;
      m_capacity = 0;
    }
  }

  /**
   * @brief Returns the next capacity used for growth operations.
   *
   * @param minimumCapacity The minimum capacity required.
   * @return The capacity that should be allocated.
   */
  [[nodiscard]] size_type nextCapacity(const size_type minimumCapacity) const {
    const size_type doubled = m_capacity == 0 ? 1 : m_capacity * 2;
    return std::max(minimumCapacity, doubled);
  }

  /**
   * @brief Allocates raw storage for @c capacity elements.
   *
   * @param capacity The number of elements worth of storage to allocate.
   * @return A pointer to the allocated storage, or @c nullptr when @c capacity is zero.
   */
  [[nodiscard]] pointer allocate(size_type capacity) {
    return capacity == 0 ? nullptr : traits::allocate(m_allocator, capacity);
  }

  /**
   * @brief Destroys a contiguous range of elements.
   *
   * @param data The first element in the range.
   * @param count The number of elements in the range.
   */
  void destroyRange(pointer data, const size_type count) noexcept {
    if (data == nullptr) {
      return;
    }

    for (size_type i = 0; i < count; ++i) {
      traits::destroy(m_allocator, data + i);
    }
  }

  /**
   * @brief Reallocates storage and preserves the current contents.
   *
   * @param newCapacity The new storage capacity.
   */
  void reallocate(size_type newCapacity) {
    pointer newData = nullptr;
    size_type constructed = 0;

    try {
      newData = allocate(newCapacity);
      for (; constructed < m_size; ++constructed) {
        traits::construct(m_allocator, newData + constructed, std::move_if_noexcept(m_data[constructed]));
      }
    } catch (...) {
      destroyRange(newData, constructed);
      traits::deallocate(m_allocator, newData, newCapacity);
      throw;
    }

    destroyRange(m_data, m_size);
    if (m_data != nullptr) {
      traits::deallocate(m_allocator, m_data, m_capacity);
    }

    m_data = newData;
    m_capacity = newCapacity;
  }

  /**
   * @brief Ensures capacity is available for one additional element.
   */
  void ensureCapacityForAppend() {
    if (m_size == m_capacity) {
      reserve(nextCapacity(m_size + 1));
    }
  }

  /**
   * @brief Resizes the vector, optionally filling new elements with a value.
   *
   * @param count The desired size.
   * @param value The fill value, or @c nullptr when default construction should be used.
   * @param hasValue Whether @c value points to a valid fill value.
   */
  void resizeImpl(size_type count, const T* value, const bool hasValue) {
    if (count < m_size) {
      destroyRange(m_data + count, m_size - count);
      m_size = count;
      return;
    }

    if (count > m_capacity) {
      reserve(nextCapacity(count));
    }

    for (; m_size < count; ++m_size) {
      if (hasValue) {
        traits::construct(m_allocator, m_data + m_size, *value);
      } else {
        if constexpr (std::is_default_constructible_v<T>) {
          traits::construct(m_allocator, m_data + m_size);
        } else {
          static_assert(std::is_default_constructible_v<T>,
                        "core::Vector::resize(count) requires default constructible T");
        }
      }
    }
  }

  /**
   * @brief Inserts a value before @c pos and preserves the previous contents.
   *
   * @tparam U The value category used for insertion.
   * @param pos The insertion position.
   * @param value The value that should be inserted.
   * @return An iterator to the inserted element.
   */
  template <typename U>
  iterator insertImpl(const_iterator pos, U&& value) {
    const auto index = static_cast<size_type>(pos - cbegin());
    if (index > m_size) {
      throw std::out_of_range("core::Vector::insert position out of range");
    }

    const size_type newSize = m_size + 1;
    const size_type newCapacity = m_capacity >= newSize ? m_capacity : nextCapacity(newSize);
    pointer newData = nullptr;
    size_type constructed = 0;

    try {
      newData = allocate(newCapacity);
      for (; constructed < index; ++constructed) {
        traits::construct(m_allocator, newData + constructed, m_data[constructed]);
      }

      traits::construct(m_allocator, newData + constructed, std::forward<U>(value));
      ++constructed;

      for (size_type oldIndex = index; oldIndex < m_size; ++oldIndex, ++constructed) {
        traits::construct(m_allocator, newData + constructed, m_data[oldIndex]);
      }
    } catch (...) {
      destroyRange(newData, constructed);
      traits::deallocate(m_allocator, newData, newCapacity);
      throw;
    }

    destroyRange(m_data, m_size);
    if (m_data != nullptr) {
      traits::deallocate(m_allocator, m_data, m_capacity);
    }

    m_data = newData;
    m_size = newSize;
    m_capacity = newCapacity;
    return m_data + index;
  }

  /**
   * @brief Erases a value at @c pos and preserves the previous contents.
   *
   * @param pos The position of the value that should be removed.
   * @return An iterator to the element that followed the erased element.
   */
  iterator eraseImpl(const_iterator pos) {
    const auto index = static_cast<size_type>(pos - cbegin());
    if (index >= m_size) {
      throw std::out_of_range("core::Vector::erase position out of range");
    }

    const size_type newSize = m_size - 1;
    pointer newData = nullptr;
    size_type constructed = 0;

    try {
      newData = allocate(m_capacity);
      for (; constructed < index; ++constructed) {
        traits::construct(m_allocator, newData + constructed, m_data[constructed]);
      }

      for (size_type oldIndex = index + 1; oldIndex < m_size; ++oldIndex, ++constructed) {
        traits::construct(m_allocator, newData + constructed, m_data[oldIndex]);
      }
    } catch (...) {
      destroyRange(newData, constructed);
      traits::deallocate(m_allocator, newData, m_capacity);
      throw;
    }

    destroyRange(m_data, m_size);
    if (m_data != nullptr) {
      traits::deallocate(m_allocator, m_data, m_capacity);
    }

    m_data = newData;
    m_size = newSize;
    return m_data + index;
  }

  /**
   * @brief This allocator instance manages vector storage.
   */
  allocator_type m_allocator{};

  /**
   * @brief This pointer references the beginning of the contiguous storage.
   */
  pointer m_data = nullptr;

  /**
   * @brief This value stores the current number of constructed elements.
   */
  size_type m_size = 0;

  /**
   * @brief This value stores the number of elements the current storage can hold.
   */
  size_type m_capacity = 0;
};

/**
 * @brief Deduction guide that deduces the element type from an initializer list.
 *
 * @tparam T The deduced element type.
 */
template <typename T>
Vector(std::initializer_list<T>) -> Vector<T>;

/**
 * @brief Deduction guide that deduces the element type from a size/value construction.
 *
 * @tparam T The deduced element type.
 */
template <typename T>
Vector(std::size_t, const T&) -> Vector<T>;
}  // namespace core
