#pragma once
#include <cstddef>
#include <functional>
#include <memory_resource>
#include <stdexcept>

namespace core::container {
/**
 * @brief Forward declaration of the VectorModifier class, which is a friend of IVector and is used to manipulate the
 * internal state of IVector.
 */
class VectorModifier;

/**
 * @brief IVector is a non-templated base class for Vector that holds the raw data and metadata for a vector-like
 * container. It is not intended to be used directly, but rather to provide a common storage mechanism for Vector and
 * potentially other vector-like containers in the future.
 *
 * It manages the raw memory and size/capacity information, while the derived Vector class handles the type-specific
 * construction, destruction, and element access.
 *
 * In particular, this allows the capability to operate on the underlying data buffer of a vector at runtime without
 * knowing its type.
 */
class IVector {
 public:
  /**
   * @brief The VectorModifier class is declared as a friend of IVector to allow it to access the protected and private
   * members of IVector. This is necessary because VectorModifier needs to be able to manipulate the internal state of
   * IVector, such as resizing the vector or accessing the raw data buffer, in order to provide its functionality.
   */
  friend VectorModifier;

  /**
   * @brief Destructor. Does not deallocate memory or destroy elements, as the derived Vector class is responsible for
   * that. This allows the IVector to be used as a non-owning view of the vector's data when necessary.
   */
  virtual ~IVector() = default;

  /**
   * @brief Returns the number of elements currently stored in the vector. This is used to track the size of the vector
   * and manage element construction and destruction. The derived Vector class will update this value as elements are
   * added, removed, or resized, while the IVector simply provides a common storage mechanism for this information.
   *
   * @return The number of elements currently stored in the vector.
   */
  [[nodiscard]] std::size_t size() const noexcept {
    return m_size;
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
    return m_capacity;
  }

  /**
   * @brief Returns the maximum number of elements that the vector can hold, as determined by the allocator's max_size()
   * method.
   *
   * @return The maximum number of elements that the vector can hold.
   */
  [[nodiscard]] virtual std::size_t maxSize() const noexcept = 0;

  /**
   * @brief Returns whether the vector is empty (i.e., contains no elements). This is determined by checking if the size
   * of the vector is zero. The derived Vector class will update the size as elements are added, removed, or resized,
   * while the IVector simply provides a common storage mechanism for this information.
   *
   * @return True if the vector is empty, false otherwise.
   */
  [[nodiscard]] bool empty() const noexcept {
    return m_size == 0;
  }

  /**
   * @brief Reduces the vector's capacity to match its size, if the size is less than the current capacity. This is used
   * to free up any unused memory when the vector has shrunk in size, and it ensures that the vector's storage is as
   * compact as possible. The derived Vector class will call this method when it needs to shrink the vector's storage,
   * and the IVector will handle the actual memory management and cleanup. After calling this method, the vector's
   * capacity will be equal to its size, and any excess memory will have been deallocated.
   */
  void shrinkToFit() {
    if (m_size < m_capacity) {
      reallocate(m_size);
    }
  }

  /**
   * @brief Reserves storage for at least the specified number of elements. If the current capacity is less than the
   * requested capacity, the vector will grow its storage to accommodate the new capacity. If the requested capacity is
   * greater than the maximum size allowed by the allocator, a std::length_error will be thrown.
   *
   * After calling this method, the vector's capacity will be at least as large as the requested capacity, and any
   * necessary memory will have been allocated.
   *
   * @param capacity The minimum capacity to reserve for the vector.
   */
  void reserve(const std::size_t capacity) {
    if (capacity > maxSize()) {
      throw std::length_error("Vector capacity exceeds max_size");
    }

    if (capacity > m_capacity) {
      reallocate(capacity);
    }
  }

  /**
   * @brief Reallocates the vector's storage to the specified capacity. This method is used internally when the vector
   * needs to grow or shrink its storage, and it handles the actual memory management and element copying. If the new
   * capacity is greater than the maximum size allowed by the allocator, a std::length_error will be thrown. If the new
   * capacity is zero, the vector will be reset to an empty state. After calling this method, the vector's capacity will
   * be equal to the specified capacity, and any necessary memory will have been allocated or deallocated accordingly.
   *
   * @param capacity The new capacity for the vector's storage.
   */
  void reallocate(const std::size_t capacity) {
    if (capacity == m_capacity) {
      return;
    }

    if (capacity == 0) {
      reset();
      return;
    }

    void* oldData = m_data;
    void* data = allocateStorage(capacity);

    const std::size_t size = m_size < capacity ? m_size : capacity;

    std::size_t i = 0;
    try {
      for (; i < size; ++i) {
        constructAt(getElementAddress(data, i), getElementAddress(oldData, i));
      }
    } catch (...) {
      destroyRange(data, 0, i);
      deallocateStorage(data, capacity);
      throw;
    }

    destroyRange(oldData, 0, m_size);
    deallocateStorage();

    m_data = data;
    m_size = size;
    m_capacity = capacity;
  }

