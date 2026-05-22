#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>

#include "IVector.hpp"
#include "RawIterator.hpp"

namespace core::container {
/**
 *
 */
class RawVector : public IVector {
 public:
  using traits = std::allocator_traits<std::pmr::polymorphic_allocator<>>;

  /**
   * @brief Constructor that initializes the RawVector with the specified element size and alignment.
   *
   * @param elementSize The size of each element in bytes.
   * @param alignment The alignment requirement for the elements. Defaults to the maximum alignment supported by the
   * platform.
   * @param allocator The polymorphic allocator to use for memory management. Defaults to the default memory resource
   * provided by the standard library.
   */
  explicit RawVector(const std::size_t elementSize, const std::size_t alignment = alignof(std::max_align_t),
                     const std::pmr::polymorphic_allocator<> allocator = std::pmr::get_default_resource()) noexcept
      : IVector(elementSize, alignment), m_allocator(allocator) {}

  /**
   * @brief Constructor that initializes the RawVector by copying the data and metadata from another IVector.
   *
   * @param other The IVector to copy from.
   */
  explicit RawVector(const IVector& other) noexcept
      : RawVector(other, traits::select_on_container_copy_construction(other.getByteAllocator())) {}

  /**
   * @brief Constructor that initializes the RawVector by copying the data and metadata from another IVector, using the
   * specified allocator for memory management.
   *
   * @param other The IVector to copy from.
   * @param allocator The polymorphic allocator to use for memory management in the new RawVector.
   */
  explicit RawVector(const IVector& other, const std::pmr::polymorphic_allocator<> allocator) noexcept
      : IVector(other.m_data, other.m_size, other.m_capacity, other.m_elementSize, other.m_alignment),
        m_allocator(allocator) {}

  /**
   * @brief Returns the maximum number of elements that the vector can hold, as determined by the allocator's max_size()
   * method.
   *
   * @return The maximum number of elements that the vector can hold.
   */
  [[nodiscard]] std::size_t maxSize() const noexcept override {
    return std::numeric_limits<std::size_t>::max() / m_elementSize;
  }

  /**
   * @brief Returns a pointer to the raw data buffer that holds the vector's elements. This method allows access to the
   * raw memory where the elements are stored, and it can be used by the caller to read or manipulate the elements
   * directly.
   *
   * @return A pointer to the raw data buffer that holds the vector's elements.
   */
  [[nodiscard]] void* data() noexcept {
    return m_data;
  }

