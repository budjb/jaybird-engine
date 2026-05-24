#include <algorithm>
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <new>
#include <string>
#include <thread>
#include <vector>

#include "rtti/TypeRegistry.hpp"
#include "rtti/types/IntType.hpp"

namespace {
using core::IName;
using core::rtti::IClassType;
using core::rtti::IType;
using core::rtti::TypeKind;
using core::rtti::TypeRegistry;

class StubClassType final : public IClassType {
 public:
  explicit StubClassType(const IName& name) : IClassType(name, sizeof(int), alignof(int)) {}

  void assign(void* dst, const void* src) override {
    if (!dst || !src) {
      return;
    }
    *static_cast<int*>(dst) = *static_cast<const int*>(src);
  }

  void* allocate() noexcept override {
    return operator new(sizeof(int), static_cast<std::align_val_t>(alignof(int)));
  }

  void deallocate(void* memory) noexcept override {
    if (memory) {
      operator delete(memory, sizeof(int), static_cast<std::align_val_t>(alignof(int)));
    }
  }

  void construct(void* memory) noexcept override {
    if (memory) {
      std::construct_at<int>(static_cast<int*>(memory));
    }
  }

  void destruct(void* memory) noexcept override {
    if (memory) {
      std::destroy_at<int>(static_cast<int*>(memory));
    }
  }

  void* create() override {
    void* memory = allocate();
    if (memory) {
      construct(memory);
    }
    return memory;
  }

  void destroy(void* memory) override {
    if (memory) {
      destruct(memory);
      deallocate(memory);
    }
  }

  bool equals(const void* lhs, const void* rhs) const noexcept override {
    if (!lhs || !rhs) {
      return lhs == rhs;
    }
    return *static_cast<const int*>(lhs) == *static_cast<const int*>(rhs);
  }
};

IName uniqueName(const char* prefix) {
  static std::atomic<std::size_t> counter{0};
  const std::size_t id = counter.fetch_add(1, std::memory_order_relaxed) + 1;
  return std::string(prefix) + "_" + std::to_string(id);
}

}  // namespace

TEST_CASE("Given TypeRegistry::get, when called repeatedly, then the same singleton instance is returned",
          "[rtti][type_registry]") {
  TypeRegistry* first = TypeRegistry::get();
  TypeRegistry* second = TypeRegistry::get();

  REQUIRE(first != nullptr);
  REQUIRE(second != nullptr);
  REQUIRE(first == second);
}

TEST_CASE("Given an unregistered type name, when queried in TypeRegistry, then retrieval APIs return missing results",
          "[rtti][type_registry]") {
  const TypeRegistry* registry = TypeRegistry::get();
  const IName missingName = uniqueName("missing_type");

  REQUIRE_FALSE(registry->hasType(missingName));
  REQUIRE(registry->getType(missingName) == nullptr);
  REQUIRE(registry->getClass(missingName) == nullptr);
}

TEST_CASE(
    "Given a non-class type, when registered in TypeRegistry, then hasType/getType succeed and getClass returns null",
    "[rtti][type_registry]") {
  TypeRegistry* registry = TypeRegistry::get();
  const IName typeName = uniqueName("int_like");

  auto type = std::make_unique<core::rtti::IntType>(typeName);
  IType* expected = type.get();

  REQUIRE(registry->registerType(std::move(type)));
  REQUIRE(registry->hasType(typeName));
  REQUIRE(registry->getType(typeName) == expected);
  REQUIRE(registry->getType(typeName)->kind() == TypeKind::FUNDAMENTAL);
  REQUIRE(registry->getClass(typeName) == nullptr);
}

TEST_CASE("Given a duplicate type name, when registering a second type in TypeRegistry, then registration is rejected",
          "[rtti][type_registry]") {
  TypeRegistry* registry = TypeRegistry::get();
  const IName duplicatedName = uniqueName("duplicated_type");

  auto first = std::make_unique<core::rtti::IntType>(duplicatedName);
  IType* firstPtr = first.get();
  REQUIRE(registry->registerType(std::move(first)));

  auto duplicate = std::make_unique<core::rtti::IntType>(duplicatedName);
  REQUIRE_FALSE(registry->registerType(std::move(duplicate)));

  REQUIRE(registry->hasType(duplicatedName));
  REQUIRE(registry->getType(duplicatedName) == firstPtr);
}

TEST_CASE(
    "Given a class type, when registered in TypeRegistry, then getType and getClass both return the stored descriptor",
    "[rtti][type_registry]") {
  TypeRegistry* registry = TypeRegistry::get();
  const IName className = uniqueName("stub_class");

  auto classType = std::make_unique<StubClassType>(className);
  IClassType* expectedClass = classType.get();
  IType* expectedType = expectedClass;

  REQUIRE(registry->registerType(std::move(classType)));

  IType* retrievedType = registry->getType(className);
  IClassType* retrievedClass = registry->getClass(className);

  REQUIRE(retrievedType == expectedType);
  REQUIRE(retrievedType->kind() == TypeKind::CLASS);
  REQUIRE(retrievedClass == expectedClass);
}

TEST_CASE(
    "Given an equivalent IName value for a registered type, when looked up in TypeRegistry, then the descriptor is "
    "found",
    "[rtti][type_registry]") {
  TypeRegistry* registry = TypeRegistry::get();
  const IName registeredName = uniqueName("lookup_equivalent");

  auto type = std::make_unique<core::rtti::IntType>(registeredName);
  IType* expected = type.get();
  REQUIRE(registry->registerType(std::move(type)));

  const IName equivalentLookupName(registeredName.hash());

  REQUIRE(registry->hasType(equivalentLookupName));
  REQUIRE(registry->getType(equivalentLookupName) == expected);
}

