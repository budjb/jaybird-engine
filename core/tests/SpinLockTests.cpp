#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <mutex>
#include <thread>

#include "SpinLock.hpp"
#include "Vector.hpp"

using core::Vector;

TEST_CASE("Given an unlocked SpinLock, when lock and try_lock are used, then exclusive ownership is enforced",
          "[spinlock]") {
  core::SpinLock lock;

  lock.lock();
  REQUIRE_FALSE(lock.try_lock());

  lock.unlock();
  REQUIRE(lock.try_lock());
  lock.unlock();
}

TEST_CASE(
    "Given a SpinLock held in shared mode, when try_lock is called for exclusive access, then exclusive acquisition "
    "fails until readers release",
    "[spinlock]") {
  core::SpinLock lock;

  lock.lock_shared();
  REQUIRE_FALSE(lock.try_lock());

  lock.unlock_shared();
  REQUIRE(lock.try_lock());
  lock.unlock();
}

TEST_CASE(
    "Given a SpinLock held exclusively, when try_lock_shared is called, then shared acquisition fails until exclusive "
    "ownership is released",
    "[spinlock]") {
  core::SpinLock lock;

  lock.lock();
  REQUIRE_FALSE(lock.try_lock_shared());

  lock.unlock();
  REQUIRE(lock.try_lock_shared());
  lock.unlock_shared();
}

TEST_CASE(
    "Given multiple threads taking shared locks, when they synchronize at the same point, then the lock allows "
    "concurrent readers",
    "[spinlock][thread_safety]") {
  core::SpinLock lock;
  constexpr int readerCount = 6;

  std::atomic activeReaders{0};
  std::atomic maxReaders{0};
  std::atomic releaseReaders{false};
  Vector<std::thread> readers;
  readers.reserve(readerCount);

  for (int i = 0; i < readerCount; ++i) {
    readers.emplaceBack([&]() {
      lock.lock_shared();

      const int current = activeReaders.fetch_add(1, std::memory_order_relaxed) + 1;
      int observedMax = maxReaders.load(std::memory_order_relaxed);
      while (current > observedMax &&
             !maxReaders.compare_exchange_weak(observedMax, current, std::memory_order_relaxed)) {
      }

      while (!releaseReaders.load(std::memory_order_acquire)) {
        std::this_thread::yield();
      }

      activeReaders.fetch_sub(1, std::memory_order_relaxed);
      lock.unlock_shared();
    });
  }

  while (activeReaders.load(std::memory_order_acquire) < readerCount) {
    std::this_thread::yield();
  }

  releaseReaders.store(true, std::memory_order_release);

  for (auto& reader : readers) {
    reader.join();
  }

  REQUIRE(maxReaders.load(std::memory_order_relaxed) > 1);
}

TEST_CASE(
    "Given multiple writer threads using a SpinLock, when each thread increments a shared counter, then all increments "
    "are preserved",
    "[spinlock][thread_safety]") {
  core::SpinLock lock;
  constexpr int writerCount = 8;
  constexpr int incrementsPerWriter = 2000;

  int sharedCounter = 0;
  Vector<std::thread> writers;
  writers.reserve(writerCount);

  for (int i = 0; i < writerCount; ++i) {
    writers.emplaceBack([&]() {
      for (int j = 0; j < incrementsPerWriter; ++j) {
        std::lock_guard guard(lock);
        ++sharedCounter;
      }
    });
  }

  for (auto& writer : writers) {
    writer.join();
  }

  REQUIRE(sharedCounter == writerCount * incrementsPerWriter);
}
