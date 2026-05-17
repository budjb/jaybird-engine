#pragma once

#include <cstdint>

#include "MemoryPool.hpp"

namespace core::memory {
/**
 * @brief A custom allocator class that manages memory allocations and deallocations using a memory pool.
 *
 * This allocator is designed to be compatible with the C++ Standard Library's allocator requirements, allowing it to be
 * used with standard containers and algorithms that support custom allocators. The allocator can be configured to use a
 * specific memory pool for all memory operations, or it can default to using the global heap allocator if no memory
 * pool is provided.
 *
 * @tparam T The type of objects that this allocator manages. This type will be used to determine the size of memory
 * blocks to allocate and deallocate, and to ensure that the allocator is compatible with standard containers that
 * require specific types of allocators.
 */
template <typename T>
class Allocator {
 public:
  /**
   * @brief The type of objects that this allocator manages.
   */
  using value_type = T;

  /**
   * @brief The type used to represent the size of memory blocks and the number of objects to allocate.
   */
  using size_type = std::size_t;

  /**
   * @brief The type used to represent the difference between two pointers, or the number of objects between two
   * pointers.
   */
  using difference_type = std::ptrdiff_t;

  /**
   * @brief The type used to represent a pointer to an object of the type managed by this allocator, and a pointer to a
   * constant object of the type managed by this allocator.
   */
  using pointer = T*;

  /**
   * @brief The type used to represent a pointer to a constant object of the type managed by this allocator.
   */
  using const_pointer = const T*;

  /**
   * @brief Constructs a new Allocator object.
   *
   * If a memory pool is provided, the allocator will use that pool for all memory allocations and deallocations. If no
   * memory pool is provided (i.e., if the argument is nullptr), the allocator will default to using the global heap
   * allocator (e.g., using ::operator new and ::operator delete) for all memory operations.
   *
   * @param pool An optional pointer to a MemoryPool object that this allocator will use for memory management. If
   * nullptr, the allocator will use the global heap allocator instead.
   */
  explicit Allocator(MemoryPool* pool = nullptr) noexcept : m_pool(pool) {};

  /**
   * @brief Copy constructor for the Allocator class. This constructor allows for copying of Allocator objects, but the
   * copy will share the same memory pool as the original. This means that both the original and the copied Allocator
   * will use the same memory pool for all memory operations, and any changes to the memory pool (e.g., allocations or
   * deallocations) will affect both Allocator objects.
   *
   * @tparam U The type of objects that the other Allocator manages. This type can be different from the type managed by
   * this Allocator, as long as both Allocator objects share the same memory pool.
   * @param other The Allocator object to copy. The copied Allocator will share the same memory pool as this Allocator,
   * and will use that pool for all memory operations.
   */
  template <class U>
  explicit Allocator(const Allocator<U>& other) noexcept : m_pool(other.pool()) {}

  /**
   * @brief Rebind structure for the Allocator class. This structure allows for rebinding the allocator to a different
   * type, while still sharing the same memory pool. The @c other type alias within this structure defines the type of
   * allocator that can be used to manage objects of the new type.

   * @tparam U The type of objects that the rebound Allocator will manage. This type can be different from the type
   * managed by this Allocator, as long as both Allocator objects share the same memory pool.
   */
  template <class U>
  struct rebind {
    using other = Allocator<U>;
  };

  /**
   * @brief Allocates a block of memory from the memory pool for an array of objects of the specified type. The size of
   * the memory block to allocate is determined by the number of objects and the size of the type, and the allocation is
   * made from the appropriate bin based on the size. If the size of the memory block exceeds the largest block size
   * configured for this memory pool, the allocation is forwarded to the global heap allocator (e.g., using ::operator
   * new).
   *
   * @param n The number of objects to allocate memory for.
   * @return A pointer to the allocated memory block. The caller is responsible for ensuring that the allocated memory
   * is properly deallocated when it is no longer needed.
   */
  T* allocate(const size_type n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      throw std::bad_array_new_length();
    }

    if (m_pool) {
      return static_cast<T*>(m_pool->allocate(n * sizeof(T), alignof(T)));
    }

    return static_cast<T*>(operator new(n * sizeof(T)));
  }

  /**
   * @brief Frees a block of memory back to the memory pool. The provided pointer must have been previously allocated
   * from this memory pool, and must not have already been freed. The pointer is added back to the appropriate bin's
   * free list, making it available for future allocations. If the pointer was allocated from the global heap allocator
   * (e.g., using ::operator new), it will be deallocated using the global heap deallocator (e.g., using ::operator
   * delete).
   *
   * @param ptr The pointer to the block of memory to free. This pointer must have been previously allocated from this
   * memory pool, and must not have already been freed.
   * @param n The number of objects that were allocated in the memory block. This number will be used to determine the
   * size of the memory block to free, and to ensure that the block is properly deallocated.
   */
  void deallocate(T* ptr, const size_type n) noexcept {
    if (m_pool) {
      m_pool->deallocate(ptr, n * sizeof(T));
    } else {
      operator delete(ptr);
    }
  }

  /**
   * @brief Returns a pointer to the memory pool that this allocator uses for memory management.
   *
   * @return A pointer to the MemoryPool object that this allocator uses for memory management. If this pointer is
   * nullptr, the allocator uses the global heap allocator for all memory operations.
   */
  [[nodiscard]] MemoryPool* pool() const noexcept {
    return m_pool;
  }

 private:
  /**
   * @brief A pointer to the memory pool that this allocator uses for memory management. If this pointer is nullptr, the
   * allocator will use the global heap allocator for all memory operations instead. This member variable is initialized
   * in the constructor, and is used in the @c allocate() and @c deallocate() methods to determine whether to use the
   * memory pool or the global heap allocator for memory operations.
   */
  MemoryPool* m_pool;
};

/**
 * @brief Equality comparison operator for Allocator objects. This operator checks if two Allocator objects are
 * considered equal.
 *
 * @tparam T The type of objects that the first Allocator manages.
 * @tparam U The type of objects that the second Allocator manages.
 * @return true if the two Allocator objects are considered equal, false otherwise.
 */
template <typename T, typename U>
bool operator==(const Allocator<T>& a, const Allocator<U>& b) noexcept {
  return a.pool() == b.pool();
}

/**
 * @brief Inequality comparison operator for Allocator objects. This operator checks if two Allocator objects are not
 * considered equal.
 *
 * @tparam T The type of objects that the first Allocator manages.
 * @tparam U The type of objects that the second Allocator manages.
 * @return true if the two Allocator objects are not considered equal, false otherwise.
 */
template <typename T, typename U>
bool operator!=(const Allocator<T>& a, const Allocator<U>& b) noexcept {
  return a.pool() != b.pool();
}
}  // namespace core::memory