  /**
   * @brief Resizes the vector to contain the specified number of elements. If the new size is greater than the current
   * size, new elements will be default-constructed at the end of the vector. If the new size is less than the current
   * size, excess elements will be destroyed. If the new size is greater than the current capacity, the vector will grow
   * its storage to accommodate the new size. If the new size is greater than the maximum size allowed by the allocator,
   * a std::length_error will be thrown.
   *
   * @param newSize The new size for the vector. If this is greater than the current size, new elements will be
   * default-constructed. If this is less than the current size, excess elements will be destroyed.
   */
  void resize(const std::size_t newSize) {
    resizeAndConstruct(newSize, [&](void* destination) { constructAt(destination); });
  }

  /**
   * @brief Clears the vector by destroying all constructed elements and setting the size to zero. This method does not
   * deallocate any memory or change the capacity, as the derived Vector class is responsible for managing the memory
   * and storage. After calling this method, the vector will be empty (size of zero), but it will still have the same
   * capacity and allocated memory as before.
   */
  void clear() noexcept {
    if (!m_data) {
      return;
    }

    destroyRange(m_data, 0, m_size);
    m_size = 0;
  }

  /**
   * @brief Resets the vector to an empty state, deallocating any allocated memory and destroying any constructed
   * elements. This method is used when the vector needs to be cleared or when it is being destroyed, and it ensures
   * that all resources are properly released.
   */
  void reset() noexcept {
    if (!m_data) {
      return;
    }

    destroyRange(m_data, 0, m_size);

    if (m_capacity > 0) {
      deallocateStorage();
    }

    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
  }

#pragma region STL

  /**
   * @brief STL alias for @c shrinkToFit .
   */
  void shrink_to_fit() {
    shrinkToFit();
  }

  /**
   * @brief STL alias for @c maxSize .
   */
  [[nodiscard]] std::size_t max_size() const noexcept {
    return maxSize();
  }

#pragma endregion STL

 protected:
  /**
   * @brief Constructor that initializes the IVector with the specified element size and alignment. The element size and
   * alignment parameters are used to ensure that the memory allocated for the vector's storage is properly sized and
   * aligned for the type of elements that the derived Vector class will manage. After calling this constructor, the
   * IVector will be initialized with a null data pointer, zero size, and zero capacity, and it will be ready for the
   * derived Vector class to manage its storage and elements accordingly.
   */
  explicit IVector(const std::size_t elementSize, const std::size_t alignment = alignof(std::max_align_t))
      : m_data(nullptr), m_size(0), m_capacity(0), m_elementSize(elementSize), m_alignment(alignment) {}

  /**
   * @brief Constructor that initializes the IVector with the specified data pointer, size, capacity, element size, and
   * alignment.
   *
   * After calling this constructor, the IVector will be initialized with the provided
   * data and metadata, and it will be ready for the derived Vector class to manage its storage and elements
   * accordingly.
   *
   * @param data A pointer to the raw data buffer that holds the vector's elements.
   * @param sizeInElements The number of elements currently stored in the vector.
   * @param capacityInElements The total number of elements that can be stored in the currently allocated memory.
   * @param elementSize The size of each element in bytes.
   * @param alignment The alignment requirement for the elements.
   */
  explicit IVector(void* data, const std::size_t sizeInElements, const std::size_t capacityInElements,
                   const std::size_t elementSize, const std::size_t alignment)
      : m_data(data),
        m_size(sizeInElements),
        m_capacity(capacityInElements),
        m_elementSize(elementSize),
        m_alignment(alignment) {}

#pragma region Virtuals

  /**
   * @brief Constructs an element at the specified destination using the default constructor. This method is used by the
   * derived Vector class when it needs to construct new elements, such as during resizing or when adding new elements
   * to the vector.
   *
   * @param destination A pointer to the uninitialized memory where the new element should be constructed.
   */
  virtual void constructAt(void* destination) noexcept = 0;

