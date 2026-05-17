#pragma once
#include <atomic>

namespace core {
class SpinLock {
 public:
  void lock() noexcept;
  void unlock() noexcept;
  bool try_lock() noexcept;
  void lock_shared() noexcept;
  void unlock_shared() noexcept;
  bool try_lock_shared() noexcept;

 private:
  std::atomic<int> m_locked = 0;
};
}  // namespace core
