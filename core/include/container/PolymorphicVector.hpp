#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>

#include "IVector.hpp"
#include "PolymorphicIterator.hpp"

namespace core::container {
/**
 * @brief PolymorphicVector is a non-templated wrapper around IVector that provides a type-erased interface for
 * accessing and manipulating the elements of a vector-like container. It is designed to allow users to operate on the
 * underlying data buffer of a vector at runtime without knowing its type, while still providing a convenient and
 * familiar interface for working with the vector's elements.
 */
class PolymorphicVector {
 public:
  /**
   * @brief Constructor that initializes the PolymorphicVector with a reference to an IVector.
   *
   * @param vector A reference to an IVector that the PolymorphicVector will wrap. The PolymorphicVector does not take
   * ownership of the IVector, and it will not manage its memory or lifetime. The caller is responsible for ensuring
   * that the IVector remains valid for the lifetime of the PolymorphicVector.
   */
  explicit PolymorphicVector(IVector& vector) noexcept : m_vector(vector) {}

  /**
   * @brief Returns a pointer to the raw data buffer that holds the vector's elements. This is the same pointer that is
   * stored in the underlying IVector, and it allows users to access the raw memory directly if needed.
   *
   * @return A pointer to the raw data buffer that holds the vector's elements. This pointer is type-erased and should
   * be cast to the appropriate type by the caller based on the element type of the vector.
   */
  [[nodiscard]] void* data() noexcept {
    return m_vector.m_data;
  }

  /**
   * @brief Returns a const pointer to the raw data buffer that holds the vector's elements. This is the same pointer
   * that is stored in the underlying IVector, and it allows users to access the raw memory directly if needed, while
   * ensuring that it cannot be modified through this pointer.
   *
   * @return A const pointer to the raw data buffer that holds the vector's elements. This pointer is type-erased and
   * should be cast to the appropriate type by the caller based on the element type of the vector.
   */
  [[nodiscard]] const void* data() const noexcept {
    return m_vector.m_data;
  }

  /**
   * @brief Returns the number of elements currently stored in the vector. This is used to track the size of the vector
   * and manage element construction and destruction. The derived Vector class will update this value as elements are
   * added, removed, or resized, while the IVector simply provides a common storage mechanism for this information.
   *
   * @return The number of elements currently stored in the vector.
   */
  [[nodiscard]] std::size_t size() const noexcept {
    return m_vector.size();
  }

  /**
   * @brief Returns the total number of elements that can be stored in the currently allocated memory. This is used to
   * determine when the vector needs to grow its storage. The derived Vector class will update this value when it
   * allocates or deallocates memory for the vector's storage, while the IVector simply provides a common storage
   * mechanism for this information.
   *
   * @return The total number of elements that can be stored in the currently allocated memory.
   */
  [[nodiscard]] std::size_t capacity() const noexcept {
    return m_vector.capacity();
  }

  /**
   * @brief Returns the maximum number of elements that the vector can hold, as determined by the allocator's max_size()
   * method.
   *
   * @return The maximum number of elements that the vector can hold.
   */
  [[nodiscard]] std::size_t maxSize() const noexcept {
    return m_vector.maxSize();
  }

  /**
   * @brief Returns whether the vector is empty (i.e., contains no elements).
   *
   * @return True if the vector is empty, false otherwise.
   */
  [[nodiscard]] bool empty() const noexcept {
    return m_vector.empty();
  }

  /**
   * @brief Returns a pointer to the element at the specified index. This method performs bounds checking and will throw
   * a std::out_of_range exception if the index is out of bounds (i.e., greater than or equal to the size of the
   * vector).
   *
   * @param index The index of the element to access.
   * @return A pointer to the element at the specified index.
   */
  [[nodiscard]] void* at(const std::size_t index) {
    if (index >= m_vector.m_size) {
      throw std::out_of_range("Index out of range");
    }
    return getPointer(index);
  }

  /**
   * @brief Returns a const pointer to the element at the specified index. This method performs bounds checking and will
   * throw a std::out_of_range exception if the index is out of bounds (i.e., greater than or equal to the size of the
   * vector).
   *
   * @param index The index of the element to access.
   * @return A const pointer to the element at the specified index.
   */
  [[nodiscard]] const void* at(const std::size_t index) const {
    if (index >= m_vector.m_size) {
      throw std::out_of_range("Index out of range");
    }
    return getPointer(index);
  }