  /**
   * @brief Constructs an element at the specified destination by moving or copying from the source. This method is used
   * by the derived Vector class when it needs to construct new elements based on existing elements, such as during
   * resizing or when adding new elements to the vector.
   *
   * The source pointer points to an existing element that should be used as the basis for constructing the new element
   * at the destination. The derived Vector class will determine whether to move or copy the element based on the type
   * of the elements and the context in which this method is called.
   *
   * @param destination A pointer to the uninitialized memory where the new element should be constructed.
   * @param source A pointer to an existing element that should be used as the basis for constructing the new element at
   * the destination.
   */
  virtual void constructAt(void* destination, void* source) noexcept = 0;

  /**
   * @brief Destroys the element at the specified location. This method is used by the derived Vector class when it
   * needs to destroy elements, such as during resizing or when removing elements from the vector. The derived Vector
   * class will determine how to properly destroy the element based on its type, and it will call this method to perform
   * the actual destruction. After calling this method, the memory at the specified location will be considered
   * uninitialized and should not be accessed until a new element is constructed there.
   *
   * @param element A pointer to the element that should be destroyed. The derived Vector class will ensure that this
   * pointer is valid and points to a constructed element before calling this method.
   */
  virtual void destroyAt(void* element) noexcept = 0;

  /**
   * @brief Allocates raw memory for the vector's storage with the specified capacity. This method is used by the
   * derived Vector class when it needs to allocate memory for the vector's storage, such as during resizing or when
   * growing the vector. The derived Vector class will determine how to allocate the memory based on the allocator it
   * uses, and it will call this method to perform the actual allocation. The returned pointer should point to a block
   * of memory that is large enough to hold the specified number of elements, and it should be properly aligned for the
   * element type. The derived Vector class will be responsible for managing this memory and ensuring that it is
   * properly deallocated when it is no longer needed.
   *
   * @param capacity The number of elements that the allocated memory should be able to hold. The actual size of the
   * allocated memory will be capacity * elementSize, where elementSize is the size of each element in bytes. The
   * derived Vector class will ensure that this value does not exceed the maximum size allowed by the allocator, and it
   * will call this method to allocate the necessary memory for the vector's storage.
   * @return A pointer to the allocated memory for the vector's storage. The derived Vector class will manage this
   * memory and ensure that it is properly deallocated when it is no longer needed.
   */
  virtual void* allocateStorage(std::size_t capacity) = 0;

  /**
   * @brief Deallocates the raw memory used for the vector's storage. This method is used by the derived Vector class
   * when it needs to deallocate memory for the vector's storage, such as during resizing or when shrinking the vector.
   * The derived Vector class will determine how to deallocate the memory based on the allocator it uses, and it will
   * call this method to perform the actual deallocation. The data pointer should point to a block of memory that was
   * previously allocated by the allocateStorage method, and the capacity should match the capacity that was used when
   * allocating that memory. After calling this method, the memory at the specified data pointer will be considered
   * deallocated and should not be accessed or used until it is allocated again by the allocateStorage method.
   *
   * @param data A pointer to the memory that should be deallocated. This should be a pointer that was previously
   * returned by the allocateStorage method, and it should point to a block of memory that is currently being used for
   * the vector's storage.
   * @param capacity The capacity that was used when allocating the memory at the data pointer. This should match the
   * capacity that was used when the memory was allocated, and it is used by the derived Vector class to determine how
   * much memory to deallocate based on the allocator's requirements. The derived Vector class will ensure that this
   * value is valid and corresponds to the memory being deallocated before calling this method.
   */
  virtual void deallocateStorage(void* data, std::size_t capacity) noexcept = 0;

#pragma endregion Virtuals

  /**
   * @brief Resizes the vector to contain the specified number of elements, using the provided constructor function to
   * construct new elements if the new size is greater than the current size. If the new size is less than the current
   * size, excess elements will be destroyed. If the new size is greater than the current capacity, the vector will grow
   * its storage to accommodate the new size. If the new size is greater than the maximum size allowed by the allocator,
   * a std::length_error will be thrown.
   *
   * @param newSize The new size for the vector. If this is greater than the current size, new elements will be
   * constructed using the provided constructor function. If this is less than the current size, excess elements will be
   * destroyed.
   * @param constructor A function that takes a pointer to uninitialized memory and constructs an element at that
   * location. This is used to construct new elements when the vector is resized to a larger size, and it allows for
   * custom construction logic to be provided by the caller. The constructor function should not throw exceptions, as
   * any exceptions thrown during construction will be caught and handled by the resize method to ensure proper cleanup
   * of any partially constructed elements.
   */
  void resizeAndConstruct(const std::size_t newSize, const std::function<void(void*)>& constructor) {
    if (newSize < m_size) {
      destroyRange(m_data, newSize, m_size);
      m_size = newSize;
      return;
    }

    if (newSize > maxSize()) {
      throw std::length_error("Vector capacity exceeds max_size");
    }

    if (newSize > m_capacity) {
      reallocate(computeCapacity(newSize));
    }

    std::size_t i = m_size;
    try {
      for (; i < newSize; ++i) {
        constructor(getElementAddress(m_data, i));
      }
      m_size = newSize;
    } catch (...) {
      destroyRange(m_data, m_size, i);
      throw;
    }
  }

