#pragma once
#include <cstddef>

namespace core::memory {
/**
 * @brief A simple memory arena allocator that allows for fast allocation of memory blocks. It does not support
 * deallocation of individual blocks, but instead allows for clearing the entire arena at once.
 */
class MemoryArena {
 public:
  /**
   * @brief Constructs a MemoryArena with the given size in bytes.
   *
   * @param size The size of the memory arena in bytes.
   * @note The memory arena will allocate a contiguous block of memory of the specified size.
   */
  explicit MemoryArena(std::size_t size) noexcept;

  /**
   * @brief Destructs the MemoryArena and releases the allocated memory.
   *
   * @note This will free the entire memory block allocated for the arena. Any pointers returned by the allocate()
   * method will become invalid after this destructor is called.
   */
  ~MemoryArena() noexcept;

  /**
   * @brief Allocates a block of memory of the specified size and alignment from the arena.
   *
   * This method will return a pointer to a block of memory that is at least `size` bytes in size and is aligned to
   * the specified `alignment`. The `alignment` parameter must be a power of two and must be less than or equal to
   * `alignof(std::max_align_t)`.
   *
   * If there is not enough memory left in the arena to satisfy the allocation request, this method will return
   * `nullptr`.
   *
   * @param size The size of the memory block to allocate in bytes.
   * @param alignment The alignment requirement for the allocated memory block.
   * @return A pointer to the allocated memory block, or `nullptr` if the allocation fails due to insufficient memory.
   * @note The caller is responsible for ensuring that the allocated memory block is not used after the arena is cleared
   * or destroyed, as this will lead to undefined behavior.
   */
  void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) noexcept;

  /**
   * @brief Clears the memory arena, resetting the allocation cursor to the beginning of the arena. This effectively
   * "frees" all allocated memory blocks at once, but does not actually deallocate any memory. After calling this
   * method, all pointers returned by previous calls to allocate() will become invalid and should not be used.
   *
   * This method is useful for quickly resetting the arena to a clean state without having to deallocate individual
   * blocks of memory.
   */
  void clear() noexcept;

 private:
  /**
   * @brief The size of the memory arena in bytes. This value is set during construction and does not change during the
   * lifetime of the arena.
   */
  const std::size_t m_size;

  /**
   * @brief A pointer to the beginning of the allocated memory block for the arena. This pointer is initialized during
   * construction and is used as the base address for all allocations from the arena.
   */
  std::byte* m_data;

  /**
   * @brief The current allocation cursor, represented as an integer offset from the base address of the arena. This
   * value is updated each time a new block of memory is allocated from the arena, and is reset to the base address when
   * the arena is cleared. The cursor is used to keep track of the next available memory location for allocation, and is
   * used to ensure that allocations are properly aligned according to the specified alignment requirements.
   */
  uintptr_t m_cursor;
};
}  // namespace core::memory