  /**
   * @brief Returns a pointer to the element at the specified index without performing bounds checking. The caller is
   * responsible for ensuring that the index is valid (i.e., less than the size of the vector) before calling this
   * method, as accessing an out-of-bounds index will result in undefined behavior.
   *
   * @param index The index of the element to access.
   * @return A pointer to the element at the specified index.
   */
  [[nodiscard]] void* operator[](const std::size_t index) noexcept {
    return getPointer(index);
  }

  /**
   * @brief Returns a const pointer to the element at the specified index without performing bounds checking. The caller
   * is responsible for ensuring that the index is valid (i.e., less than the size of the vector) before calling this
   * method, as accessing an out-of-bounds index will result in undefined behavior.
   *
   * @param index The index of the element to access.
   * @return A const pointer to the element at the specified index.
   */
  [[nodiscard]] const void* operator[](const std::size_t index) const noexcept {
    return getPointer(index);
  }

  /**
   * @brief Returns a pointer to the first element in the vector. This method performs bounds checking and will throw a
   * std::out_of_range exception if the vector is empty.
   *
   * @return A pointer to the first element in the vector.
   */
  [[nodiscard]] void* front() {
    if (m_vector.empty()) {
      throw std::out_of_range("Vector is empty");
    }
    return m_vector.m_data;
  }

  /**
   * @brief Returns a const pointer to the first element in the vector. This method performs bounds checking and will
   * throw a std::out_of_range exception if the vector is empty.
   *
   * @return A const pointer to the first element in the vector.
   */
  [[nodiscard]] const void* front() const {
    if (m_vector.empty()) {
      throw std::out_of_range("Vector is empty");
    }
    return m_vector.m_data;
  }

  /**
   * @brief Returns a pointer to the last element in the vector. This method performs bounds checking and will throw a
   * std::out_of_range exception if the vector is empty.
   *
   * @return A pointer to the last element in the vector.
   */
  [[nodiscard]] void* back() {
    if (m_vector.empty()) {
      throw std::out_of_range("Vector is empty");
    }
    return getPointer(m_vector.m_size - 1);
  }

  /**
   * @brief Returns a const pointer to the last element in the vector. This method performs bounds checking and will
   * throw a std::out_of_range exception if the vector is empty.
   *
   * @return A const pointer to the last element in the vector.
   */
  [[nodiscard]] const void* back() const {
    if (m_vector.empty()) {
      throw std::out_of_range("Vector is empty");
    }
    return getPointer(m_vector.m_size - 1);
  }

  /**
   * @brief Returns a PolymorphicIterator pointing to the first element in the vector. This method does not perform
   * bounds checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this
   * method, as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return A PolymorphicIterator pointing to the first element in the vector.
   */
  [[nodiscard]] PolymorphicIterator begin() noexcept {
    return PolymorphicIterator(m_vector.m_data, m_vector.m_elementSize);
  }

  /**
   * @brief Returns a const PolymorphicIterator pointing to the first element in the vector. This method does not
   * perform bounds checking, and it is the caller's responsibility to ensure that the vector is not empty before
   * calling this method, as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return A const PolymorphicIterator pointing to the first element in the vector.
   */
  [[nodiscard]] PolymorphicIterator begin() const noexcept {
    return PolymorphicIterator(m_vector.m_data, m_vector.m_elementSize);
  }

  /**
   * @brief Returns a PolymorphicIterator to the first element in the vector. This method does not perform bounds
   * checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this method,
   * as dereferencing the pointer when the vector is empty will result in undefined behavior.
   *
   * @return A const pointer to the first element in the vector.
   */
  [[nodiscard]] PolymorphicIterator cbegin() const noexcept {
    return PolymorphicIterator(m_vector.m_data, m_vector.m_elementSize);
  }

  /**
   * @brief Returns a PolymorphicIterator pointing one past the last element in the vector. This method does not perform
   * bounds checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this
   * method, as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return A PolymorphicIterator pointing one past the last element in the vector.
   */
  [[nodiscard]] PolymorphicIterator end() noexcept {
    return PolymorphicIterator(getPointer(m_vector.m_size), m_vector.m_elementSize);
  }

