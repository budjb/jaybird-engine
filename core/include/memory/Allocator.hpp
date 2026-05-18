#pragma once

#include <cstddef>
#include <concepts>
#include <limits>
#include <memory>
#include <new>
#include <type_traits>

#include "MemoryPool.hpp"

namespace core::memory {
/**
 * @brief Concept that defines the requirements for a type to be considered an Allocator. This concept checks for the
 * presence of the necessary type aliases and member functions that are required for a type to be used as an allocator
 * in the C++ Standard Library. If a type satisfies all of these requirements, it can be used as an allocator in
 * standard containers and algorithms that support custom allocators.
 */
template <typename A>
concept Allocator =
    requires(A a, typename std::allocator_traits<A>::size_type n, typename std::allocator_traits<A>::pointer p) {
      typename std::allocator_traits<A>::value_type;
      { std::allocator_traits<A>::allocate(a, n) } -> std::same_as<typename std::allocator_traits<A>::pointer>;
      { std::allocator_traits<A>::deallocate(a, p, n) } -> std::same_as<void>;
    };

/**
 * @brief A variable template that provides a convenient way to check if a type satisfies the Allocator concept. This
 * variable template can be used in static assertions or other compile-time checks to ensure that a type is a valid
 * allocator before using it in standard containers or algorithms that require allocators. If the type satisfies the
 * Allocator concept, this variable will be true; otherwise, it will be false.
 *
 * @tparam T The type to check for satisfying the Allocator concept.
 * @return true if the type satisfies the Allocator concept, false otherwise.
 */
template <typename T>
inline constexpr bool is_allocator_v = Allocator<T>;

/**
 * @brief A simple allocator class that manages memory allocations and deallocations using the global heap. This
 * allocator is designed to satisfy the C++ Standard Library's allocator requirements, allowing it to be used with
 * standard containers and algorithms that support custom allocators. The allocator uses the global heap for all memory
 * operations, and does not provide any additional functionality or optimizations beyond what is required by the
 * Allocator concept.
 *
 * @tparam T The type of objects that this allocator manages. This type will be used to determine the size of memory
 * blocks to allocate and deallocate, and to ensure that the allocator is compatible with standard containers that
 * require specific types of allocators.
 */
template <typename T>
class HeapAllocator {
 public:
  /**
   * @brief The type of objects that this allocator manages. This type will be used to determine the size of memory
   * blocks to allocate and deallocate, and to ensure that the allocator is compatible with standard containers that
   * require specific types of allocators.
   */
  using value_type = T;

  /**
   * @brief Indicates that this allocator should propagate on container move assignment.
   *
   * This means that if a container is move-assigned from another container that uses this allocator, the allocator will
   * be moved along with the container, rather than being copied or left unchanged. This is important for ensuring that
   * the allocator's resources are properly transferred during move operations, and can help to optimize performance by
   * avoiding unnecessary copies of the allocator.
   */
  using propagate_on_container_move_assignment = std::true_type;

  /**
   * @brief Indicates that all instances of this allocator type are considered equal.
   *
   * This means that if two containers use this allocator type, they will be considered to have the same allocator,
   * regardless of whether they are the same instance or not.
   */
  using is_always_equal = std::true_type;

  /**
   * @brief Rebind structure for the Allocator class. This structure allows for rebinding the allocator to a different
   * type, while still satisfying the Allocator concept. The @c other type alias within this structure defines the type
   * of allocator that can be used to manage objects of the new type. This is important for ensuring that the allocator
   * can be used with standard containers that require specific types of allocators, and allows for flexibility in how
   * the allocator can be used with different types of objects.
   *
   * @tparam U The type of objects that the rebound Allocator will manage. This type can be different from the type
   * managed by this Allocator, as long as the rebound Allocator satisfies the Allocator concept and can be used with
   * standard containers that require specific types of allocators.
   */
  template <typename U>
  friend class HeapAllocator;

  /**
   * @brief Constructs a new heap allocator.
   */
  HeapAllocator() noexcept = default;

  /**
   * @brief Copy constructor for the Allocator class.
   */
  HeapAllocator(const HeapAllocator&) noexcept = default;

  /**
   * @brief Move constructor for the Allocator class.
   */
  HeapAllocator(HeapAllocator&&) noexcept = default;

  /**
   * @brief Copy constructor for the Allocator class that allows for copying of Allocator objects.
   *
   * @tparam U The type of objects that the other Allocator manages.
   */
  template <typename U>
  HeapAllocator(const HeapAllocator<U>&) noexcept {}

  /**
   * @brief Copy assignment for the Allocator class.
   *
   * @returns A reference to this Allocator object after the assignment.
   */
  HeapAllocator& operator=(const HeapAllocator&) noexcept = default;