  /**
   * @brief Deallocates the vector's storage using the current data pointer and capacity. This is a convenience method
   * that calls the virtual deallocateStorage method with the current data and capacity. The derived Vector class will
   * ensure that the current data pointer and capacity are valid before calling this method, and it will call this
   * method to perform the actual deallocation of the vector's storage when necessary.
   */
  void deallocateStorage() noexcept {
    deallocateStorage(m_data, m_capacity);
  }

  /**
   * @brief Ensures that a request for a certain capacity does not exceed the maximum size allowed by the allocator.
   * This method is used internally by the vector when it needs to check if a requested capacity is valid before
   * attempting to allocate memory for it. If the requested capacity exceeds the maximum size, a std::length_error will
   * be thrown to indicate that the request cannot be fulfilled.
   *
   * @param requested The requested capacity that needs to be checked against the maximum size allowed by the allocator.
   * @throws std::length_error if the requested capacity exceeds the maximum size allowed by the allocator.
   */
  void ensureBaseCapacityRequest(const std::size_t requested) const {
    if (requested > maxSize()) {
      throw std::length_error("Vector capacity exceeds max_size");
    }
  }

  /**
   * @brief Calculates the recommended capacity for the vector based on a minimum required capacity. This method is used
   * internally by the vector when it needs to determine how much to grow its storage when a new capacity is needed. The
   * method starts with the current capacity (or 1 if the current capacity is zero) and doubles it until it is at least
   * as large as the minimum required capacity. If doubling the capacity would exceed the maximum size allowed by the
   * allocator, the method will return the minimum required capacity instead. This approach allows the vector to grow
   * its storage efficiently while ensuring that it does not exceed the limits of the allocator.
   *
   * @param minimum The minimum required capacity that the recommended capacity should be at least as large as. This is
   * typically the new size that the vector needs to accommodate, and the recommended capacity will be calculated to
   * ensure that it can hold at least this many elements.
   * @return The recommended capacity for the vector's storage, which will be at least as large as the specified
   * minimum. This is calculated by starting with the current capacity and doubling it until it is large enough, while
   * ensuring that it does not exceed the maximum size allowed by the allocator.
   * @throws std::length_error if the minimum required capacity exceeds the maximum size allowed by the allocator.
   */
  [[nodiscard]] std::size_t recommendedBaseCapacity(const std::size_t minimum) const {
    ensureBaseCapacityRequest(minimum);

    const std::size_t max = maxSize();
    std::size_t result = m_capacity == 0 ? 1 : m_capacity;
    while (result < minimum) {
      const std::size_t doubled = result > max / 2 ? max : result * 2;
      if (doubled <= result) {
        return minimum;
      }
      result = doubled;
    }

    return result;
  }

  /**
   * @brief Calculates the address of the element at the specified index in the vector's data buffer. This method is
   * used internally by the vector when it needs to access or manipulate elements in the data buffer, and it performs
   * pointer arithmetic based on the element size to calculate the correct address. The index should be less than the
   * current size of the vector, and the returned pointer will point to the memory location where the element at that
   * index is stored.
   *
   * @param data A pointer to the raw data buffer that holds the vector's elements.
   * @param index The index of the element for which to calculate the address.
   * @return A pointer to the memory location where the element at the specified index is stored in the data buffer.
   */
  [[nodiscard]] void* getElementAddress(void* data, const std::size_t index) const noexcept {
    auto* bytes = static_cast<std::byte*>(data);
    return bytes + index * m_elementSize;
  }

  /**
   * @brief Calculates the address of the element at the specified index in the vector's data buffer. This method is
   * used internally by the vector when it needs to access or manipulate elements in the data buffer, and it performs
   * pointer arithmetic based on the element size to calculate the correct address. The index should be less than the
   * current size of the vector, and the returned pointer will point to the memory location where the element at that
   * index is stored.
   *
   * @param data A pointer to the raw data buffer that holds the vector's elements.
   * @param index The index of the element for which to calculate the address.
   * @return A pointer to the memory location where the element at the specified index is stored in the data buffer.
   */
  [[nodiscard]] const void* getElementAddress(const void* data, const std::size_t index) const noexcept {
    auto* bytes = static_cast<const std::byte*>(data);
    return bytes + index * m_elementSize;
  }