  /**
   * @brief Returns a const PolymorphicIterator pointing one past the last element in the vector. This method does not
   * perform bounds checking, and it is the caller's responsibility to ensure that the vector is not empty before
   * calling this method, as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return A const PolymorphicIterator pointing one past the last element in the vector.
   */
  [[nodiscard]] PolymorphicIterator end() const noexcept {
    return PolymorphicIterator(getPointer(m_vector.m_size), m_vector.m_elementSize);
  }

  /**
   * @brief Returns a PolymorphicIterator to one past the last element in the vector. This method does not perform
   * bounds checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this
   * method, as dereferencing the pointer when the vector is empty will result in undefined behavior.
   *
   * @return A const pointer to one past the last element in the vector.
   */
  [[nodiscard]] PolymorphicIterator cend() const noexcept {
    return PolymorphicIterator(getPointer(m_vector.m_size), m_vector.m_elementSize);
  }

  /**
   * @brief Returns a reverse PolymorphicIterator pointing to the last element in the vector. This method does not
   * perform bounds checking, and it is the caller's responsibility to ensure that the vector is not empty before
   * calling this method, as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return A reverse PolymorphicIterator pointing to the last element in the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator rbegin() noexcept {
    return PolymorphicReverseIterator(end());
  }

  /**
   * @brief Returns a const reverse PolymorphicIterator pointing to the last element in the vector. This method does not
   * perform bounds checking, and it is the caller's responsibility to ensure that the vector is not empty before
   * calling this method, as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return A const reverse PolymorphicIterator pointing to the last element in the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator rbegin() const noexcept {
    return PolymorphicReverseIterator(end());
  }

  /**
   * @brief Returns a const reverse PolymorphicIterator pointing to the last element in the vector.
   *
   * @return A const reverse PolymorphicIterator pointing to the last element in the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator crbegin() const noexcept {
    return rbegin();
  }

  /**
   * @brief Returns a reverse PolymorphicIterator pointing to one element before the first element in the vector
   * (i.e., the reverse end). This method does not perform bounds checking.
   *
   * @return A reverse PolymorphicIterator pointing to the reverse end of the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator rend() noexcept {
    return PolymorphicReverseIterator(begin());
  }

  /**
   * @brief Returns a const reverse PolymorphicIterator pointing to one element before the first element in the vector.
   *
   * @return A const reverse PolymorphicIterator pointing to the reverse end of the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator rend() const noexcept {
    return PolymorphicReverseIterator(begin());
  }

  /**
   * @brief Returns a const reverse PolymorphicIterator pointing to one element before the first element in the vector.
   *
   * @return A const reverse PolymorphicIterator pointing to the reverse end of the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator crend() const noexcept {
    return rend();
  }

  /**
   * @brief Removes all elements from the vector, leaving it with a size of 0. This method destroys all existing
   * elements in the vector and resets its size to 0, but it does not change the capacity of the vector or deallocate
   * any memory. The vector will still have the same capacity and allocated memory as before, but it will be considered
   * empty and ready to accept new elements. After calling this method, the vector will be in a valid but empty state,
   * and any iterators or references to elements in the vector will be invalidated.
   */
  void clear() noexcept {
    m_vector.clear();
  }

  /**
   * @brief Reserves storage for at least the specified number of elements. If the current capacity is less than the
   * specified capacity, the vector will grow its storage to accommodate at least that many elements. If the current
   * capacity is already greater than or equal to the specified capacity, this method does nothing. After calling this
   * method, the vector's capacity will be at least the specified capacity, and it will be able to hold at least that
   * many elements without needing to grow its storage again. This method does not change the size of the vector or
   * construct any new elements; it only ensures that there is enough allocated memory to hold the specified number of
   * elements.
   *
   * @param capacity The minimum capacity to reserve for the vector's storage. If the current capacity is less than this
   * value, the vector will grow its storage to accommodate at least this many elements. If the current capacity is
   * already greater than or equal to this value, this method does nothing.
   */
  void reserve(const std::size_t capacity) {
    m_vector.reserve(capacity);
  }

