#pragma once
#include <cstddef>

namespace core::memory {
/**
 * @brief A simple wrapper that manages the lifecycle of a raw memory buffer. The Buffer class is responsible for
 * allocating and deallocating a contiguous block of memory, and providing access to the raw memory.
 *
 * The Buffer class is designed to be move-only and cannot be copied. The memory buffer is released when the object is
 * destructed.
 */
class Buffer {
 public:
  /**
   * @brief Constructs a new Buffer object with the specified size in bytes. The constructor allocates a contiguous
   * block of memory of the specified size, and initializes the internal state of the Buffer object.
   *
   * @param size The size of the memory buffer to allocate in bytes.
   */
  explicit Buffer(std::size_t size);

  /**
   * @brief Move constructor for the Buffer class. This constructor transfers ownership of the memory buffer from the
   * source Buffer object to the newly constructed Buffer object. After the move, the source Buffer object is left in a
   * valid but unspecified state, and the memory buffer is now owned by the new Buffer object.
   */
  ~Buffer();

  /**
   * @brief The copy constructor is deleted to prevent copying of Buffer objects, as they manage unique resources
   * (memory buffers).
   */
  Buffer(const Buffer&) = delete;

  /**
   * @brief The move assignment operator is deleted to prevent moving of Buffer objects, as they manage unique resources
   * (memory buffers).
   */
  Buffer(Buffer&&) noexcept;

  /**
   * @brief The copy assignment operator is deleted to prevent copying of Buffer objects, as they manage unique
   * resources (memory buffers).
   */
  Buffer& operator=(const Buffer&) = delete;

  /**
   * @brief The move assignment operator is deleted to prevent moving of Buffer objects, as they manage unique resources
   * (memory buffers).
   */
  Buffer& operator=(Buffer&&) = delete;

  /**
   * @brief Returns a pointer to the byte at the specified offset within the memory buffer. The offset is zero-based,
   * and the size of the buffer is determined by the size specified when the Buffer was created. If the offset is out of
   * range (greater than or equal to the size of the buffer), a @c std::out_of_range exception is thrown.
   *
   * @param offset The zero-based offset of the byte to retrieve within the memory buffer.
   * @return A pointer to the byte at the specified offset within the memory buffer.
   * @throws std::out_of_range If the offset is out of range (greater than or equal to the size of the buffer).
   */
  [[nodiscard]] void* at(std::size_t offset) const;

  /**
   * @brief Returns the size of the memory buffer in bytes.
   *
   * @return The size of the memory buffer in bytes.
   */
  [[nodiscard]] std::size_t size() const;

  /**
   * @brief Checks if the specified address is within the range of the memory buffer managed by this Buffer object. The
   * address is compared against the starting address of the buffer and the ending address (starting address + size). If
   * the address is greater than or equal to the starting address and less than the ending address, the function returns
   * true, indicating that the address is contained within the buffer. Otherwise, it returns false.
   *
   * @param address The address to check for containment within the memory buffer.
   * @return true if the specified address is within the range of the memory buffer, false otherwise.
   */
  bool contains(uintptr_t address) const;

  /**
   * @brief Checks if the specified pointer is within the range of the memory buffer managed by this Buffer object. The
   * pointer is converted to an address and compared against the starting address of the buffer and the ending address
   * (starting address + size). If the pointer's address is greater than or equal to the starting address and less than
   * the ending address, the function returns true, indicating that the pointer is contained within the buffer.
   * Otherwise, it returns false.
   *
   * @param ptr The pointer to check for containment within the memory buffer.
   * @return true if the specified pointer is within the range of the memory buffer, false otherwise.
   */
  bool contains(void* ptr) const;

 private:
  /**
   * @brief The size of the memory buffer in bytes.
   */
  const std::size_t m_size;

  /**
   * @brief The raw memory buffer managed by this Buffer object.
   */
  mutable std::byte* m_data;
};
}  // namespace core::memory
