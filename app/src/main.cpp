#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include "memory/Allocator.hpp"
#include "memory/MemoryArena.hpp"

namespace {
void runPoolMicroBenchmark(core::memory::MemoryPool& pool, const std::size_t iterations) {
  auto pointers = std::make_unique<void*[]>(iterations);

  // Warm up the allocator path to reduce first-use effects from skewing measurements.
  for (std::size_t i = 0; i < 10000; ++i) {
    void* ptr = pool.allocate(sizeof(int), alignof(int));
    pool.deallocate(ptr, sizeof(int), alignof(int));
  }

  const auto allocStart = std::chrono::steady_clock::now();
  for (std::size_t i = 0; i < iterations; ++i) {
    pointers[i] = pool.allocate(sizeof(int), alignof(int));
  }
  const auto allocEnd = std::chrono::steady_clock::now();

  const auto deallocStart = std::chrono::steady_clock::now();
  for (std::size_t i = 0; i < iterations; ++i) {
    pool.deallocate(pointers[i], sizeof(int), alignof(int));
  }
  const auto deallocEnd = std::chrono::steady_clock::now();

  const auto allocNs = std::chrono::duration_cast<std::chrono::nanoseconds>(allocEnd - allocStart).count();
  const auto deallocNs = std::chrono::duration_cast<std::chrono::nanoseconds>(deallocEnd - deallocStart).count();

  const auto allocPerOp = static_cast<double>(allocNs) / static_cast<double>(iterations);
  const auto deallocPerOp = static_cast<double>(deallocNs) / static_cast<double>(iterations);

  std::cout << "Pool micro-benchmark (" << iterations << " ops)\n";
  std::cout << "  allocate:   " << allocNs << " ns total (" << allocPerOp << " ns/op)\n";
  std::cout << "  deallocate: " << deallocNs << " ns total (" << deallocPerOp << " ns/op)\n";
}

void runPoolMixedSizeMicroBenchmark(core::memory::MemoryPool& pool, const std::size_t iterations) {
  constexpr std::size_t kSizes[] = {8, 16, 32, 64};
  constexpr std::size_t kSizeClassCount = std::size(kSizes);
  constexpr std::size_t kAlignment = alignof(void*);

  auto pointers = std::make_unique<void*[]>(iterations);

  for (std::size_t i = 0; i < 10000; ++i) {
    const auto size = kSizes[i % kSizeClassCount];
    void* ptr = pool.allocate(size, kAlignment);
    pool.deallocate(ptr, size, kAlignment);
  }

  const auto allocStart = std::chrono::steady_clock::now();
  for (std::size_t i = 0; i < iterations; ++i) {
    pointers[i] = pool.allocate(kSizes[i % kSizeClassCount], kAlignment);
  }
  const auto allocEnd = std::chrono::steady_clock::now();

  const auto deallocStart = std::chrono::steady_clock::now();
  for (std::size_t i = 0; i < iterations; ++i) {
    pool.deallocate(pointers[i], kSizes[i % kSizeClassCount], kAlignment);
  }
  const auto deallocEnd = std::chrono::steady_clock::now();

  const auto allocNs = std::chrono::duration_cast<std::chrono::nanoseconds>(allocEnd - allocStart).count();
  const auto deallocNs = std::chrono::duration_cast<std::chrono::nanoseconds>(deallocEnd - deallocStart).count();

  const auto allocPerOp = static_cast<double>(allocNs) / static_cast<double>(iterations);
  const auto deallocPerOp = static_cast<double>(deallocNs) / static_cast<double>(iterations);

  std::cout << "Pool mixed-size micro-benchmark (" << iterations << " ops; 8/16/32/64-byte round-robin)\n";
  std::cout << "  allocate:   " << allocNs << " ns total (" << allocPerOp << " ns/op)\n";
  std::cout << "  deallocate: " << deallocNs << " ns total (" << deallocPerOp << " ns/op)\n";
}
} // namespace

int main() {
  std::vector<uintptr_t> my_vector;

  my_vector.reserve(100000000);

  auto allocator = core::memory::PoolAllocator<int>(&core::memory::defaultMemoryPool);

  std::chrono::time_point start = std::chrono::steady_clock::now();
  for (int i = 0; i < 100000000; ++i) {
    void* ptr = allocator.allocate(1);
    my_vector.push_back(reinterpret_cast<uintptr_t>(ptr));
  }

  for (const uintptr_t ptr : my_vector) {
    allocator.deallocate(reinterpret_cast<int*>(ptr), 1);
  }

  std::chrono::time_point end = std::chrono::steady_clock::now();
  auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "Time taken: " << elapsed_ms << " ms\n";

  auto arena = core::memory::MemoryArena(100000000 * sizeof(int));

  start = std::chrono::steady_clock::now();
  for (int i = 0; i < 100000000; ++i) {
    if (arena.allocate(sizeof(int), alignof(int)) == nullptr) {
      std::cerr << "Memory arena out of memory at iteration " << i << "\n";
      break;
    }
  }
  arena.clear();
  end = std::chrono::steady_clock::now();
  elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "Time taken: " << elapsed_ms << " ms\n";

  runPoolMicroBenchmark(core::memory::defaultMemoryPool, 50000000);
  runPoolMixedSizeMicroBenchmark(core::memory::defaultMemoryPool, 50000000);

  return 0;
}
