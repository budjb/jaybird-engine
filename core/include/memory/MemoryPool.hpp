#pragma once

#include <shared_mutex>
#include <vector>

#include "Buffer.hpp"
#include "Common.hpp"
#include "rtti/RTTI.hpp"

namespace core::memory {
/**
 * @brief An interface that defines the configuration for a memory bin. The IBinConfig class provides information about
 * the size of the blocks in the bin, as well as the number of blocks that can be stored in the bin. This information is
 * used by the Bin class to manage the memory allocation and deallocation of blocks within the bin.
 */
class IBinConfig {
 public:
  /**
   * @brief Constructs a new IBinConfig object with the specified number of blocks and block size. The constructor
   * initializes the internal state of the IBinConfig object, and sets the size and capacity of the bin based on the
   * provided parameters.
   *
   * @param blocks The number of blocks that can be stored in the bin. This determines the capacity of the bin, and is
   * used to calculate the total size of the memory chunk that will be allocated for the bin.
   * @param blockSize The size of each block in bytes. This determines the size of each block that will be allocated
   * within the bin, and is used to calculate the total size of the memory chunk that will be allocated for the bin.
   * @param growthNumerator Numerator for the growth ratio used when computing grow size.
   * @param growthDenominator Denominator for the growth ratio used when computing grow size.
   *
   */
  explicit IBinConfig(std::size_t blocks, std::size_t blockSize, std::size_t growthNumerator = 1,
                      std::size_t growthDenominator = 1) noexcept;

  /**
   * @brief Destructs the IBinConfig object. The destructor is declared as pure virtual to make the class abstract.
   */
  virtual ~IBinConfig() = default;

  /**
   * @brief Returns the size of each block in bytes.
   *
   * @return The size of each block in bytes.
   */
  [[nodiscard]] std::size_t size() const noexcept;

  /**
   * @brief Returns the number of blocks that can be stored in the bin.
   *
   * @return The number of blocks that can be stored in the bin.
   */
  [[nodiscard]] std::size_t capacity() const noexcept;

  /**
   * @brief Returns the numerator of the growth ratio used when computing the size to grow the bin.
   *
   * A growth ratio less than 1 will result in logarithmic growth of the bin, a growth ratio of 1 will result in linear
   * growth of the bin, and a growth ratio greater than 1 will result in exponential growth of the bin. The growth ratio
   * allows for a flexible approach to managing the growth of the bin, and can be adjusted based on the expected
   * allocation patterns and performance requirements of the application.
   *
   * The choice of growth ratio can impact the performance of the memory pool, as it affects how quickly the bin grows
   * in response to allocation requests, and how efficiently it manages memory over time.
   *
   * @return The numerator of the growth ratio used when computing the size to grow the bin.
   */
  [[nodiscard]] std::size_t growthNumerator() const noexcept {
    return m_growthNumerator;
  }

  /**
   * @brief Returns the denominator of the growth ratio used when computing the size to grow the bin.
   *
   * A growth ratio less than 1 will result in logarithmic growth of the bin, a growth ratio of 1 will result in linear
   * growth of the bin, and a growth ratio greater than 1 will result in exponential growth of the bin. The growth ratio
   * allows for a flexible approach to managing the growth of the bin, and can be adjusted based on the expected
   * allocation patterns and performance requirements of the application.
   *
   * The choice of growth ratio can impact the performance of the memory pool, as it affects how quickly the bin grows
   * in response to allocation requests, and how efficiently it manages memory over time.
   *
   * @return The denominator of the growth ratio used when computing the size to grow the bin.
   */
  [[nodiscard]] std::size_t growthDenominator() const noexcept {
    return m_growthDenominator;
  }

 private:
  /**
   * @brief The size of each block in bytes.
   */
  const std::size_t m_size;

  /**
   * @brief The number of blocks that can be stored in the bin.
   */
  const std::size_t m_capacity;

  /**
   * @brief The numerator of the growth ratio used when computing the size to grow the bin.
   */
  const std::size_t m_growthNumerator;