  void shrinkToFit() {
    m_vector.shrinkToFit();
  }

  void* pushBack(const void* value) {
    return insert(cend().data(), value);
  }

  void popBack() {
    if (m_vector.empty()) {
      throw std::out_of_range("Vector is empty");
    }

    m_vector.destroyAt(getPointer(m_vector.m_size - 1));
    --m_vector.m_size;
  }

  void* insert(const void* position, const void* value) {
    return insert(position, 1, value);
  }

  void* insert(const PolymorphicIterator position, const void* value) {
    return insert(position.data(), value);
  }

  void* insert(const void* position, const std::size_t count, const void* value) {
    const std::size_t index = indexForPosition(position, true);
    return insertCopiesAt(index, count, value);
  }

  void* insert(const PolymorphicIterator position, const std::size_t count, const void* value) {
    return insert(position.data(), count, value);
  }

  void* erase(const void* position) {
    const std::size_t index = indexForPosition(position, false);
    return erase(position, getPointer(index + 1));
  }

  void* erase(const PolymorphicIterator position) {
    return erase(position.data());
  }

  void* erase(const void* first, const void* last) {
    const std::size_t beginIndex = indexForPosition(first, true);
    const std::size_t endIndex = indexForPosition(last, true);

    if (endIndex < beginIndex) {
      throw std::out_of_range("Vector erase range is invalid");
    }

    if (beginIndex == endIndex) {
      return getPointer(beginIndex);
    }

    if (beginIndex == 0 && endIndex == m_vector.m_size) {
      clear();
      return m_vector.m_data;
    }

    void* oldData = m_vector.m_data;
    void* newData = m_vector.allocateStorage(m_vector.m_capacity);
    const std::size_t newSize = m_vector.m_size - (endIndex - beginIndex);
    std::size_t constructed = 0;

    try {
      for (; constructed < beginIndex; ++constructed) {
        m_vector.constructAt(m_vector.getElementAddress(newData, constructed),
                             m_vector.getElementAddress(oldData, constructed));
      }

      for (std::size_t i = endIndex; i < m_vector.m_size; ++i, ++constructed) {
        m_vector.constructAt(m_vector.getElementAddress(newData, constructed), m_vector.getElementAddress(oldData, i));
      }
    } catch (...) {
      m_vector.destroyRange(newData, 0, constructed);
      m_vector.deallocateStorage(newData, m_vector.m_capacity);
      throw;
    }

    m_vector.destroyRange(oldData, 0, m_vector.m_size);
    m_vector.deallocateStorage();

    m_vector.m_data = newData;
    m_vector.m_size = newSize;
    return getPointer(beginIndex);
  }

  void* erase(const PolymorphicIterator first, const PolymorphicIterator last) {
    return erase(first.data(), last.data());
  }

  void resize(const std::size_t newSize) {
    m_vector.resize(newSize);
  }

  /**
   * @brief Assigns count copies of the given value to the vector, replacing its current contents.
   * This mirrors Vector::assign(count, value).
   *
   * @param count The number of copies to assign.
   * @param value A pointer to the value to copy. Must not be nullptr.
   * @throws std::invalid_argument if value is nullptr.
   */
  void assign(const std::size_t count, const void* value) {
    if (value == nullptr) {
      throw std::invalid_argument("Assigned value pointer is invalid");
    }

    // Clear and rebuild
    m_vector.clear();
    m_vector.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
      m_vector.copyConstructAt(m_vector.getElementAddress(m_vector.m_data, m_vector.m_size), value);
      ++m_vector.m_size;
    }
  }

