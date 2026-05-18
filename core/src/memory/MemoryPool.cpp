#include "memory/MemoryPool.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

namespace {
std::size_t alignUp(const std::size_t size, const std::size_t alignment) {
  return size + alignment - 1 & ~(alignment - 1);
}
}  // namespace

namespace core::memory {
BinConfig::BinConfig(const std::size_t blocks, const std::size_t blockSize, const std::size_t growthNumerator,
                     const std::size_t growthDenominator) noexcept
    : size(blockSize),
      capacity(blocks),
      growthNumerator(growthNumerator == 0 ? 1 : growthNumerator),
      growthDenominator(growthDenominator == 0 ? 1 : growthDenominator) {}

Bin::Bin(const BinConfig& config) noexcept : m_config(config) {
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

const BinConfig& Bin::config() const noexcept {
  return m_config;
}

bool Bin::contains(void* ptr) const noexcept {
  return std::ranges::any_of(m_chunks.begin(), m_chunks.end(),
                             [ptr](const auto& chunk) { return chunk.contains(ptr); });
}

void Bin::grow() {
  std::unique_lock lock(m_chunkMutex);

  const auto count = calculateGrowth();
  const auto size = m_config.size;

  if (count > std::numeric_limits<std::size_t>::max() / size) {
    throw std::overflow_error("core::memory:Bin::grow(): buffer size overflow");
  }

  auto chunk = Buffer(size * count);

  auto* memory = static_cast<std::byte*>(chunk.at(0));

  for (std::size_t i = 0; i < count; ++i) {
    auto* node = reinterpret_cast<Block*>(memory);
    node->next = m_free;
    m_free = node;
    memory += size;
  }

  m_chunks.push_back(std::move(chunk));
}

std::size_t Bin::calculateGrowth() const noexcept {
  const auto numerator = m_config.growthNumerator;
  const auto denominator = m_config.growthDenominator;

  if (numerator == denominator) {
    return m_config.capacity;
  }

  const auto nextChunkIndex = m_chunks.size() + 1;
  const auto capacity = static_cast<long double>(m_config.capacity);
  const auto ratio = static_cast<long double>(numerator) / static_cast<long double>(denominator);

  if (numerator < denominator) {
    const auto scaled = std::log2(static_cast<long double>(nextChunkIndex) + 1.0L) * ratio * capacity;
    return std::max<std::size_t>(1, static_cast<std::size_t>(std::ceil(scaled)));
  }

  const auto scaled = std::pow(ratio, static_cast<long double>(nextChunkIndex - 1)) * capacity;
  return std::max<std::size_t>(1, static_cast<std::size_t>(std::ceil(scaled)));
}

MemoryPool::MemoryPool(std::vector<BinConfig>&& bins) {
  if (bins.empty()) {
    throw std::invalid_argument("core::memory:MemoryPool::MemoryPool(): bins cannot be empty");
  }

  std::ranges::sort(bins, [](const BinConfig& a, const BinConfig& b) { return a.size < b.size; });

  for (auto& bin : bins) {
    m_bins.emplace_back(bin);
  }

  m_alignment = m_bins.front().config().size;
}

void* MemoryPool::allocate(const std::size_t size) {
  return allocate(size, m_alignment);
}

void* MemoryPool::allocate(const std::size_t size, const std::size_t alignment) {
  const auto requestedAlignment = std::max<std::size_t>(1, alignment);
  const auto aligned = alignUp(size, requestedAlignment);

  if (auto* bin = findBin(aligned, requestedAlignment)) {
    return bin->allocate();
  }

  if (requestedAlignment > alignof(std::max_align_t)) {
    return operator new(aligned, static_cast<std::align_val_t>(requestedAlignment));
  }

  return operator new(aligned);
}

void* MemoryPool::allocate(const rtti::IType* type) {
  return allocate(type->size(), type->alignment());
}

void MemoryPool::deallocate(void* ptr, const std::size_t size) {
  deallocate(ptr, size, m_alignment);
}

void MemoryPool::deallocate(void* ptr, const std::size_t size, const std::size_t alignment) {
  if (!ptr) {
    return;
  }

  const auto requestedAlignment = std::max<std::size_t>(1, alignment);
  const auto aligned = alignUp(size, requestedAlignment);

  if (auto* bin = findBin(aligned, requestedAlignment)) {
    bin->deallocate(ptr);
    return;
  }

  if (requestedAlignment > alignof(std::max_align_t)) {
    operator delete(ptr, static_cast<std::align_val_t>(requestedAlignment));
    return;
  }

  operator delete(ptr);
}

Bin* MemoryPool::findBin(const std::size_t size, const std::size_t alignment) noexcept {
  if (alignment > alignof(std::max_align_t)) {
    return nullptr;
  }

  // Bin sizes are power-of-two values. For power-of-two alignment, divisibility
  // reduces to selecting a bin whose size is >= both requested size and alignment.
  const auto requiredSize = std::max(size, alignment);
  for (auto& bin : m_bins) {
    if (bin.config().size >= requiredSize) {
      return &bin;
    }
  }

  return nullptr;
}
}  // namespace core::memory