  /**
   * @brief Move assignment for the Allocator class.
   *
   * @returns A reference to this Allocator object after the assignment.
   */
  HeapAllocator& operator=(HeapAllocator&&) noexcept = default;

  /**
   * @brief Destructor for the Allocator class.
   */
  ~HeapAllocator() noexcept = default;

  /**
   * @brief Allocates a block of memory from the global heap for an array of objects of the specified type. The size of
   * the memory block to allocate is determined by the number of objects and the size of the type, and the allocation is
   * made using the global heap allocator (e.g., using ::operator new).
   *
   * @param n The number of objects to allocate memory for.
   * @return A pointer to the allocated memory block. The caller is responsible for ensuring that the allocated memory
   * is properly deallocated when it is no longer needed.
   */
  T* allocate(const std::size_t n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      throw std::bad_array_new_length();
    }

    if constexpr (alignof(T) > alignof(std::max_align_t)) {
      return static_cast<T*>(operator new(n * sizeof(T), static_cast<std::align_val_t>(alignof(T))));
    } else {
      return static_cast<T*>(operator new(n * sizeof(T)));
    }
  }

  /**
   * @brief Frees a block of memory back to the global heap. The provided pointer must have been previously allocated
   * from the global heap, and must not have already been freed. The pointer is deallocated using the global heap
   * deallocator (e.g., using ::operator delete).
   *
   * @param ptr The pointer to the block of memory to free. This pointer must have been previously allocated from the
   * global heap, and must not have already been freed.
   */
  void deallocate(T* ptr, const std::size_t) noexcept {
    if constexpr (alignof(T) > alignof(std::max_align_t)) {
      ::operator delete(ptr, static_cast<std::align_val_t>(alignof(T)));
    } else {
      ::operator delete(ptr);
    }
  }

  [[nodiscard]] HeapAllocator select_on_container_copy_construction() const noexcept {
    return {};
  }
};

/**
 * @brief Equality comparison operator for Allocator objects.
 *
 * @tparam T  The type of objects that the first Allocator manages.
 * @tparam U The type of objects that the second Allocator manages.
 * @return true if the two Allocator objects are considered equal, false otherwise.
 */
template <typename T, typename U>
bool operator==(const HeapAllocator<T>&, const HeapAllocator<U>&) noexcept {
  return true;
}

/**
 * @brief Inequality comparison operator for Allocator objects.
 *
 * @tparam T The type of objects that the first Allocator manages.
 * @tparam U The type of objects that the second Allocator manages.
 * @return true if the two Allocator objects are not considered equal, false otherwise.
 */
template <typename T, typename U>
bool operator!=(const HeapAllocator<T>&, const HeapAllocator<U>&) noexcept {
  return false;
}

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
class PoolAllocator {
 public:
  /**
   * @brief The type of objects that this allocator manages.
   */
  using value_type = T;

  using propagate_on_container_move_assignment = std::true_type;
  using is_always_equal = std::false_type;

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
  explicit PoolAllocator(MemoryPool* pool = nullptr) noexcept : m_pool(pool) {};

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
  PoolAllocator(const PoolAllocator<U>& other) noexcept : m_pool(other.pool()) {}

  /**
   * @brief Rebind structure for the Allocator class. This structure allows for rebinding the allocator to a different
   * type, while still sharing the same memory pool. The @c other type alias within this structure defines the type of
   * allocator that can be used to manage objects of the new type.

   * @tparam U The type of objects that the rebound Allocator will manage. This type can be different from the type
   * managed by this Allocator, as long as both Allocator objects share the same memory pool.
   */
  template <class U>
  struct rebind {
    using other = PoolAllocator<U>;
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
  T* allocate(const std::size_t n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      throw std::bad_array_new_length();
    }

    if (m_pool) {
      return static_cast<T*>(m_pool->allocate(n * sizeof(T), alignof(T)));
    }

    if constexpr (alignof(T) > alignof(std::max_align_t)) {
      return static_cast<T*>(operator new(n * sizeof(T), static_cast<std::align_val_t>(alignof(T))));
    } else {
      return static_cast<T*>(operator new(n * sizeof(T)));
    }
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
  void deallocate(T* ptr, const std::size_t n) noexcept {
    if (m_pool) {
      m_pool->deallocate(ptr, n * sizeof(T), alignof(T));
    } else {
      if constexpr (alignof(T) > alignof(std::max_align_t)) {
        ::operator delete(ptr, static_cast<std::align_val_t>(alignof(T)));
      } else {
        ::operator delete(ptr);
      }
    }
  }

  [[nodiscard]] PoolAllocator select_on_container_copy_construction() const noexcept {
    return *this;
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
bool operator==(const PoolAllocator<T>& a, const PoolAllocator<U>& b) noexcept {
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
bool operator!=(const PoolAllocator<T>& a, const PoolAllocator<U>& b) noexcept {
  return a.pool() != b.pool();
}
}  // namespace core::memory