  /**
   * @brief Assigns elements from a contiguous range [first, last) to the vector, replacing its current contents.
   * The range is treated as raw contiguous memory of elements of type represented by elementSize.
   *
   * @param first A pointer to the first element in the range.
   * @param last A pointer to one past the last element in the range.
   * @throws std::invalid_argument if first or last is nullptr (unless both are nullptr for empty range).
   * @throws std::out_of_range if the range is invalid (last < first).
   */
  void assign(const void* first, const void* last) {
    if ((first == nullptr) != (last == nullptr)) {
      throw std::invalid_argument("Range pointers are invalid");
    }

    if (first == nullptr && last == nullptr) {
      m_vector.clear();
      return;
    }

    const auto* firstBytes = static_cast<const std::byte*>(first);
    const auto* lastBytes = static_cast<const std::byte*>(last);

    if (lastBytes < firstBytes) {
      throw std::out_of_range("Assign range is invalid");
    }

    const std::size_t byteCount = lastBytes - firstBytes;
    if (byteCount % m_vector.m_elementSize != 0) {
      throw std::out_of_range("Range size is not a multiple of element size");
    }

    const std::size_t count = byteCount / m_vector.m_elementSize;

    // Clear and rebuild
    m_vector.clear();
    m_vector.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
      m_vector.copyConstructAt(m_vector.getElementAddress(m_vector.m_data, m_vector.m_size),
                               m_vector.getElementAddress(const_cast<void*>(first), i));
      ++m_vector.m_size;
    }
  }

  /**
   * @brief Inserts elements from a contiguous range [first, last) at the specified position.
   * This mirrors Vector::insert(position, first, last).
   *
   * @param position The position where elements should be inserted (before the element at this address).
   * @param first A pointer to the first element in the range to insert.
   * @param last A pointer to one past the last element in the range.
   * @return A pointer to the first inserted element, or position if the range is empty.
   * @throws std::invalid_argument if first or last is nullptr (unless both are nullptr for empty range).
   * @throws std::out_of_range if position, first, or last is invalid.
   */
  void* insert(const void* position, const void* first, const void* last) {
    if ((first == nullptr) != (last == nullptr)) {
      throw std::invalid_argument("Range pointers are invalid");
    }

    // Handle empty range
    if (first == nullptr && last == nullptr) {
      return const_cast<void*>(position);
    }

    const auto* firstBytes = static_cast<const std::byte*>(first);
    const auto* lastBytes = static_cast<const std::byte*>(last);

    if (lastBytes < firstBytes) {
      throw std::out_of_range("Insert range is invalid");
    }

    const std::size_t byteCount = lastBytes - firstBytes;
    if (byteCount % m_vector.m_elementSize != 0) {
      throw std::out_of_range("Range size is not a multiple of element size");
    }

    const std::size_t count = byteCount / m_vector.m_elementSize;
    if (count == 0) {
      return const_cast<void*>(position);
    }

    const std::size_t index = indexForPosition(position, true);
    return insertRangeAt(index, first, count);
  }

  void* insert(const PolymorphicIterator position, const void* first, const void* last) {
    return insert(position.data(), first, last);
  }

  /**
   * @brief Constructs a new element in place at the end of the vector using a constructor callback.
   * The callback receives a void* pointer to uninitialized memory where it should construct the element.
   *
   * This provides emplace-like functionality for the type-erased PolymorphicVector.
   *
   * @param constructor A callable that takes a void* and constructs an element at that location.
   * @return A pointer to the newly constructed element.
   * @throws std::invalid_argument if constructor is null or throws.
   */
  void* emplaceBack(const std::function<void(void*)>& constructor) {
    if (!constructor) {
      throw std::invalid_argument("Constructor callback is invalid");
    }

    const std::size_t required = m_vector.m_size + 1;
    m_vector.ensureBaseCapacityRequest(required);

    const std::size_t newCapacity =
        required <= m_vector.m_capacity ? m_vector.m_capacity : m_vector.recommendedBaseCapacity(required);
    void* oldData = m_vector.m_data;
    void* newData = m_vector.allocateStorage(newCapacity);

    std::size_t constructed = 0;
    try {
      // Move existing elements
      for (; constructed < m_vector.m_size; ++constructed) {
        m_vector.constructAt(m_vector.getElementAddress(newData, constructed),
                             m_vector.getElementAddress(oldData, constructed));
      }

      // Construct new element
      constructor(m_vector.getElementAddress(newData, constructed));
      ++constructed;
    } catch (...) {
      m_vector.destroyRange(newData, 0, constructed);
      m_vector.deallocateStorage(newData, newCapacity);
      throw;
    }

    m_vector.destroyRange(oldData, 0, m_vector.m_size);
    if (m_vector.m_capacity > 0) {
      m_vector.deallocateStorage();
    }

    m_vector.m_data = newData;
    m_vector.m_size = constructed;
    m_vector.m_capacity = newCapacity;
    return m_vector.getElementAddress(newData, m_vector.m_size - 1);
  }

  /**
   * @brief Constructs a new element in place at the specified position using a constructor callback.
   * The callback receives a void* pointer to uninitialized memory where it should construct the element.
   *
   * This provides emplace-like functionality for the type-erased PolymorphicVector at arbitrary positions.
   *
   * @param position The position where the new element should be constructed (before the element at this address).
   * @param constructor A callable that takes a void* and constructs an element at that location.
   * @return A pointer to the newly constructed element.
   * @throws std::invalid_argument if constructor is null or throws.
   * @throws std::out_of_range if position is invalid.
   */
  void* emplace(const void* position, const std::function<void(void*)>& constructor) {
    if (!constructor) {
      throw std::invalid_argument("Constructor callback is invalid");
    }

    const std::size_t index = indexForPosition(position, true);

    if (index == m_vector.m_size) {
      return emplaceBack(constructor);
    }

    const std::size_t required = m_vector.m_size + 1;
    m_vector.ensureBaseCapacityRequest(required);

    const std::size_t newCapacity =
        required <= m_vector.m_capacity ? m_vector.m_capacity : m_vector.recommendedBaseCapacity(required);
    void* oldData = m_vector.m_data;
    void* newData = m_vector.allocateStorage(newCapacity);

    std::size_t constructed = 0;
    try {
      // Move elements before insertion point
      for (; constructed < index; ++constructed) {
        m_vector.constructAt(m_vector.getElementAddress(newData, constructed),
                             m_vector.getElementAddress(oldData, constructed));
      }

      // Construct new element at insertion point
      constructor(m_vector.getElementAddress(newData, constructed));
      ++constructed;

      // Move elements after insertion point
      for (std::size_t i = index; i < m_vector.m_size; ++i, ++constructed) {
        m_vector.constructAt(m_vector.getElementAddress(newData, constructed), m_vector.getElementAddress(oldData, i));
      }
    } catch (...) {
      m_vector.destroyRange(newData, 0, constructed);
      m_vector.deallocateStorage(newData, newCapacity);
      throw;
    }

    m_vector.destroyRange(oldData, 0, m_vector.m_size);
    if (m_vector.m_capacity > 0) {
      m_vector.deallocateStorage();
    }

    m_vector.m_data = newData;
    m_vector.m_size = constructed;
    m_vector.m_capacity = newCapacity;
    return m_vector.getElementAddress(newData, index);
  }

  void* emplace(const PolymorphicIterator position, const std::function<void(void*)>& constructor) {
    return emplace(position.data(), constructor);
  }

  [[nodiscard]] std::size_t elementSize() const noexcept {
    return m_vector.m_elementSize;
  }

 private:
  [[nodiscard]] std::size_t indexForPosition(const void* position, const bool allowEnd) const {
    const void* beginPtr = m_vector.m_data;
    const void* endPtr = getPointer(m_vector.m_size);

    if (beginPtr == nullptr) {
      if (allowEnd && position == nullptr) {
        return 0;
      }
      throw std::out_of_range("Vector iterator position is invalid");
    }

    const auto* beginBytes = static_cast<const std::byte*>(beginPtr);
    const auto* endBytes = static_cast<const std::byte*>(endPtr);
    const auto* currentBytes = static_cast<const std::byte*>(position);

    if (currentBytes < beginBytes || currentBytes > endBytes || (!allowEnd && currentBytes == endBytes)) {
      throw std::out_of_range("Vector iterator position is invalid");
    }

    const std::ptrdiff_t byteOffset = currentBytes - beginBytes;
    if (byteOffset % static_cast<std::ptrdiff_t>(m_vector.m_elementSize) != 0) {
      throw std::out_of_range("Vector iterator position is invalid");
    }

    return static_cast<std::size_t>(byteOffset / static_cast<std::ptrdiff_t>(m_vector.m_elementSize));
  }

  [[nodiscard]] void* insertCopiesAt(const std::size_t index, const std::size_t count, const void* value) {
    if (index > m_vector.m_size) {
      throw std::out_of_range("Vector iterator position is invalid");
    }

    if (count == 0) {
      return getPointer(index);
    }

    if (value == nullptr) {
      throw std::invalid_argument("Inserted value pointer is invalid");
    }

    const std::size_t required = m_vector.m_size + count;
    m_vector.ensureBaseCapacityRequest(required);

    const std::size_t newCapacity =
        required <= m_vector.m_capacity ? m_vector.m_capacity : m_vector.recommendedBaseCapacity(required);
    void* oldData = m_vector.m_data;
    void* newData = m_vector.allocateStorage(newCapacity);
    std::size_t constructed = 0;

    try {
      for (; constructed < index; ++constructed) {
        m_vector.constructAt(m_vector.getElementAddress(newData, constructed),
                             m_vector.getElementAddress(oldData, constructed));
      }

      for (std::size_t inserted = 0; inserted < count; ++inserted, ++constructed) {
        m_vector.copyConstructAt(m_vector.getElementAddress(newData, constructed), value);
      }

      for (std::size_t i = index; i < m_vector.m_size; ++i, ++constructed) {
        m_vector.constructAt(m_vector.getElementAddress(newData, constructed), m_vector.getElementAddress(oldData, i));
      }
    } catch (...) {
      m_vector.destroyRange(newData, 0, constructed);
      m_vector.deallocateStorage(newData, newCapacity);
      throw;
    }

    m_vector.destroyRange(oldData, 0, m_vector.m_size);
    if (m_vector.m_capacity > 0) {
      m_vector.deallocateStorage();
    }

    m_vector.m_data = newData;
    m_vector.m_size = constructed;
    m_vector.m_capacity = newCapacity;
    return getPointer(index);
  }

  [[nodiscard]] void* getPointer(const std::size_t index) const noexcept {
    if (m_vector.m_data == nullptr) {
      return nullptr;
    }
    return static_cast<std::byte*>(m_vector.m_data) + index * m_vector.m_elementSize;
  }

  [[nodiscard]] void* insertRangeAt(const std::size_t index, const void* first, const std::size_t count) {
    if (index > m_vector.m_size) {
      throw std::out_of_range("Vector iterator position is invalid");
    }

    if (count == 0) {
      return getPointer(index);
    }

    if (first == nullptr) {
      throw std::invalid_argument("Inserted range starting pointer is invalid");
    }

    const std::size_t required = m_vector.m_size + count;
    m_vector.ensureBaseCapacityRequest(required);

    const std::size_t newCapacity =
        required <= m_vector.m_capacity ? m_vector.m_capacity : m_vector.recommendedBaseCapacity(required);
    void* oldData = m_vector.m_data;
    void* newData = m_vector.allocateStorage(newCapacity);

    std::size_t constructed = 0;
    try {
      // Move elements before insertion point
      for (; constructed < index; ++constructed) {
        m_vector.constructAt(m_vector.getElementAddress(newData, constructed),
                             m_vector.getElementAddress(oldData, constructed));
      }

      // Copy elements from the source range
      for (std::size_t srcIdx = 0; srcIdx < count; ++srcIdx, ++constructed) {
        m_vector.copyConstructAt(m_vector.getElementAddress(newData, constructed),
                                 m_vector.getElementAddress(const_cast<void*>(first), srcIdx));
      }

      // Move elements after insertion point
      for (std::size_t i = index; i < m_vector.m_size; ++i, ++constructed) {
        m_vector.constructAt(m_vector.getElementAddress(newData, constructed), m_vector.getElementAddress(oldData, i));
      }
    } catch (...) {
      m_vector.destroyRange(newData, 0, constructed);
      m_vector.deallocateStorage(newData, newCapacity);
      throw;
    }

    m_vector.destroyRange(oldData, 0, m_vector.m_size);
    if (m_vector.m_capacity > 0) {
      m_vector.deallocateStorage();
    }

    m_vector.m_data = newData;
    m_vector.m_size = constructed;
    m_vector.m_capacity = newCapacity;
    return getPointer(index);
  }

  IVector& m_vector;
};
}  // namespace core::container