  /**
   * @brief Returns a const pointer to the raw data buffer that holds the vector's elements. This method allows
   * read-only access to the raw memory where the elements are stored, and it can be used by the caller to read the
   * elements without modifying them.
   *
   * @return A const pointer to the raw data buffer that holds the vector's elements.
   */
  [[nodiscard]] const void* data() const noexcept {
    return m_data;
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
    if (index >= m_size) {
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
    if (index >= m_size) {
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
    if (empty()) {
      throw std::out_of_range("Vector is empty");
    }
    return m_data;
  }

  /**
   * @brief Returns a const pointer to the first element in the vector. This method performs bounds checking and will
   * throw a std::out_of_range exception if the vector is empty.
   *
   * @return A const pointer to the first element in the vector.
   */
  [[nodiscard]] const void* front() const {
    if (empty()) {
      throw std::out_of_range("Vector is empty");
    }
    return m_data;
  }

  /**
   * @brief Returns a pointer to the last element in the vector. This method performs bounds checking and will throw a
   * std::out_of_range exception if the vector is empty.
   *
   * @return A pointer to the last element in the vector.
   */
  [[nodiscard]] void* back() {
    if (empty()) {
      throw std::out_of_range("Vector is empty");
    }
    return getPointer(m_size - 1);
  }

  /**
   * @brief Returns a const pointer to the last element in the vector. This method performs bounds checking and will
   * throw a std::out_of_range exception if the vector is empty.
   *
   * @return A const pointer to the last element in the vector.
   */
  [[nodiscard]] const void* back() const {
    if (empty()) {
      throw std::out_of_range("Vector is empty");
    }
    return getPointer(m_size - 1);
  }

  /**
   * @brief Returns an iterator pointing to the first element in the vector. This method does not perform bounds
   * checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this method,
   * as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return An iterator pointing to the first element in the vector.
   */
  [[nodiscard]] RawIterator begin() noexcept {
    return RawIterator(m_data, m_elementSize);
  }

  /**
   * @brief Returns a const iterator pointing to the first element in the vector. This method does not perform bounds
   * checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this method,
   * as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return A const iterator pointing to the first element in the vector.
   */
  [[nodiscard]] RawIterator begin() const noexcept {
    return RawIterator(m_data, m_elementSize);
  }

  /**
   * @brief Returns an iterator to the first element in the vector. This method does not perform bounds checking, and it
   * is the caller's responsibility to ensure that the vector is not empty before calling this method, as dereferencing
   * the pointer when the vector is empty will result in undefined behavior.
   *
   * @return An iterator to the first element in the vector.
   */
  [[nodiscard]] RawIterator cbegin() const noexcept {
    return RawIterator(m_data, m_elementSize);
  }

  /**
   * @brief Returns an interator pointing one past the last element in the vector. This method does not perform bounds
   * checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this method,
   * as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return An interator pointing one past the last element in the vector.
   */
  [[nodiscard]] RawIterator end() noexcept {
    return RawIterator(getPointer(m_size), m_elementSize);
  }

  /**
   * @brief Returns a const iterator pointing one past the last element in the vector. This method does not perform
   * bounds checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this
   * method, as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return A const iterator pointing one past the last element in the vector.
   */
  [[nodiscard]] RawIterator end() const noexcept {
    return RawIterator(getPointer(m_size), m_elementSize);
  }

  /**
   * @brief Returns an interator to one past the last element in the vector. This method does not perform
   * bounds checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this
   * method, as dereferencing the pointer when the vector is empty will result in undefined behavior.
   *
   * @return An iterator to one past the last element in the vector.
   */
  [[nodiscard]] RawIterator cend() const noexcept {
    return RawIterator(getPointer(m_size), m_elementSize);
  }

  /**
   * @brief Returns a reverse iterator pointing to the last element in the vector. This method does not perform bounds
   * checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this method,
   * as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return A reverse iterator pointing to the last element in the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator rbegin() noexcept {
    return PolymorphicReverseIterator(end());
  }

  /**
   * @brief Returns a const reverse iterator pointing to the last element in the vector. This method does not perform
   * bounds checking, and it is the caller's responsibility to ensure that the vector is not empty before calling this
   * method, as dereferencing the iterator when the vector is empty will result in undefined behavior.
   *
   * @return A const reverse iterator pointing to the last element in the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator rbegin() const noexcept {
    return PolymorphicReverseIterator(end());
  }

  /**
   * @brief Returns a const reverse iterator pointing to the last element in the vector.
   *
   * @return A const reverse iterator pointing to the last element in the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator crbegin() const noexcept {
    return rbegin();
  }

  /**
   * @brief Returns a reverse iterator pointing to one element before the first element in the vector
   * (i.e., the reverse end). This method does not perform bounds checking.
   *
   * @return A reverse iterator pointing to the reverse end of the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator rend() noexcept {
    return PolymorphicReverseIterator(begin());
  }

  /**
   * @brief Returns a const reverse iterator pointing to one element before the first element in the vector.
   *
   * @return A const reverse iterator pointing to the reverse end of the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator rend() const noexcept {
    return PolymorphicReverseIterator(begin());
  }

  /**
   * @brief Returns a const reverse iterator pointing to one element before the first element in the vector.
   *
   * @return A const reverse iterator pointing to the reverse end of the vector.
   */
  [[nodiscard]] PolymorphicReverseIterator crend() const noexcept {
    return rend();
  }

  /**
   * @brief Appends a copy of the given value to the end of the vector. This method mirrors Vector::push_back(const T&
   * value) but operates on raw pointers. The method will handle the necessary memory management and construction of the
   * new element in the vector. After calling this method, the new element will be added to the end of the vector, and
   * the size of the vector will be increased by one.
   *
   * @param value A pointer to the value to be copied and added to the end of the vector. Must not be nullptr.
   * @return A pointer to the newly added element in the vector.
   * @throws std::invalid_argument if position or value is nullptr, or if position does not point to a valid position
   * within the vector.
   * @throws std::out_of_range if position is outside the valid range of the vector (i.e., before the beginning or after
   * the end of the vector).
   * @throws std::length_error if the resulting size of the vector after insertion would exceed the maximum size allowed
   * by the allocator.
   *
   */
  void* pushBack(const void* value) {
    return insert(cend().data(), value);
  }

  /**
   * @brief Inserts a copy of the given value at the specified position in the vector. This method mirrors
   * Vector::insert(const_iterator position, const T& value) but operates on raw pointers. The method will handle the
   * necessary memory management and construction of the new element in the vector. After calling this method, the new
   * element will be inserted at the specified position in the vector, and the size of the vector will be increased by
   * one. The method returns a pointer to the newly inserted element in the vector.
   *
   * @param position A pointer to the location in the vector where the new element should be inserted. Must not be
   * nullptr and should point to a valid position within the vector (i.e., between the beginning and end of the vector).
   * @param value A pointer to the value to be copied and inserted at the specified position in the vector. Must not be
   * nullptr.
   * @return A pointer to the newly inserted element in the vector.
   * @throws std::invalid_argument if position or value is nullptr, or if position does not point to a valid position
   * within the vector.
   * @throws std::out_of_range if position is outside the valid range of the vector (i.e., before the beginning or after
   * the end of the vector).
   * @throws std::length_error if the resulting size of the vector after insertion would exceed the maximum size allowed
   * by the allocator.
   */
  void* insert(const void* position, const void* value) {
    return insert(position, 1, value);
  }

  /**
   * @brief Inserts count copies of the given value at the specified position in the vector. This method mirrors
   * Vector::insert(const_iterator position, size_type count, const T& value) but operates on raw pointers. The method
   * will handle the necessary memory management and construction of the new elements in the vector. After calling this
   * method, the new elements will be inserted at the specified position in the vector, and the size of the vector will
   * be increased by the count of new elements. The method returns a pointer to the first of the newly inserted elements
   * in the vector.
   *
   * @param position A pointer to the location in the vector where the new elements should be inserted. Must not be
   * nullptr and should point to a valid position within the vector (i.e., between the beginning and end of the vector).
   * @param value A pointer to the value to be copied and inserted at the specified position in the vector. Must not be
   * nullptr.
   * @return A pointer to the first of the newly inserted elements in the vector.
   * @throws std::invalid_argument if position or value is nullptr, or if position does not point to a valid position
   * within the vector.
   * @throws std::out_of_range if position is outside the valid range of the vector (i.e., before the beginning or after
   * the end of the vector).
   * @throws std::length_error if the resulting size of the vector after insertion would exceed the maximum size allowed
   * by the allocator.
   */
  void* insert(const RawIterator position, const void* value) {
    return insert(position.data(), value);
  }

  /**
   * @brief Inserts count copies of the given value at the specified position in the vector. This method mirrors
   * Vector::insert(const_iterator position, size_type count, const T& value) but operates on raw pointers. The method
   * will handle the necessary memory management and construction of the new elements in the vector. After calling this
   * method, the new elements will be inserted at the specified position in the vector, and the size of the vector will
   * be increased by the count of new elements. The method returns a pointer to the first of the newly inserted elements
   * in the vector.
   *
   * @param position A pointer to the location in the vector where the new elements should be inserted. Must not be
   * nullptr and should point to a valid position within the vector (i.e., between the beginning and end of the vector).
   * @param count The number of copies of the value to be inserted at the specified position in the vector. This value
   * should be greater than zero.
   * @param value A pointer to the value to be copied and inserted at the specified position in the vector. Must not be
   * nullptr.
   * @return A pointer to the first of the newly inserted elements in the vector.
   * @throws std::invalid_argument if position or value is nullptr, if position does not point to a valid position
   * within the vector, or if count is zero.
   * @throws std::out_of_range if position is outside the valid range of the vector (i.e., before the beginning or after
   * the end of the vector).
   * @throws std::length_error if the resulting size of the vector after insertion would exceed the maximum size allowed
   * by the allocator.
   */
  void* insert(const void* position, const std::size_t count, const void* value) {
    const std::size_t index = getIndex(position, true);
    return insertCopiesAt(index, count, value);
  }

  /**
   * @brief Inserts count copies of the given value at the specified position in the vector. This method mirrors
   * Vector::insert(const_iterator position, size_type count, const T& value) but operates on raw pointers. The method
   * will handle the necessary memory management and construction of the new elements in the vector. After calling this
   * method, the new elements will be inserted at the specified position in the vector, and the size of the vector will
   * be increased by the count of new elements. The method returns a pointer to the first of the newly inserted elements
   * in the vector.
   *
   * @param position A pointer to the location in the vector where the new elements should be inserted. Must not be
   * nullptr and should point to a valid position within the vector (i.e., between the beginning and end of the vector).
   * @param count The number of copies of the value to be inserted at the specified position in the vector. This value
   * should be greater than zero.
   * @param value A pointer to the value to be copied and inserted at the specified position in the vector. Must not be
   * nullptr.
   * @return A pointer to the first of the newly inserted elements in the vector.
   * @throws std::invalid_argument if position or value is nullptr, if position does not point to a valid position
   * within the vector, or if count is zero.
   * @throws std::out_of_range if position is outside the valid range of the vector (i.e., before the beginning or after
   * the end of the vector).
   * @throws std::length_error if the resulting size of the vector after insertion would exceed the maximum size allowed
   * by the allocator.
   */
  void* insert(const RawIterator position, const std::size_t count, const void* value) {
    return insert(position.data(), count, value);
  }

  /**
   * @brief Removes the element at the specified position from the vector. This method mirrors
   * Vector::erase(const_iterator position) but operates on raw pointers. The method will handle the necessary memory
   * management and destruction of the removed element in the vector. After calling this method, the element at the
   * specified position will be removed from the vector, and the size of the vector will be decreased by one. The method
   * returns a pointer to the element that followed the removed element in the vector (i.e., the element that is now at
   * the position of the removed element). If the removed element was the last element in the vector, the method returns
   * a pointer to the end of the vector (i.e., one past the last element).
   *
   * @param position A pointer to the location in the vector of the element to be removed. Must not be nullptr and
   * should point to a valid position within the vector (i.e., between the beginning and end of the vector).
   * @return A pointer to the element that followed the removed element in the vector, or a pointer to the end of the
   * vector if the removed element was the last element.
   * @throws std::invalid_argument if position is nullptr or does not point to a valid position within the vector.
   * @throws std::out_of_range if position is outside the valid range of the vector (i.e., before the beginning or after
   * the end of the vector).
   */
  void* erase(const void* position) {
    const std::size_t index = getIndex(position, false);
    return erase(position, getPointer(index + 1));
  }

  /**
   * @brief Removes the element at the specified position from the vector. This method mirrors
   * Vector::erase(const_iterator position) but operates on raw pointers. The method will handle the necessary memory
   * management and destruction of the removed element in the vector. After calling this method, the element at the
   * specified position will be removed from the vector, and the size of the vector will be decreased by one. The method
   * returns a pointer to the element that followed the removed element in the vector (i.e., the element that is now at
   * the position of the removed element). If the removed element was the last element in the vector, the method returns
   * a pointer to the end of the vector (i.e., one past the last element).
   *
   * @param position A pointer to the location in the vector of the element to be removed. Must not be nullptr and
   * should point to a valid position within the vector (i.e., between the beginning and end of the vector).
   * @return A pointer to the element that followed the removed element in the vector, or a pointer to the end of the
   * vector if the removed element was the last element.
   * @throws std::invalid_argument if position is nullptr or does not point to a valid position within the vector.
   * @throws std::out_of_range if position is outside the valid range of the vector (i.e., before the beginning or after
   * the end of the vector).
   */
  void* erase(const RawIterator position) {
    return erase(position.data());
  }

  /**
   * @brief Removes a range of elements from the vector, defined by the half-open interval [first, last). This method
   * mirrors Vector::erase(const_iterator first, const_iterator last) but operates on raw pointers. The method will
   * handle the necessary memory management and destruction of the removed elements in the vector. After calling this
   * method, the elements in the specified range will be removed from the vector, and the size of the vector will be
   * decreased by the number of removed elements. The method returns a pointer to the element that followed the last
   * removed element in the vector (i.e., the element that is now at the position of the last removed element). If the
   * last removed element was the last element in the vector, the method returns a pointer to the end of the vector
   * (i.e., one past the last element).
   *
   * @param first A pointer to the location in the vector of the first element in the range to be removed. Must not be
   * nullptr and should point to a valid position within the vector (i.e., between the beginning and end of the vector).
   * @param last A pointer to the location in the vector one past the last element in the range to be removed. Must not
   * be nullptr and should point to a valid position within the vector (i.e., between the beginning and end of the
   * vector).
   * @return A pointer to the element that followed the last removed element in the vector, or a pointer to the end of
   * the vector if the last removed element was the last element.
   * @throws std::invalid_argument if first or last is nullptr, if first or last does not point to a valid position
   * within the vector, or if the range defined by [first, last) is invalid (i.e., last points to a position before
   * first).
   * @throws std::out_of_range if first or last is outside the valid range of the vector (i.e., before the beginning or
   * after the end of the vector).
   */
  void* erase(const void* first, const void* last) {
    const std::size_t beginIndex = getIndex(first, true);
    const std::size_t endIndex = getIndex(last, true);

    if (endIndex < beginIndex) {
      throw std::out_of_range("Vector erase range is invalid");
    }

    if (beginIndex == endIndex) {
      return getPointer(beginIndex);
    }

    if (beginIndex == 0 && endIndex == m_size) {
      clear();
      return m_data;
    }

    void* oldData = m_data;
    void* newData = allocateStorage(m_capacity);
    const std::size_t newSize = m_size - (endIndex - beginIndex);
    std::size_t constructed = 0;

    try {
      for (; constructed < beginIndex; ++constructed) {
        constructAt(getElementAddress(newData, constructed), getElementAddress(oldData, constructed));
      }

      for (std::size_t i = endIndex; i < m_size; ++i, ++constructed) {
        constructAt(getElementAddress(newData, constructed), getElementAddress(oldData, i));
      }
    } catch (...) {
      destroyRange(newData, 0, constructed);
      deallocateStorage(newData, m_capacity);
      throw;
    }

    destroyRange(oldData, 0, m_size);
    IVector::deallocateStorage();

    m_data = newData;
    m_size = newSize;
    return getPointer(beginIndex);
  }

  /**
   * @brief Removes a range of elements from the vector, defined by the half-open interval [first, last). This method
   * mirrors Vector::erase(const_iterator first, const_iterator last) but operates on raw pointers. The method will
   * handle the necessary memory management and destruction of the removed elements in the vector. After calling this
   * method, the elements in the specified range will be removed from the vector, and the size of the vector will be
   * decreased by the number of removed elements. The method returns a pointer to the element that followed the last
   * removed element in the vector (i.e., the element that is now at the position of the last removed element). If the
   * last removed element was the last element in the vector, the method returns a pointer to the end of the vector
   * (i.e., one past the last element).
   *
   * @param first A pointer to the location in the vector of the first element in the range to be removed. Must not be
   * nullptr and should point to a valid position within the vector (i.e., between the beginning and end of the vector).
   * @param last A pointer to the location in the vector one past the last element in the range to be removed. Must not
   * be nullptr and should point to a valid position within the vector (i.e., between the beginning and end of the
   * vector).
   * @throws std::invalid_argument if first or last is nullptr, if first or last does not point to a valid position
   * within the vector, or if the range defined by [first, last) is invalid (i.e., last points to a position before
   * first).
   * @throws std::out_of_range if first or last is outside the valid range of the vector (i.e., before the beginning or
   * after the end of the vector).
   */
  void* erase(const RawIterator first, const RawIterator last) {
    return erase(first.data(), last.data());
  }

  /**
   * @brief Assigns count copies of the given value to the vector, replacing its current contents. This method mirrors
   * Vector::assign(size_type count, const T& value) but operates on raw pointers. The method will handle the necessary
   * memory management and construction of the new elements in the vector. After calling this method, the vector will
   * contain count copies of the given value, and the size of the vector will be set to count.
   *
   * @param count The number of copies to assign.
   * @param value A pointer to the value to copy. Must not be nullptr.
   * @throws std::invalid_argument if value is nullptr.
   * @throws std::length_error if count is greater than the maximum size allowed by the allocator.
   */
  void assign(const std::size_t count, const void* value) {
    if (value == nullptr) {
      throw std::invalid_argument("Assigned value pointer is invalid");
    }

    // Clear and rebuild
    clear();
    reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
      copyConstructAt(getElementAddress(m_data, m_size), value);
      ++m_size;
    }
  }

  /**
   * @brief Assigns elements from a contiguous range [first, last) to the vector, replacing its current contents. This
   * method mirrors Vector::assign(InputIt first, InputIt last) but operates on raw pointers. The method will handle the
   * necessary memory management and construction of the new elements in the vector. After calling this method, the
   * vector will contain copies of the elements in the specified range, and the size of the vector will be set to the
   * number of elements in the range.
   *
   * @param first A pointer to the first element in the range.
   * @param last A pointer to one past the last element in the range.
   * @throws std::invalid_argument if first or last is nullptr (unless both are nullptr for empty range).
   * @throws std::out_of_range if the range is invalid (last < first).
   */
  void assign(const void* first, const void* last) {
    if (first == nullptr && last == nullptr) {
      clear();
      return;
    }

    if (first == nullptr || last == nullptr) {
      throw std::invalid_argument("Range pointers are invalid");
    }

    const auto* firstBytes = static_cast<const std::byte*>(first);
    const auto* lastBytes = static_cast<const std::byte*>(last);

    if (lastBytes < firstBytes) {
      throw std::out_of_range("Assign range is invalid");
    }

    const std::size_t byteCount = lastBytes - firstBytes;
    if (byteCount % m_elementSize != 0) {
      throw std::out_of_range("Range size is not a multiple of element size");
    }

    const std::size_t count = byteCount / m_elementSize;

    // Clear and rebuild
    clear();
    reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
      copyConstructAt(getElementAddress(m_data, m_size), getElementAddress(const_cast<void*>(first), i));
      ++m_size;
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
    if (first == nullptr && last == nullptr) {
      return const_cast<void*>(position);
    }

    if (first == nullptr || last == nullptr) {
      throw std::invalid_argument("Range pointers are invalid");
    }

    const auto* firstBytes = static_cast<const std::byte*>(first);
    const auto* lastBytes = static_cast<const std::byte*>(last);

    if (lastBytes < firstBytes) {
      throw std::out_of_range("Insert range is invalid");
    }

    const std::size_t byteCount = lastBytes - firstBytes;
    if (byteCount % m_elementSize != 0) {
      throw std::out_of_range("Range size is not a multiple of element size");
    }

    const std::size_t count = byteCount / m_elementSize;
    if (count == 0) {
      return const_cast<void*>(position);
    }

    const std::size_t index = getIndex(position, true);
    return insertRangeAt(index, first, count);
  }

  /**
   * @brief Inserts elements from a contiguous range [first, last) at the specified position. This mirrors
   * Vector::insert(position, first, last).
   *
   * @param position The position where elements should be inserted (before the element at this address).
   * @param first A pointer to the first element in the range to insert.
   * @param last A pointer to one past the last element in the range.
   * @return A pointer to the first inserted element, or position if the range is empty.
   * @throws std::invalid_argument if first or last is nullptr (unless both are nullptr for empty range).
   * @throws std::out_of_range if position, first, or last is invalid.
   * @throws std::length_error if the resulting size of the vector after insertion would exceed the maximum size allowed
   * by the allocator.
   */
  void* insert(const RawIterator position, const void* first, const void* last) {
    return insert(position.data(), first, last);
  }

  /**
   * @brief Constructs a new element in place at the end of the vector using a constructor callback. The callback
   * receives a void* pointer to uninitialized memory where it should construct the element.
   *
   * @param constructor A callable that takes a void* and constructs an element at that location.
   * @return A pointer to the newly constructed element.
   * @throws std::invalid_argument if constructor is null or throws.
   * @throws std::length_error if the resulting size of the vector after insertion would exceed the maximum size allowed
   * by the allocator.
   */
  void* emplaceBack(const std::function<void(void*)>& constructor) {
    if (!constructor) {
      throw std::invalid_argument("Constructor callback is invalid");
    }

    const std::size_t required = m_size + 1;
    ensureBaseCapacityRequest(required);

    const std::size_t newCapacity = required <= m_capacity ? m_capacity : recommendedBaseCapacity(required);
    void* oldData = m_data;
    void* newData = allocateStorage(newCapacity);

    std::size_t constructed = 0;
    try {
      // Move existing elements
      for (; constructed < m_size; ++constructed) {
        constructAt(getElementAddress(newData, constructed), getElementAddress(oldData, constructed));
      }

      // Construct new element
      constructor(getElementAddress(newData, constructed));
      ++constructed;
    } catch (...) {
      destroyRange(newData, 0, constructed);
      deallocateStorage(newData, newCapacity);
      throw;
    }

    destroyRange(oldData, 0, m_size);
    if (m_capacity > 0) {
      IVector::deallocateStorage();
    }

    m_data = newData;
    m_size = constructed;
    m_capacity = newCapacity;
    return getElementAddress(newData, m_size - 1);
  }

  /**
   * @brief Constructs a new element in place at the specified position using a constructor callback.
   * The callback receives a void* pointer to uninitialized memory where it should construct the element.
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

    const std::size_t index = getIndex(position, true);

    if (index == m_size) {
      return emplaceBack(constructor);
    }

    const std::size_t required = m_size + 1;
    ensureBaseCapacityRequest(required);

    const std::size_t newCapacity = required <= m_capacity ? m_capacity : recommendedBaseCapacity(required);
    void* oldData = m_data;
    void* newData = allocateStorage(newCapacity);

    std::size_t constructed = 0;
    try {
      // Move elements before insertion point
      for (; constructed < index; ++constructed) {
        constructAt(getElementAddress(newData, constructed), getElementAddress(oldData, constructed));
      }

      // Construct new element at insertion point
      constructor(getElementAddress(newData, constructed));
      ++constructed;

      // Move elements after insertion point
      for (std::size_t i = index; i < m_size; ++i, ++constructed) {
        constructAt(getElementAddress(newData, constructed), getElementAddress(oldData, i));
      }
    } catch (...) {
      destroyRange(newData, 0, constructed);
      deallocateStorage(newData, newCapacity);
      throw;
    }

    destroyRange(oldData, 0, m_size);
    if (m_capacity > 0) {
      IVector::deallocateStorage();
    }

    m_data = newData;
    m_size = constructed;
    m_capacity = newCapacity;
    return getElementAddress(newData, index);
  }

  /**
   * @brief Constructs a new element in place at the specified position using a constructor callback. The callback
   * receives a void* pointer to uninitialized memory where it should construct the element. This method mirrors
   * emplace(const void* position, const std::function<void(void*)>& constructor) but takes a RawIterator for the
   * position instead of a void* pointer.
   *
   * @param position The position where the new element should be constructed (before the element at this address).
   * @param constructor A callable that takes a void* and constructs an element at that location.
   * @return A pointer to the newly constructed element.
   * @throws std::invalid_argument if constructor is null or throws.
   * @throws std::out_of_range if position is invalid.
   */
  void* emplace(const RawIterator position, const std::function<void(void*)>& constructor) {
    return emplace(position.data(), constructor);
  }

 protected:
  /**
   * @brief Inserts count copies of the given value at the specified index in the vector. This method assumes that the
   * index is valid (i.e., between 0 and the current size of the vector) and that the value pointer is not nullptr. The
   * method will handle the necessary memory management and construction of the new elements in the vector. After
   * calling this method, the new elements will be inserted at the specified index in the vector, and the size of the
   * vector will be increased by the count of new elements. The method returns a pointer to the first of the newly
   * inserted elements in the vector.
   *
   * @param index The index at which to insert the new elements. Must be between 0 and the current size of the vector
   * (inclusive).
   * @param count The number of copies of the value to be inserted at the specified index in the vector. This value
   * should be greater than zero.
   * @param value A pointer to the value to be copied and inserted at the specified index in the vector. Must not be
   * nullptr.
   * @return A pointer to the first of the newly inserted elements in the vector.
   * @throws std::invalid_argument if value is nullptr.
   * @throws std::out_of_range if index is greater than the current size of the vector.
   * @throws std::length_error if the resulting size of the vector after insertion would exceed the maximum size allowed
   * by the allocator.
   */
  [[nodiscard]] void* insertCopiesAt(const std::size_t index, const std::size_t count, const void* value) {
    if (index > m_size) {
      throw std::out_of_range("Vector iterator position is invalid");
    }

    if (count == 0) {
      return getPointer(index);
    }

    if (value == nullptr) {
      throw std::invalid_argument("Inserted value pointer is invalid");
    }

    const std::size_t required = m_size + count;
    ensureBaseCapacityRequest(required);

    const std::size_t newCapacity = required <= m_capacity ? m_capacity : recommendedBaseCapacity(required);
    void* oldData = m_data;
    void* newData = allocateStorage(newCapacity);
    std::size_t constructed = 0;

    try {
      for (; constructed < index; ++constructed) {
        constructAt(getElementAddress(newData, constructed), getElementAddress(oldData, constructed));
      }

      for (std::size_t inserted = 0; inserted < count; ++inserted, ++constructed) {
        copyConstructAt(getElementAddress(newData, constructed), value);
      }

      for (std::size_t i = index; i < m_size; ++i, ++constructed) {
        constructAt(getElementAddress(newData, constructed), getElementAddress(oldData, i));
      }
    } catch (...) {
      destroyRange(newData, 0, constructed);
      deallocateStorage(newData, newCapacity);
      throw;
    }

    destroyRange(oldData, 0, m_size);
    if (m_capacity > 0) {
      IVector::deallocateStorage();
    }

    m_data = newData;
    m_size = constructed;
    m_capacity = newCapacity;
    return getPointer(index);
  }

  /**
   * @brief Calculates the index in the vector corresponding to the given raw pointer position. This method checks if
   * the pointer is valid (i.e., it points to a location within the vector's storage) and returns the corresponding
   * index. If allowEnd is true, the pointer can also point to one past the last element (i.e., the end of the vector),
   * in which case the method will return the index equal to the current size of the vector.
   *
   * @param pointer A raw pointer to a position in the vector's storage. Must not be nullptr.
   * @param allowEnd If true, the position can be nullptr to indicate the end of an empty vector, or it can point to one
   * past the last element. If false, the position must point to a valid element within the vector.
   * @return The index corresponding to the given position in the vector.
   * @throws std::invalid_argument if position is nullptr (unless allowEnd is true and position is nullptr) or if
   * position does not point to a valid location within the vector.
   */
  [[nodiscard]] std::size_t getIndex(const void* pointer, const bool allowEnd) const {
    const void* beginPtr = m_data;
    const void* endPtr = getPointer(m_size);

    if (beginPtr == nullptr) {
      if (pointer == nullptr && allowEnd) {
        return 0;
      }
      throw std::out_of_range("Vector iterator position is invalid");
    }

    if (pointer == nullptr) {
      throw std::out_of_range("Pointer is invalid");
    }

    const auto* beginBytes = static_cast<const std::byte*>(beginPtr);
    const auto* endBytes = static_cast<const std::byte*>(endPtr);
    const auto* currentBytes = static_cast<const std::byte*>(pointer);

    if (currentBytes < beginBytes || currentBytes > endBytes || (!allowEnd && currentBytes == endBytes)) {
      throw std::out_of_range("Vector iterator position is invalid");
    }

    const std::ptrdiff_t byteOffset = currentBytes - beginBytes;
    if (byteOffset % static_cast<std::ptrdiff_t>(m_elementSize) != 0) {
      throw std::out_of_range("Vector iterator position is invalid");
    }

    return static_cast<std::size_t>(byteOffset / static_cast<std::ptrdiff_t>(m_elementSize));
  }

  /**
   * @brief Gets a pointer to the element at the specified index in the vector. This method checks if the index is valid
   * (i.e., between 0 and the current size of the vector) and if the vector is not empty. If the index is valid and the
   * vector is not empty, it returns a pointer to the element at the specified index.
   *
   * @param index The index of the element to get a pointer to. Must be between 0 and the current size of the vector
   * (inclusive).
   * @return A pointer to the element at the specified index in the vector.
   * @throws std::out_of_range if index is greater than the current size of the vector or if the vector is empty.
   * @throws std::invalid_argument if the vector is empty.
   */
  [[nodiscard]] void* getPointer(const std::size_t index) const {
    if (index > m_size) {
      throw std::out_of_range("Vector iterator position is invalid");
    }

    if (m_data == nullptr) {
      if (index == 0) {
        return nullptr;
      }
      throw std::invalid_argument("Vector is empty");
    }

    return static_cast<std::byte*>(m_data) + index * m_elementSize;
  }

  /**
   * @brief Inserts a range of elements from a contiguous range [first, first + count) at the specified index in the
   * vector. This method assumes that the index is valid (i.e., between 0 and the current size of the vector) and that
   * the first pointer is not nullptr. The method will handle the necessary memory management and construction of the
   * new elements in the vector. After calling this method, the new elements will be inserted at the specified index in
   * the vector, and the size of the vector will be increased by the count of new elements. The method returns a pointer
   * to the first of the newly inserted elements in the vector.
   *
   * @param index The index at which to insert the new elements. Must be between 0 and the current size of the vector
   * (inclusive).
   * @param first A pointer to the first element in the range to be inserted. Must not be nullptr.
   * @param count The number of elements in the range to be inserted. This value should be greater than zero.
   * @return A pointer to the first of the newly inserted elements in the vector.
   * @throws std::invalid_argument if first is nullptr.
   * @throws std::out_of_range if index is greater than the current size of the vector.
   * @throws std::length_error if the resulting size of the vector after insertion would exceed the maximum size allowed
   * by the allocator.
   */
  [[nodiscard]] void* insertRangeAt(const std::size_t index, const void* first, const std::size_t count) {
    if (index > m_size) {
      throw std::out_of_range("Vector iterator position is invalid");
    }

    if (count == 0) {
      return getPointer(index);
    }

    if (first == nullptr) {
      throw std::invalid_argument("Inserted range starting pointer is invalid");
    }

    const std::size_t required = m_size + count;
    ensureBaseCapacityRequest(required);

    const std::size_t newCapacity = required <= m_capacity ? m_capacity : recommendedBaseCapacity(required);
    void* oldData = m_data;
    void* newData = allocateStorage(newCapacity);

    std::size_t constructed = 0;
    try {
      // Move elements before insertion point
      for (; constructed < index; ++constructed) {
        constructAt(getElementAddress(newData, constructed), getElementAddress(oldData, constructed));
      }

      // Copy elements from the source range
      for (std::size_t srcIdx = 0; srcIdx < count; ++srcIdx, ++constructed) {
        copyConstructAt(getElementAddress(newData, constructed), getElementAddress(const_cast<void*>(first), srcIdx));
      }

      // Move elements after insertion point
      for (std::size_t i = index; i < m_size; ++i, ++constructed) {
        constructAt(getElementAddress(newData, constructed), getElementAddress(oldData, i));
      }
    } catch (...) {
      destroyRange(newData, 0, constructed);
      deallocateStorage(newData, newCapacity);
      throw;
    }

    destroyRange(oldData, 0, m_size);
    if (m_capacity > 0) {
      IVector::deallocateStorage();
    }

    m_data = newData;
    m_size = constructed;
    m_capacity = newCapacity;
    return getPointer(index);
  }

  /**
   * @brief Allocates raw storage for the vector with the specified capacity. This method calculates the total size of
   * the required storage based on the element size and the requested capacity, and uses the allocator to allocate the
   * necessary memory. The method also checks if the requested capacity is valid (i.e., not zero and does not exceed the
   * maximum size allowed by the allocator) before attempting to allocate memory. If the capacity is valid, it returns a
   * pointer to the allocated storage; otherwise, it throws an exception.
   *
   * @param capacity The number of elements for which to allocate storage. This value should be greater than zero and
   * should not exceed the maximum size allowed by the allocator.
   * @return A pointer to the allocated storage for the vector.
   * @throws std::length_error if the requested capacity is zero or exceeds the maximum size allowed by the allocator.
   * @throws std::bad_alloc if the allocator fails to allocate the requested memory.
   */
  void* allocateStorage(const std::size_t capacity) override {
    if (capacity == 0) {
      return nullptr;
    }

    if (m_elementSize == 0 || capacity > maxSize()) {
      throw std::length_error("Vector capacity exceeds addressable storage");
    }

    return m_allocator.resource()->allocate(capacity * m_elementSize, m_alignment);
  }

  /**
   * @brief Deallocates the raw storage used by the vector. This method checks if the provided data pointer and capacity
   * are valid (i.e., not nullptr and not zero) before attempting to deallocate memory. If the parameters are valid, it
   * uses the allocator to deallocate the memory previously allocated for the vector's storage. If the parameters are
   * invalid, the method does nothing.
   *
   * @param data A pointer to the storage to be deallocated. Must not be nullptr if capacity is greater than zero.
   * @param capacity The number of elements for which the storage was allocated. Must be greater than zero if data is
   * not nullptr.
   * @throws std::invalid_argument if data is nullptr while capacity is greater than zero, or if capacity is zero while
   * data is not nullptr.
   * @throws std::length_error if the capacity exceeds the maximum size allowed by the allocator.
   * @throws std::bad_alloc if the allocator fails to deallocate the memory (though this is uncommon and typically not
   * expected to throw).
   */
  void deallocateStorage(void* data, const std::size_t capacity) noexcept override {
    if (data == nullptr || capacity == 0 || m_elementSize == 0) {
      return;
    }

    m_allocator.resource()->deallocate(data, capacity * m_elementSize, m_alignment);
  }

  /**
   * @brief Gets the allocator used for memory management in the RawVector. This method returns a polymorphic allocator
   * that is used by the RawVector for allocating and deallocating memory for its storage.
   *
   * @return A polymorphic allocator used for memory management in the RawVector.
   */
  [[nodiscard]] std::pmr::polymorphic_allocator<> getByteAllocator() const noexcept override {
    return m_allocator;
  }

  /**
   * @brief The allocator used for memory management in the RawVector.
   */
  std::pmr::polymorphic_allocator<> m_allocator;
};
}  // namespace core::container