TEST_CASE(
    "Given an existing simple type, when a class type with the same name is registered, then registration fails and "
    "the original descriptor remains",
    "[rtti][type_registry][negative]") {
  TypeRegistry* registry = TypeRegistry::get();
  const IName duplicatedName = uniqueName("duplicate_kind");

  auto simpleType = std::make_unique<core::rtti::IntType>(duplicatedName);
  IType* expected = simpleType.get();
  REQUIRE(registry->registerType(std::move(simpleType)));

  auto duplicateClassType = std::make_unique<StubClassType>(duplicatedName);
  REQUIRE_FALSE(registry->registerType(std::move(duplicateClassType)));

  REQUIRE(registry->getType(duplicatedName) == expected);
  REQUIRE(registry->getType(duplicatedName)->kind() == TypeKind::FUNDAMENTAL);
  REQUIRE(registry->getClass(duplicatedName) == nullptr);
}

TEST_CASE(
    "Given a registered type and an unrelated lookup name, when queried in TypeRegistry, then no accidental match is "
    "returned",
    "[rtti][type_registry][negative]") {
  const TypeRegistry* registry = TypeRegistry::get();
  const IName registeredName = uniqueName("negative_lookup_registered");
  const IName otherName = uniqueName("negative_lookup_other");

  auto type = std::make_unique<core::rtti::IntType>(registeredName);
  IType* expected = type.get();
  REQUIRE(TypeRegistry::get()->registerType(std::move(type)));

  REQUIRE(registry->getType(registeredName) == expected);
  REQUIRE_FALSE(registry->hasType(otherName));
  REQUIRE(registry->getType(otherName) == nullptr);
  REQUIRE(registry->getClass(otherName) == nullptr);
}

TEST_CASE(
    "Given many unique type names, when concurrent writers register through TypeRegistry, then all registrations "
    "succeed and remain queryable",
    "[rtti][type_registry][thread_safety]") {
  TypeRegistry* registry = TypeRegistry::get();
  constexpr int writerCount = 12;

  const std::string base = "concurrent_unique_register_" + std::to_string(uniqueName("seed").hash());
  std::vector<std::string> names;
  names.reserve(writerCount);
  for (int i = 0; i < writerCount; ++i) {
    names.push_back(base + "_" + std::to_string(i));
  }

  std::vector successes(writerCount, 0);
  std::vector<std::thread> threads;
  threads.reserve(writerCount);

  for (int i = 0; i < writerCount; ++i) {
    threads.emplace_back([registry, &names, &successes, i]() {
      auto type = std::make_unique<core::rtti::IntType>(IName(names[static_cast<std::size_t>(i)]));
      successes[static_cast<std::size_t>(i)] = registry->registerType(std::move(type)) ? 1 : 0;
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  for (int i = 0; i < writerCount; ++i) {
    REQUIRE(successes[static_cast<std::size_t>(i)] == 1);
    const IName name(names[static_cast<std::size_t>(i)]);
    REQUIRE(registry->hasType(name));
    REQUIRE(registry->getType(name) != nullptr);
  }
}

TEST_CASE(
    "Given many concurrent duplicate registrations, when all writers use one type name in TypeRegistry, then exactly "
    "one registration succeeds",
    "[rtti][type_registry][thread_safety]") {
  TypeRegistry* registry = TypeRegistry::get();
  const IName sharedName = uniqueName("concurrent_duplicate_register");
  constexpr int writerCount = 16;

  std::vector successes(writerCount, 0);
  std::vector<std::thread> threads;
  threads.reserve(writerCount);

  for (int i = 0; i < writerCount; ++i) {
    threads.emplace_back([registry, &successes, sharedName, i]() {
      auto type = std::make_unique<core::rtti::IntType>(sharedName);
      successes[static_cast<std::size_t>(i)] = registry->registerType(std::move(type)) ? 1 : 0;
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  const int successCount = static_cast<int>(std::ranges::count(successes, 1));
  REQUIRE(successCount == 1);
  REQUIRE(registry->hasType(sharedName));
  REQUIRE(registry->getType(sharedName) != nullptr);
}

TEST_CASE(
    "Given a registered type, when many concurrent readers repeatedly query TypeRegistry, then all reads remain "
    "consistent",
    "[rtti][type_registry][thread_safety]") {
  TypeRegistry* registry = TypeRegistry::get();
  const IName name = uniqueName("concurrent_read");

  auto type = std::make_unique<core::rtti::IntType>(name);
  IType* expected = type.get();
  REQUIRE(registry->registerType(std::move(type)));

  constexpr int readerCount = 8;
  constexpr int iterationsPerReader = 5000;
  std::atomic inconsistentRead{false};

  std::vector<std::thread> readers;
  readers.reserve(readerCount);

  for (int i = 0; i < readerCount; ++i) {
    readers.emplace_back([registry, name, expected, &inconsistentRead]() {
      for (int j = 0; j < iterationsPerReader; ++j) {
        if (!registry->hasType(name)) {
          inconsistentRead.store(true, std::memory_order_relaxed);
          break;
        }

        if (const IType* actual = registry->getType(name); actual != expected) {
          inconsistentRead.store(true, std::memory_order_relaxed);
          break;
        }

        if (registry->getClass(name) != nullptr) {
          inconsistentRead.store(true, std::memory_order_relaxed);
          break;
        }
      }
    });
  }

  for (auto& reader : readers) {
    reader.join();
  }

  REQUIRE_FALSE(inconsistentRead.load(std::memory_order_relaxed));
}