  /**
   * @brief The denominator of the growth ratio used when computing the size to grow the bin.
   */
  const std::size_t m_growthDenominator;
};

/**
 * @brief A concrete implementation of the IBinConfig interface that represents a memory bin with a fixed block size.
 * The BinConfig class is a template class that takes a single template parameter, BlockSize, which specifies the size
 * of each block in bytes.
 *
 * The BinConfig class is constrained to only accept block sizes that are greater than zero and are powers of two. This
 * constraint ensures that the block size is valid for memory allocation and deallocation, and allows for efficient
 * memory management within the bin.
 *
 * @tparam BlockSize The size of each block in bytes. This must be greater than or equal to 8 and a power of two (e.g.,
 * 8, 16, 32, etc.).
 */
template <std::size_t BlockSize>
  requires(BlockSize >= MINIMUM_ALIGNMENT && (BlockSize & BlockSize - 1) == 0)
class BinConfig : public IBinConfig {
 public:
  /**
   * @brief Constructs a new BinConfig object with the specified number of blocks.
   *
   * @param blocks The number of blocks that can be stored in the bin.
   * @param growthNumerator Numerator for the growth ratio used when computing grow size.
   * @param growthDenominator Denominator for the growth ratio used when computing grow size.
   */
  explicit BinConfig(const std::size_t blocks, const std::size_t growthNumerator = 1,
                     const std::size_t growthDenominator = 1) noexcept
      : IBinConfig(blocks, BlockSize, growthNumerator, growthDenominator) {}

  /**
   * @brief Destructs the BinConfig object.
   */
  ~BinConfig() override = default;
};

/**
 * @brief An intrusive free node used in the Bin's freelist. Each free block in a bin stores a FreeNode
 * at its beginning, allowing for a singly-linked freelist without external storage overhead.
 */
struct Block {
  Block* next;
};

/**
 * @brief A memory bin that manages a collection of memory chunks and a free list of available blocks. The Bin class
 * provides an interface for allocating and deallocating memory blocks from the bin, and internally manages the memory
 * chunks and free list to efficiently handle memory requests. The Bin class allows for efficient memory management by
 * grouping blocks of similar sizes into bins, which can reduce fragmentation and improve allocation performance.
 */
class Bin {
 public:
  /**
   * @brief Constructs a new Bin object with the specified configuration. The constructor initializes the internal state
   * of the Bin object, and sets up the initial memory chunk based on the provided configuration.
   *
   * @param config A shared pointer to an IBinConfig object that defines the configuration for the bin.
   */
  explicit Bin(const std::shared_ptr<const IBinConfig>& config) noexcept;

  /**
   * @brief Move constructor for the Bin class. This constructor allows for efficient transfer of ownership of the
   * resources managed by the Bin object, such as the memory chunks and free list. After the move, the source Bin object
   * is left in a valid but unspecified state, and the resources are now owned by the new Bin object.
   */
  explicit Bin(Bin&& other) noexcept;

  /**
   * @brief The copy constructor is deleted to prevent copying of Bin objects, as they manage unique resources (memory
   * chunks and free list).
   */
  explicit Bin(const Bin&) = delete;

  /**
   * @brief The copy assignment operator is deleted to prevent copying of Bin objects, as they manage unique resources
   * (memory chunks and free list).
   */
  Bin& operator=(const Bin&) const = delete;

  /**
   * @brief The move assignment operator is deleted to prevent moving of Bin objects, as they manage unique resources
   * (memory chunks and free list).
   */
  Bin& operator=(Bin&&) = delete;

  /**
   * @brief Allocates a block of memory from the bin.
   *
   * The allocation will take the block at the top of the free list. If there are no elements in the free list, a new
   * chunk will be allocated and configured.
   *
   * The allocation process is thread-safe, allowing for concurrent allocations
   * from multiple threads without causing data races or other synchronization issues.
   * @return
   */
  void* allocate() noexcept;

  /**
   * @brief Frees a block of memory back to the bin. The provided pointer must have been previously allocated from this
   * bin, and must not have already been freed. The pointer is added back to the free list, making it available for
   * future allocations.
   *
   * The deallocation process is thread-safe, allowing for concurrent deallocations from multiple threads without
   * causing data races or other synchronization issues.
   *
   * @param ptr The pointer to the block of memory to free. This pointer must have been previously allocated from this
   * bin, and must not have already been freed.
   * @throws std::invalid_argument If the provided pointer is not valid (e.g., it was not allocated from this bin, or it
   * has already been freed).
   */
  void deallocate(void* ptr) noexcept;

  /**
   * @brief Returns a reference to the configuration object associated with this bin.
   *
   * @return A reference to the IBinConfig object that defines the configuration for this bin.
   */
  [[nodiscard]] const std::shared_ptr<const IBinConfig>& config() const;

  /**
   * @brief Checks if the specified pointer is within the memory range of the chunks managed by this bin.
   *
   * @param ptr The pointer to check.
   * @return true if the pointer is within the memory range of the chunks managed by this bin, false otherwise.
   */
  bool contains(void* ptr) const noexcept;

