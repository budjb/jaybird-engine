#include "SpinLock.hpp"

namespace core {
void SpinLock::lock() noexcept {
  int expected = 0;
  while (!m_locked.compare_exchange_weak(expected, -1, std::memory_order_acquire, std::memory_order_relaxed)) {
    expected = 0;
    m_locked.wait(m_locked.load(std::memory_order_relaxed), std::memory_order_relaxed);
  }
}

void SpinLock::unlock() noexcept {
  m_locked.store(0, std::memory_order_release);
  m_locked.notify_all();
}

bool SpinLock::try_lock() noexcept {
  int expected = 0;
  return m_locked.compare_exchange_strong(expected, -1, std::memory_order_acquire, std::memory_order_relaxed);
}

void SpinLock::lock_shared() noexcept {
  int current = m_locked.load(std::memory_order_relaxed);
  while (true) {
    if (current == -1) {
      m_locked.wait(-1, std::memory_order_relaxed);
      current = m_locked.load(std::memory_order_relaxed);
      continue;
    }
    if (m_locked.compare_exchange_weak(current, current + 1, std::memory_order_acquire, std::memory_order_relaxed)) {
      break;
    }
  }
}
void SpinLock::unlock_shared() noexcept {
  if (m_locked.fetch_sub(1, std::memory_order_release) == 1) {
    m_locked.notify_all();
  }
}
bool SpinLock::try_lock_shared() noexcept {
  int current = m_locked.load(std::memory_order_relaxed);
  if (current == -1) {
    return false;
  }
  return m_locked.compare_exchange_strong(current, current + 1, std::memory_order_acquire, std::memory_order_relaxed);
}
}  // namespace core
