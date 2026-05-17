#include "memory/MemoryPool.hpp"

#include <algorithm>
#include <any>
#include <cassert>
#include <format>
#include <iostream>
#include <utility>

namespace {
std::size_t alignUp(const std::size_t size, const std::size_t alignment) {
  return size + alignment - 1 & ~(alignment - 1);
}
}  // namespace

namespace core::memory {
IBinConfig::IBinConfig(const std::size_t blocks, const std::size_t blockSize) noexcept
    : m_size(blockSize), m_capacity(blocks) {}

std::size_t IBinConfig::size() const noexcept {
  return m_size;
}

std::size_t IBinConfig::capacity() const noexcept {
  return m_capacity;
}

Chunk::Chunk(const std::size_t blocks, const std::size_t blockSize) noexcept
    : m_blocks(blocks), m_blockSize(blockSize), m_buffer(blocks * blockSize) {}

Chunk::Chunk(Chunk&& other) noexcept
    : m_blocks(other.m_blocks),
      m_blockSize(other.m_blockSize),
      m_buffer(std::move(const_cast<Buffer&>(other.m_buffer))) {}

void* Chunk::get(const std::size_t index) const noexcept {
#ifndef NDEBUG
  assert(index < m_blocks && "core::memory:Chunk::get(): index out of range");
#endif
  return m_buffer.at(index * m_blockSize);
}

void* Chunk::get() const noexcept {
  return m_buffer.at(0);
}

std::size_t Chunk::blocks() const noexcept {
  return m_blocks;
}

std::size_t Chunk::blockSize() const noexcept {
  return m_blockSize;
}

bool Chunk::contains(void* ptr) const noexcept {
  return m_buffer.contains(ptr);
}

Bin::Bin(const std::shared_ptr<const IBinConfig>& config) noexcept : m_config(config) {
#ifndef NDEBUG
  assert(m_config->size() > MINIMUM_ALIGNMENT && (m_config->size() & m_config->size() - 1) == 0 &&
         "core::memory:Bin::Bin(): block size must be a power of two and greater than the size of a byte");
#endif
  grow();
}

Bin::Bin(Bin&& other) noexcept
    : m_chunks(std::move(other.m_chunks)), m_free(std::exchange(other.m_free, nullptr)), m_config(other.m_config) {}

void* Bin::allocate() noexcept {
  if (!m_free) {
    grow();
  }

  auto* node = m_free;
  m_free = m_free->next;
  return node;
}

void Bin::deallocate(void* ptr) noexcept {
  auto* node = static_cast<Block*>(ptr);
  node->next = m_free;
  m_free = node;
}

const IBinConfig& Bin::config() const {
  return *m_config;
}

bool Bin::contains(void* ptr) const noexcept {
  return std::ranges::any_of(m_chunks, [ptr](const Chunk& chunk) { return chunk.contains(ptr); });
}

void Bin::grow() {
  std::unique_lock lock(m_chunkMutex);

  const auto count = m_config->capacity();
  const auto size = m_config->size();

  auto chunk = Chunk(count, size);

  auto* memory = static_cast<std::byte*>(chunk.get());

  for (std::size_t i = 0; i < count; ++i) {
    auto* node = reinterpret_cast<Block*>(memory + i * size);
    node->next = m_free;
    m_free = node;
  }

  m_chunks.push_back(std::move(chunk));
}

MemoryPool::MemoryPool(std::vector<std::shared_ptr<const IBinConfig>>&& bins) {
  std::ranges::sort(bins, [](const std::shared_ptr<const IBinConfig>& a, const std::shared_ptr<const IBinConfig>& b) {
    return a->size() < b->size();
  });

  for (auto& bin : bins) {
    m_bins.emplace_back(bin);
  }

  m_alignment = m_bins.front().config().size();
}

void* MemoryPool::allocate(const std::size_t size) {
  return allocate(size, m_alignment);
}

void* MemoryPool::allocate(const std::size_t size, const std::size_t alignment) {
  const auto aligned = alignUp(size, alignment);

  if (auto* bin = findBin(aligned)) {
    return bin->allocate();
  }

  return operator new(alignUp(size, alignment));
}

void* MemoryPool::allocate(const rtti::IType* type) {
  return allocate(type->size(), type->alignment());
}

void MemoryPool::deallocate(void* ptr, const std::size_t size) {
  const auto aligned = alignUp(size, m_alignment);

  if (auto* bin = findBin(aligned)) {
    bin->deallocate(ptr);
  } else {
    operator delete(ptr);
  }
}

Bin* MemoryPool::findBin(const std::size_t size) {
  const auto it = std::ranges::lower_bound(m_bins.begin(), m_bins.end(), size, std::less(),
                                           [](const Bin& bin) { return bin.config().size(); });

  if (it != m_bins.end()) {
    return &*it;
  }

  return nullptr;
}

}  // namespace core::memory
