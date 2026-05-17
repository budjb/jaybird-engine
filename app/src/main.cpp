#include <iostream>

#include "memory/Allocator.hpp"
#include "memory/MemoryArena.hpp"

int main() {
  std::vector<uintptr_t> my_vector;

  my_vector.reserve(100000000);

  auto allocator = core::memory::Allocator<int>(&core::memory::defaultMemoryPool);

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

  return 0;
}