  /**
   * @brief Destroys a range of elements in the vector's data buffer, starting from the element at the specified begin
   * index up to (but not including) the element at the specified end index. This method is used internally by the
   * vector when it needs to destroy multiple elements, such as during resizing or when removing a range of elements
   * from the vector. The method iterates over the specified range of indices and calls the destroyAt method for each
   * element in that range to properly destroy them. After calling this method, the
   * elements in the specified range will have been destroyed, and the memory they occupied will be considered
   * uninitialized.
   *
   * @param data A pointer to the raw data buffer that holds the vector's elements.
   * @param begin The index of the first element in the range to be destroyed.
   * @param end The index one past the last element in the range to be destroyed.
   */
  void destroyRange(void* data, const std::size_t begin, const std::size_t end) noexcept {
    if (!data) {
      return;
    }

    for (std::size_t i = end; i > begin; --i) {
      destroyAt(getElementAddress(data, i - 1));
    }
  }

  /**
   * @brief The raw data pointer that holds the vector's elements. The derived Vector class is responsible for
   * allocating and managing this memory, as well as constructing and destroying the elements. The IVector simply
   * provides a common storage mechanism for this data, allowing the Vector to operate on it without needing to know the
   * specific type of the elements.
   */
  void* m_data;

  /**
   * @brief The number of elements currently stored in the vector. This is used to track the size of the vector and
   * manage element construction and destruction. The derived Vector class will update this value as elements are added,
   * removed, or resized, while the IVector simply provides a common storage mechanism for this information.
   *
   * Note that this is the number of elements, not the number of bytes. The actual memory usage can be calculated as
   * m_size * m_elementSize, and the total allocated memory can be calculated as m_capacity * m_elementSize.
   */
  std::size_t m_size;

  /**
   * @brief The total number of elements that can be stored in the currently allocated memory. This is used to determine
   * when the vector needs to grow its storage. The derived Vector class will update this value when it allocates or
   * deallocates memory for the vector's storage, while the IVector simply provides a common storage mechanism for this
   * information.
   *
   * Note that this is the number of elements, not the number of bytes. The actual memory usage can be calculated as
   * m_size * m_elementSize, and the total allocated memory can be calculated as m_capacity * m_elementSize.
   */
  std::size_t m_capacity;

  /**
   * @brief The size of each element in bytes. This is used to calculate the total memory needed for the vector's
   * storage, as well as to perform pointer arithmetic when accessing elements in the data buffer. The derived Vector
   * class will set this value based on the type of elements it manages, and the IVector will use it to provide a common
   * storage mechanism for the vector's data and metadata. This allows the Vector to operate on the underlying data
   * buffer without needing to know the specific type of the elements, as it can use the element size to calculate
   * offsets and manage memory accordingly.
   */
  std::size_t m_elementSize;

  /**
   * @brief The alignment requirement for the elements. This is used to ensure that the allocated memory is properly
   * aligned for the element type, which is important for performance and correctness on many platforms. The derived
   * Vector class will set this value based on the alignment requirements of the element type it manages, and the
   * IVector will use it to provide a common storage mechanism for the vector's data and metadata.
   */
  std::size_t m_alignment;

 private:
  /**
   * @brief Calculates the recommended capacity for the vector when resizing or growing. This method implements a growth
   * strategy that typically doubles the capacity when more space is needed, while also ensuring that the capacity does
   * not exceed the maximum allowed by the allocator.
   *
   * The method takes a minimum required capacity as input and returns a recommended capacity that is greater than or
   * equal to the minimum, following the growth strategy. This allows the vector to efficiently manage its storage and
   * minimize the number of reallocations needed as elements are added or removed.
   *
   * @param minimum The minimum required capacity in terms of the number of elements. The returned recommended capacity
   * will be at least this value.
   * @return The recommended capacity in terms of the number of elements, which is greater than or equal to the minimum
   * and follows the growth strategy.
   */
  [[nodiscard]] std::size_t computeCapacity(const std::size_t minimum) const {
    const auto max = maxSize();

    if (minimum > max) {
      throw std::length_error("Vector capacity exceeds max size");
    }

    std::size_t result = m_capacity == 0 ? 1 : m_capacity;

    while (result < minimum) {
      const std::size_t doubled = result > max / 2 ? max : result * 2;
      if (doubled <= result) {
        return minimum;
      }
      result = doubled;
    }

    return result;
  }
};
}  // namespace core::container
