#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <string_view>
#include <thread>

#include "Hash.hpp"
#include "NamePool.hpp"
#include "Vector.hpp"
#include "types/Name.hpp"

using core::Vector;

namespace {
using core::fnv1a_64;
using core::Name;
using core::NamePool;

std::string uniqueNameText(const char* prefix) {
  static std::atomic<std::size_t> counter{0};
  const std::size_t id = counter.fetch_add(1, std::memory_order_relaxed) + 1;
  return std::string(prefix) + "_" + std::to_string(id);
}

}  // namespace

TEST_CASE("Given NamePool::get, when called repeatedly, then the same singleton instance is returned", "[iname_pool]") {
  NamePool& first = NamePool::get();
  NamePool& second = NamePool::get();

  REQUIRE(&first == &second);
}

TEST_CASE("Given a fresh text value, when addName is called, then the name is interned and retrievable by Name",
          "[iname_pool]") {
  NamePool& pool = NamePool::get();
  const std::string text = uniqueNameText("pool_add");

  const Name name = pool.addName(text);

  REQUIRE(name == Name(text));
  REQUIRE(name.hash() == fnv1a_64(text));
  REQUIRE(pool.hasName(name));
  REQUIRE(pool.getName(name) == text);
}

TEST_CASE(
    "Given an interned text, when addName is called again with the same content, then the same hash identity is "
    "returned",
    "[iname_pool]") {
  NamePool& pool = NamePool::get();
  const std::string text = uniqueNameText("pool_duplicate");

  const Name first = pool.addName(text);
  const Name second = pool.addName(text);

  REQUIRE(first == second);
  REQUIRE(first.hash() == second.hash());
  REQUIRE(pool.getName(first) == text);
}

TEST_CASE(
    "Given a mutable source string, when addName is called, then the pool stores an owned copy independent of later "
    "source changes",
    "[iname_pool]") {
  NamePool& pool = NamePool::get();
  std::string source = uniqueNameText("pool_copy_semantics");
  const std::string expected = source;

  const Name name = pool.addName(std::string_view(source));

  source.assign("mutated_after_add");

  REQUIRE(pool.getName(name) == expected);
}

TEST_CASE(
    "Given an interned text, when accessed through operator[] overloads, then both Name and hash lookups return the "
    "same content",
    "[iname_pool]") {
  NamePool& pool = NamePool::get();
  const std::string text = uniqueNameText("pool_index_operator");

  const Name name = pool.addName(text);

  REQUIRE(pool[name] == text);
  REQUIRE(pool[name.hash()] == text);
}

TEST_CASE(
    "Given an interned name, when converted back through Name::toString and string_view conversion, then pooled text "
    "is returned",
    "[iname_pool]") {
  NamePool& pool = NamePool::get();
  const std::string text = uniqueNameText("pool_iname_to_string");

  const Name name = pool.addName(text);

  REQUIRE(name.toString() == text);
  REQUIRE(static_cast<std::string_view>(name) == text);
}

TEST_CASE("Given a name hash that was never interned, when queried with hasName, then false is returned",
          "[iname_pool][negative]") {
  const NamePool& pool = NamePool::get();
  const Name missing(uniqueNameText("pool_missing_name"));

  REQUIRE_FALSE(pool.hasName(missing));
}

TEST_CASE("Given an empty string, when addName is called, then it is interned and retrievable as an empty string",
          "[iname_pool][negative]") {
  NamePool& pool = NamePool::get();

  const Name name = pool.addName("");

  REQUIRE(pool.hasName(name));
  REQUIRE(pool.getName(name).empty());
}

TEST_CASE("Given many unique texts, when concurrent writers call addName, then all names are interned and retrievable",
          "[iname_pool][thread_safety]") {
  NamePool& pool = NamePool::get();
  constexpr int writerCount = 12;

  Vector<std::string> texts;
  texts.reserve(writerCount);
  for (int i = 0; i < writerCount; ++i) {
    texts.pushBack(uniqueNameText("pool_concurrent_unique"));
  }

  Vector<core::hash_t> hashes(writerCount, 0);
  Vector<std::thread> writers;
  writers.reserve(writerCount);

  for (int i = 0; i < writerCount; ++i) {
    writers.emplaceBack([&pool, &texts, &hashes, i]() {
      const Name name = pool.addName(texts[static_cast<std::size_t>(i)]);
      hashes[static_cast<std::size_t>(i)] = name.hash();
    });
  }

  for (auto& writer : writers) {
    writer.join();
  }

  for (int i = 0; i < writerCount; ++i) {
    const Name name(hashes[static_cast<std::size_t>(i)]);
    REQUIRE(pool.hasName(name));
    REQUIRE(pool.getName(name) == texts[static_cast<std::size_t>(i)]);
  }
}

TEST_CASE(
    "Given one shared text, when concurrent writers call addName, then all returned identities are equal and retrieval "
    "remains correct",
    "[iname_pool][thread_safety]") {
  NamePool& pool = NamePool::get();
  const std::string text = uniqueNameText("pool_concurrent_duplicate");
  constexpr int writerCount = 16;

  Vector<core::hash_t> hashes(writerCount, 0);
  Vector<std::thread> writers;
  writers.reserve(writerCount);

  for (int i = 0; i < writerCount; ++i) {
    writers.emplaceBack([&pool, &text, &hashes, i]() {
      const Name name = pool.addName(text);
      hashes[static_cast<std::size_t>(i)] = name.hash();
    });
  }

  for (auto& writer : writers) {
    writer.join();
  }

  const core::hash_t expectedHash = hashes.front();
  for (const core::hash_t hash : hashes) {
    REQUIRE(hash == expectedHash);
  }

  const Name shared(expectedHash);
  REQUIRE(pool.hasName(shared));
  REQUIRE(pool.getName(shared) == text);
}

TEST_CASE(
    "Given interned names, when many concurrent readers query hasName and getName, then observed values stay "
    "consistent",
    "[iname_pool][thread_safety]") {
  NamePool& pool = NamePool::get();

  const std::string aText = uniqueNameText("pool_concurrent_reader_a");
  const std::string bText = uniqueNameText("pool_concurrent_reader_b");
  const Name aName = pool.addName(aText);
  const Name bName = pool.addName(bText);

  constexpr int readerCount = 8;
  constexpr int iterationsPerReader = 5000;
  std::atomic inconsistent{false};

  Vector<std::thread> readers;
  readers.reserve(readerCount);

  for (int i = 0; i < readerCount; ++i) {
    readers.emplaceBack([&pool, aName, bName, &aText, &bText, &inconsistent]() {
      for (int j = 0; j < iterationsPerReader; ++j) {
        if (!pool.hasName(aName) || !pool.hasName(bName)) {
          inconsistent.store(true, std::memory_order_relaxed);
          break;
        }

        if (pool.getName(aName) != aText || pool.getName(bName) != bText) {
          inconsistent.store(true, std::memory_order_relaxed);
          break;
        }
      }
    });
  }

  for (auto& reader : readers) {
    reader.join();
  }

  REQUIRE_FALSE(inconsistent.load(std::memory_order_relaxed));
}