 private:
  /**
   * @brief Grows the bin by allocating a new chunk of memory and adding its blocks to the free list. This method is
   * called when an allocation request is made and the free list is empty. The new chunk is created based on the
   * configuration of the bin, and its blocks are added to the free list, making them available for future allocations.
   *
   * The growth process is thread-safe, allowing for concurrent growth of the bin from multiple threads without causing
   * data races or other synchronization issues.
   */
  void grow();

  /**
   * @brief Calculates the number of blocks to allocate for the next chunk based on the growth ratio defined in the
   * bin's configuration. The growth ratio is determined by the growth numerator and denominator, and can result in
   * logarithmic, linear, or exponential growth of the bin. The calculation takes into account the number of chunks
   * already allocated, and determines the appropriate number of blocks to allocate for the next chunk to efficiently
   * manage memory and handle allocation requests.
   *
   * @return The number of blocks to allocate for the next chunk when growing the bin.
   */
  std::size_t calculateGrowth() const noexcept;

  /**
   * @brief A mutex that protects access to the list of memory chunks managed by this bin.
   */
  mutable std::shared_mutex m_chunkMutex;

  /**
   * @brief A vector of memory chunks that are managed by this bin. Each chunk contains a contiguous block of memory
   * that is divided into blocks of a certain size, as defined by the bin's configuration. The chunks are allocated as
   * needed when the bin grows, and are used to satisfy allocation requests from the free list.
   */
  std::vector<Buffer> m_chunks;

  /**
   * @brief A pointer to the head of the intrusive freelist. Each free block stores a FreeNode at its start,
   * allowing the entire freelist to be maintained without external vector storage, improving cache locality
   * and reducing allocation overhead.
   */
  Block* m_free = nullptr;

  /**
   * @brief A shared pointer to the configuration object that defines the size and capacity of the blocks managed by
   * this bin.
   */
  const std::shared_ptr<const IBinConfig> m_config;
};

/**
 * @brief A memory pool that manages multiple bins of different block sizes. The MemoryPool class provides an interface
 * for allocating and deallocating memory blocks of various sizes, and internally manages the bins to efficiently handle
 * memory requests. The MemoryPool class allows for efficient memory management by grouping blocks of similar sizes into
 * bins, which can reduce fragmentation and improve allocation performance.
 *
 * The MemoryPool class also provides thread-safe allocation and deallocation of memory blocks, allowing for concurrent
 * access from multiple threads without causing data  races or other synchronization issues.
 */
class MemoryPool {
 public:
  /**
   * @brief Constructs a new MemoryPool object with the specified bin configurations. The constructor initializes the
   * internal state of the MemoryPool object, and sets up the bins based on the provided configurations. Each bin is
   * created using the corresponding configuration, and the MemoryPool manages the bins to handle memory allocation and
   * deallocation requests.
   *
   * @param bins A vector of unique pointers to IBinConfig objects that define the configurations for the bins managed
   * by this memory pool. Each configuration specifies the size and capacity of the blocks for a particular bin. The
   * MemoryPool will create a bin for each configuration provided in the vector, and will use these bins to manage
   * memory allocation and deallocation requests.
   * @throws std::invalid_argument If the provided vector of bin configurations is empty, or if any of the
   * configurations are invalid (e.g., if the block size is not a power of two, or if the number of blocks is zero).
   */
  explicit MemoryPool(std::vector<std::shared_ptr<const IBinConfig>>&& bins);

  /**
   * @brief Allocates a block of memory from the memory pool. After the given size is aligned, the smallest sized bin
   * configured for this memory pool that will fit the size is used for the allocation.
   *
   * If the requested size is larger than the largest block size configured for this memory pool, the allocation is
   * forwarded to the global heap allocator (e.g., using ::operator new). This allows the memory pool to handle
   * allocation requests of any size, while still providing efficient management for smaller blocks of memory.
   *
   * The allocation process is thread-safe, allowing for concurrent allocations from multiple threads without causing
   * data races or other synchronization issues.
   *
   * @param size The size of the memory block to allocate in bytes. This size will be aligned to the nearest multiple of
   * the smallest block size configured for this memory pool, and the allocation will be made from the appropriate bin
   * based on the aligned size. If the aligned size exceeds the largest block size configured for this memory pool, the
   * allocation will be forwarded to the global heap allocator.
   * @return
   */
  void* allocate(std::size_t size);

