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
IBinConfig::IBinConfig(const std::size_t blocks, const std::size_t blockSize, const std::size_t growthNumerator,
                       const std::size_t growthDenominator) noexcept
    : m_size(blockSize),
      m_capacity(blocks),
      m_growthNumerator(growthNumerator == 0 ? 1 : growthNumerator),
      m_growthDenominator(growthDenominator == 0 ? 1 : growthDenominator) {}

std::size_t IBinConfig::size() const noexcept {
  return m_size;
}

std::size_t IBinConfig::capacity() const noexcept {
  return m_capacity;
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

const std::shared_ptr<const IBinConfig>& Bin::config() const {
  return m_config;
}

bool Bin::contains(void* ptr) const noexcept {
  return std::ranges::any_of(m_chunks.begin(), m_chunks.end(),
                             [ptr](const auto& chunk) { return chunk.contains(ptr); });
}

void Bin::grow() {
  std::unique_lock lock(m_chunkMutex);

  const auto count = calculateGrowth();
  const auto size = m_config->size();

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
  const auto numerator = m_config->growthNumerator();
  const auto denominator = m_config->growthDenominator();

  if (numerator == denominator) {
    return m_config->capacity();
  }

  const auto nextChunkIndex = m_chunks.size() + 1;
  const auto capacity = static_cast<long double>(m_config->capacity());
  const auto ratio = static_cast<long double>(numerator) / static_cast<long double>(denominator);

  if (numerator < denominator) {
    const auto scaled = std::log2(static_cast<long double>(nextChunkIndex) + 1.0L) * ratio * capacity;
    return std::max<std::size_t>(1, static_cast<std::size_t>(std::ceil(scaled)));
  }

  const auto scaled = std::pow(ratio, static_cast<long double>(nextChunkIndex - 1)) * capacity;
  return std::max<std::size_t>(1, static_cast<std::size_t>(std::ceil(scaled)));
}

MemoryPool::MemoryPool(std::vector<std::shared_ptr<const IBinConfig>>&& bins) {
  std::ranges::sort(bins, [](const std::shared_ptr<const IBinConfig>& a, const std::shared_ptr<const IBinConfig>& b) {
    return a->size() < b->size();
  });

  for (auto& bin : bins) {
    m_bins.emplace_back(bin);
  }

  m_alignment = m_bins.front().config()->size();
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
                                           [](const Bin& bin) { return bin.config()->size(); });

  if (it != m_bins.end()) {
    return &*it;
  }

  return nullptr;
}

}  // namespace core::memory
