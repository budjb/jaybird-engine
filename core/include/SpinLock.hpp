#pragma once
#include <atomic>

#include "Export.hpp"

namespace core {
/**
 * @brief A simple reader-writer spin lock supporting both exclusive and shared locking modes.
 *
 * This class satisfies @c BasicLockable (exclusive) and provides @c lock_shared / @c unlock_shared for shared
 * access, allowing multiple concurrent readers or a single exclusive writer.
 */
class JAYBIRD_API SpinLock {
 public:
  /**
   * @brief Acquires the lock exclusively, spinning until it becomes available.
   */
  void lock() noexcept;

  /**
   * @brief Releases the exclusive lock.
   */
  void unlock() noexcept;

  /**
   * @brief Attempts to acquire the lock exclusively without blocking.
   *
   * @return @c true if the lock was acquired, @c false if it is already held.
   */
  bool try_lock() noexcept;

  /**
   * @brief Acquires the lock in shared mode, allowing multiple concurrent readers.
   */
  void lock_shared() noexcept;

  /**
   * @brief Releases the shared lock.
   */
  void unlock_shared() noexcept;

  /**
   * @brief Attempts to acquire the lock in shared mode without blocking.
   *
   * @return @c true if the lock was acquired, @c false if it is held exclusively.
   */
  bool try_lock_shared() noexcept;

 private:
  /**
   * @brief Atomic state variable tracking the lock: 0 means unlocked, positive values represent
   * the active shared-reader count, and -1 means held exclusively.
   */
  std::atomic<int> m_locked = 0;
};
}  // namespace core