  /**
   * @brief Allocates a block of memory from the memory pool for an object of the specified type. The size of the memory
   * block to allocate is determined by the size of the type, and the allocation is made from the appropriate bin based
   * on the size. If the size of the type exceeds the largest block size configured for this memory pool, the allocation
   * is forwarded to the global heap allocator (e.g., using ::operator new).
   *
   * @param size The size of the memory block to allocate in bytes. This size will be determined by the size of the
   * specified type, and the allocation will be made from the appropriate bin based on the size.
   * @param alignment The alignment requirement for the memory block to allocate in bytes. This alignment will be used
   * to ensure that the allocated memory block meets the specified alignment requirements.
   * @return A pointer to the allocated memory block. The caller is responsible for ensuring that the allocated memory
   * is properly deallocated when it is no longer needed.
   */
  void* allocate(std::size_t size, std::size_t alignment);

  /**
   * @brief Allocates a block of memory from the memory pool for an object of the specified type. The size of the memory
   * block to allocate is determined by the size of the type, and the allocation is made from the appropriate bin based
   * on the size. If the size of the type exceeds the largest block size configured for this memory pool, the allocation
   * is forwarded to the global heap allocator (e.g., using ::operator new).
   *
   * @param type The RTTI type information for the object to allocate memory for. The size of the memory block to
   * allocate will be determined by the size of the type, and the allocation will be made from the appropriate bin based
   * on the size.
   * @return A pointer to the allocated memory block. The caller is responsible for ensuring that the allocated memory
   * is properly deallocated when it is no longer needed.
   */
  void* allocate(const rtti::IType* type);

  /**
   * @brief Frees a block of memory back to the memory pool. The provided pointer must have been previously allocated
   * from this memory pool, and must not have already been freed. The pointer is added back to the appropriate bin's
   * free list, making it available for future allocations. If the pointer was allocated from the global heap allocator
   * (e.g., using ::operator new), it will be deallocated using the global heap deallocator (e.g., using ::operator
   * delete).
   *
   * @param ptr The pointer to the block of memory to free. This pointer must have been previously allocated from this
   * memory pool, and must not have already been freed.
   * @param size The size of the memory block to free in bytes. This size will be used to determine which bin the block
   * belongs to, and to ensure that the block is properly deallocated.
   */
  void deallocate(void* ptr, std::size_t size);

 private:
  /**
   * @brief Finds the appropriate bin for the given size. The method searches through the bins managed by this memory
   * pool to find the smallest bin that can accommodate the requested size. If a suitable bin is found, a pointer to
   * that bin is returned. If no suitable bin is found (i.e., if the requested size exceeds the largest block size
   * configured for this memory pool), the method returns nullptr, indicating that the allocation should be forwarded to
   * the global heap allocator.
   *
   * @param size The size of the memory block to allocate in bytes. This size will be used to determine which bin can
   * accommodate the allocation request.
   * @return A pointer to the Bin object that can accommodate the allocation request, or nullptr if no suitable bin is
   * found.
   */
  Bin* findBin(std::size_t size);

  /**
   * @brief A vector of memory bins that are managed by this memory pool. Each bin is responsible for managing a
   * specific size of memory blocks, as defined by the bin's configuration.
   */
  std::vector<Bin> m_bins;

  /**
   * @brief The alignment requirement for memory blocks allocated from this memory pool. This alignment is determined by
   * the smallest block size configured for the bins in this memory pool, and is used to ensure that all allocated
   * memory blocks meet the necessary alignment requirements for efficient access and performance.
   */
  std::size_t m_alignment;
};

/**
 * @brief A default memory pool with a predefined set of bins for common block sizes. This memory pool is configured
 * with bins for block sizes of 8, 16, 32, 64, 128, 256, 512, and 1024 bytes, each with a capacity of 1024 blocks. This
 * default memory pool can be used for general-purpose memory allocation and deallocation, providing efficient
 * management for a wide range of block sizes commonly used in applications. The default memory pool is designed to
 * handle typical memory allocation patterns, and can help reduce fragmentation and improve performance for common use
 * cases.
 */
inline MemoryPool defaultMemoryPool({
    std::make_shared<const BinConfig<8>>(10000000),
    std::make_shared<const BinConfig<16>>(5000000),
    std::make_shared<const BinConfig<32>>(2500000),
    std::make_shared<const BinConfig<64>>(100000),
    std::make_shared<const BinConfig<128>>(50000),
    std::make_shared<const BinConfig<256>>(25000),
    std::make_shared<const BinConfig<512>>(10000),
    std::make_shared<const BinConfig<1024>>(5000),
});

}  // namespace core::memory
