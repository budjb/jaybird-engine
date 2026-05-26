#pragma once
#include <atomic>

#include "Export.hpp"

namespace core {
/**
 * @brief A simple spin lock implementation that provides basic locking functionality for synchronizing access to shared
 * resources in a multithreaded environment.
 */
class JAYBIRD_API SpinLock {
 public:
  /**
   * @brief Acquires the lock by spinning until the lock is successfully acquired. This function will block the calling
   * thread until the lock is available.
   *
   * If the lock is already held by another thread, the calling thread will continuously check the lock variable until
   * it becomes available, at which point it will acquire the lock and proceed with its execution.
   */
  void lock() noexcept;

  /**
   * @brief Releases the lock by setting the lock variable to indicate that the lock is now available. This function
   * should be called by the thread that currently holds the lock to allow other threads to acquire it.
   *
   * After calling this function, the lock will be available for other threads to acquire, and the calling thread will
   * no longer have exclusive access to the shared resource protected by the lock.
   */
  void unlock() noexcept;

  /**
   * @brief Attempts to acquire the lock without blocking. This function will check if the lock is currently available
   * and, if so, it will acquire the lock and return true.
   *
   * If the lock is already held by another thread, this function will return false immediately without blocking the
   * calling thread.
   *
   * @return true if the lock was successfully acquired, false if the lock is already held by another thread.
   */
  bool try_lock() noexcept;

  /**
   * @brief Acquires the lock in shared mode, allowing multiple threads to acquire the lock simultaneously for read-only
   * access to the shared resource. This function will block the calling thread until the lock is available for shared
   * access.
   */
  void lock_shared() noexcept;

  /**
   * @brief Releases the lock from shared mode, allowing other threads to acquire the lock for exclusive access. This
   * function should be called by threads that currently hold the lock in shared mode to allow other threads to acquire
   * it for exclusive access.
   */
  void unlock_shared() noexcept;

  /**
   * @brief Attempts to acquire the lock in shared mode without blocking. This function will check if the lock is
   * currently available for shared access and, if so, it will acquire the lock in shared mode and return true.
   *
   * @return true if the lock was successfully acquired in shared mode, false if the lock is already held by another
   * thread for exclusive access.
   */
  bool try_lock_shared() noexcept;

 private:
  /**
   * @brief An atomic integer variable that represents the state of the lock. A value of 0 indicates that the lock is
   * available, while a value of 1 indicates that the lock is currently held by a thread. This variable is used to
   * implement the spin lock mechanism, allowing threads to check and update the lock state atomically without the need
   * for more complex synchronization primitives. The atomic nature of this variable ensures that multiple threads can
   * safely access and modify the lock state without causing race conditions or undefined behavior.
   */
  std::atomic<int> m_locked = 0;
};
}  // namespace core
