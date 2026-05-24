# jaybird-engine — Agent Context

> This file is the authoritative context document for AI agents working on this codebase.
> Keep it up to date as the project evolves. Sections marked **[PLACEHOLDER]** need owner input.

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Architecture](#2-architecture)
3. [Directory Structure](#3-directory-structure)
4. [Core Subsystems](#4-core-subsystems)
5. [Naming Conventions & Style](#5-naming-conventions--style)
6. [Build System](#6-build-system)
7. [Testing](#7-testing)
8. [Key Design Patterns](#8-key-design-patterns)
9. [What Exists vs. What Is Planned](#9-what-exists-vs-what-is-planned)
10. [Important Constraints & Gotchas](#10-important-constraints--gotchas)
11. [Placeholders Needing Owner Input](#11-placeholders-needing-owner-input)

---

## 1. Project Overview

**jaybird-engine** is a C++23 game engine (or engine foundation) that serves as a playground and learning project.

The engine targets **Windows** and is built with **CMake 4.2** and **C++23**. The target compiler is clang (clang-cl on
Windows), but it should be compatible with MSVC as well (except for coverage builds, which require Clang).

The primary target architecture of the project is Windows x86_64, but the codebase is written with cross-platform
compatibility in mind where possible.

---

## 2. Architecture

```
jaybird-engine/
├── core/          ← Static library: jaybird-engine-core
│   ├── include/   ← Public headers (added to target_include_directories PUBLIC)
│   └── src/       ← Implementation files
├── tests/         ← Catch2 test executable: core_tests
│   └── *.cpp
└── CMakeLists.txt ← Root; controls C++23 standard, CTest, coverage flag
```

The `core` library is a **static library** (`jaybird-engine-core`). All public API lives under
`core/include/`. Tests are a separate CMake target that links `jaybird-engine-core` and
`Catch2::Catch2WithMain`.

---

## 3. Directory Structure

### `core/include/`

| File / Folder             | Purpose                                                                                          |
|---------------------------|--------------------------------------------------------------------------------------------------|
| `Hash.hpp`                | `core::hash_t` (`uint64_t`) and `constexpr fnv1a_64()` hash function                             |
| `IName.hpp`               | `core::IName` — interned string handle (stores FNV-1a hash, resolves via `INamePool`)            |
| `INamePool.hpp`           | `core::INamePool` — singleton map from `hash_t → std::string`, thread-safe with `shared_mutex`   |
| `SpinLock.hpp`            | `core::SpinLock` — reader-writer spin lock (supports both exclusive and shared locking)          |
| `rtti/RTTI.hpp`           | Convenience umbrella header — includes all RTTI public headers                                   |
| `rtti/TypeKind.hpp`       | `enum class TypeKind` — `NAME, CLASS, ARRAY, STRING, SIMPLE`                                     |
| `rtti/IType.hpp`          | Abstract base for all type descriptors                                                           |
| `rtti/TType.hpp`          | CRTP helper: `TType<T>` implements `IType` for concrete non-container types                      |
| `rtti/IContainerType.hpp` | `IContainerType : IType` — adds `inner()` pointer to element type                                |
| `rtti/IArrayType.hpp`     | `IArrayType : IContainerType` — full dynamic-array API (at, front, back, push/pop, resize, …)    |
| `rtti/TArrayType.hpp`     | `TArrayType<T>` — implements `IArrayType` over `std::vector<T>`                                  |
| `rtti/IClassType.hpp`     | `IClassType : IType` — tag class for class/struct types (minimally implemented)                  |
| `rtti/Iterator.hpp`       | `Iterator<T>` and `ReverseIterator<T>` — type-erased (T=void) or typed iterator pair             |
| `rtti/TypeRegistry.hpp`   | `TypeRegistry` — singleton, thread-safe `IName → unique_ptr<IType>` map                          |
| `rtti/TypeRegistrar.hpp`  | `TypeRegistrar` — **[stub, not yet implemented]**                                                |
| `rtti/types/IntType.hpp`  | `IntType : TType<int>` and `IntArrayType : TArrayType<int>` — reference concrete implementations |

### `core/src/`

Mirrors the `include/` structure. Each `.cpp` provides implementations for the corresponding
header. Template-heavy code (`TType`, `TArrayType`, `Iterator`) lives entirely in headers.

### `tests/`

| File                           | What it tests                                                  |
|--------------------------------|----------------------------------------------------------------|
| `INameTests.cpp`               | `IName` construction, hashing, equality, conversions           |
| `INamePoolTests.cpp`           | `INamePool` singleton, `addName`, `getName`, `hasName`         |
| `IntTypeTests.cpp`             | `IntType` lifecycle, assign, equals                            |
| `IntArrayTypeTests.cpp`        | `IntArrayType` / `TArrayType<int>` full API                    |
| `TypeRegistryTests.cpp`        | `TypeRegistry` singleton, register, lookup, thread-safety      |
| `RttiIteratorTests.cpp`        | `Iterator<>` (void) and `Iterator<int>` arithmetic, comparison |
| `RttiReverseIteratorTests.cpp` | `ReverseIterator<>` arithmetic, comparison                     |
| `RttiTestUtils.hpp`            | Shared test helpers (no test cases)                            |

---

## 4. Core Subsystems

### 4.1 Interned Names (`IName` / `INamePool`)

- Strings are interned by hashing them with **FNV-1a 64-bit**.
- `IName` is just a `hash_t` value — cheap to copy, compare, and use as a map key.
- `INamePool::get()` is the singleton. Call `addName(str)` to intern; `getName(hash)` to resolve.
- **Important:** Constructing an `IName` from a string does **not** automatically intern it in the
  pool. You must call `INamePool::get().addName(str)` explicitly if you need resolution to work.
- `IName` is hashable via `std::hash<core::IName>` (specialised in `IName.hpp`).

### 4.2 RTTI Type Hierarchy

```
IType  (abstract — name, size, alignment, kind; assign/create/free/construct/destroy/equals)
├── TType<T>           — generic non-container implementation (all ops via C++ semantics on T)
│   └── IntType        — TType<int>, kind=SIMPLE
├── IClassType         — tag subclass, kind=CLASS (fields/methods TBD)
├── IContainerType     — adds inner() pointer
│   └── IArrayType     — full std::vector-style API (abstract)
│       └── TArrayType<T> — concrete impl over std::vector<T>
│           └── IntArrayType — TArrayType<int>
```

**TypeKind** values:

- `SIMPLE` — primitives (int, float, …)
- `CLASS` — user-defined structs/classes
- `ARRAY` — dynamic array container
- `STRING` — string type (not yet implemented concretely)
- `NAME` — interned string type (not yet implemented concretely)

### 4.3 Type Registry

- Global singleton: `TypeRegistry::get()`.
- `registerType(unique_ptr<IType>&&)` — takes ownership; **returns false and discards** if name
  already exists (no overwrite).
- `getType(IName)` — returns raw `IType*` (non-owning).
- `getClass(IName)` — convenience that `dynamic_cast`s to `IClassType*`.
- `hasType(IName)` — existence check.
- Thread-safe: uses `std::shared_mutex` (shared for reads, exclusive for writes).

### 4.4 Iterators

`Iterator<T>` / `ReverseIterator<T>` are **not** STL iterators in the strict sense:

- `operator[]` returns `void*` (not a reference).
- `operator*` is absent; use `.get()` or the implicit conversion to `T*`.
- When `T = void` (type-erased mode), pointer arithmetic uses `IArrayType::inner()->size()`.
- `ReverseIterator` wraps a forward `Iterator`; `.get()` returns one-before-the-wrapped-pos.

### 4.5 SpinLock

Reader-writer spin lock. Satisfies `BasicLockable` (exclusive) and has `lock_shared` / `unlock_shared`.
Currently not used by `TypeRegistry` (which uses `std::shared_mutex`).

---

## 5. Naming Conventions & Style

| Entity                        | Convention                    | Example                                         |
|-------------------------------|-------------------------------|-------------------------------------------------|
| Types / classes               | `PascalCase`                  | `IArrayType`, `TType`, `TypeRegistry`           |
| Interfaces (abstract/virtual) | `I` prefix                    | `IType`, `IArrayType`, `IClassType`             |
| Template wrappers             | `T` prefix                    | `TType<T>`, `TArrayType<T>`                     |
| Member variables              | `m_` prefix, `camelCase`      | `m_name`, `m_arrayType`                         |
| Functions / methods           | `camelCase`                   | `registerType()`, `pushBack()`, `shrinkToFit()` |
| Namespaces                    | `snake_case`                  | `core`, `core::rtti`                            |
| Test tags                     | `[subsystem][area]`           | `[rtti][type_registry][thread_safety]`          |
| Test naming                   | Gherkin-style full sentence   | `"Given X, when Y, then Z"`                     |
| Header guards                 | `#pragma once` (no `#ifndef`) | —                                               |
| Implementation in headers     | OK for templates              | `TType.hpp`, `TArrayType.hpp`, `Iterator.hpp`   |

Code style broadly follows **Google C++ style** (braces on same line, 2-space indent, `[[nodiscard]]`
on non-void getters, `noexcept` on non-throwing functions).

All new .hpp files go in `core/include/` (or a relevant subdirectory).
All new .cpp implementation files go in `core/src/` (matching subdirectory).
New types added to the library must also be listed in `core/CMakeLists.txt`.

---

## 6. Build System

### CMake Targets

| Target                | Type         | Description        |
|-----------------------|--------------|--------------------|
| `jaybird-engine-core` | `STATIC` lib | Core library       |
| `core-tests`          | Executable   | Catch2 test runner |

### Build Configurations

| Config         | Directory                     | Notes                                                         |
|----------------|-------------------------------|---------------------------------------------------------------|
| Debug          | `cmake-build-debug/`          | Standard debug build                                          |
| Release        | `cmake-build-release/`        | Standard release build                                        |
| Debug+Coverage | `cmake-build-debug-coverage/` | Requires Clang; `-fprofile-instr-generate -fcoverage-mapping` |

Enable coverage: pass `-DENABLE_COVERAGE=ON` to CMake (only activates for Clang).

### Adding a New Source File

1. Create `core/src/rtti/MyThing.cpp` (and `core/include/rtti/MyThing.hpp`).
2. Add both paths to the `add_library(jaybird-engine-core ...)` call in `core/CMakeLists.txt`.
3. If it is a template-only file, it goes in `include/` only (both `.hpp` declaration and
   implementation).

### Running Tests

```powershell
# From the repo root (adjust path for your config)
cmake --build cmake-build-debug --target core_tests
ctest --test-dir cmake-build-debug -V
```

---

## 7. Testing

- Framework: **Catch2 v3.5.4** (fetched via `FetchContent`).
- Linked with `Catch2::Catch2WithMain` (no custom `main()`).
- Tests discovered via `catch_discover_tests(core_tests)` / CTest.

### Test Writing Rules

- One `TEST_CASE` per behavior. Give it a full sentence description, e.g.: `"Given X, when Y, then Z"`.
- Tags must include the subsystem (`[rtti]`) and optionally `[thread_safety]` or `[negative]`.
- Use anonymous namespaces for test-local helpers and stubs.
- Prefer using `std::make_unique` when constructing type descriptors for the registry (they take
  ownership).
- For concurrency tests: use `std::atomic` flags and `std::thread`; join all threads before
  asserting.
- For unique names in registry tests: use a counter-based `uniqueName(prefix)` helper to avoid
  cross-test pollution (registry is a singleton that persists across the test binary run).

### Adding a New Test File

1. Create `tests/MyThingTests.cpp`.
2. Add it to `add_executable(core_tests ...)` in `tests/CMakeLists.txt`.

---

## 8. Key Design Patterns

### Type-erased operations via `void*`

All `IType` virtual methods accept `void*` / `const void*` for instances. The concrete type (e.g.,
`TType<int>`) knows the real type and `static_cast`s internally. Callers must guarantee that the
pointer points to a correctly-typed object.

### Singleton pattern

Both `INamePool` and `TypeRegistry` use a private constructor + `static ... get()` returning a
static local instance (or pointer to one). Neither is copyable nor movable.

### Concept-constrained template construction

`TArrayType<T>` requires its `inner` constructor argument to satisfy the `TypedInnerDescriptorFor`
concept — i.e., the inner descriptor must derive from `IType` and expose a `::Type` alias equal to
`T`. This catches mismatched element type descriptors at compile time.

### Array type names use a prefix

Array types are named `"array:<inner-type-name>"`. The helper `typePrefix<TypeKind::ARRAY>(name)`
in `IArrayType.hpp` constructs this string. (Marked `TODO: move these`.)

### `asArray()` on `IType`

`IType::asArray()` returns `IArrayType*` if the type is an array kind (checked via `TypeKind`),
otherwise `nullptr`. This avoids needing `dynamic_cast` for the common array-check case.

---

## 9. What Exists vs. What Is Planned

| Feature                                           | Status                     |
|---------------------------------------------------|----------------------------|
| `IName` / `INamePool` — interned strings          | ✅ Complete                 |
| `SpinLock`                                        | ✅ Complete                 |
| `IType` / `TType<T>`                              | ✅ Complete                 |
| `IContainerType` / `IArrayType` / `TArrayType<T>` | ✅ Complete                 |
| `Iterator<T>` / `ReverseIterator<T>`              | ✅ Complete                 |
| `IClassType`                                      | ✅ Stub (constructor only)  |
| `TypeRegistry`                                    | ✅ Complete                 |
| `TypeRegistrar`                                   | 🔲 Stub (empty class body) |
| Concrete `StringType`                             | 🔲 Not started             |
| Concrete `NameType`                               | 🔲 Not started             |
| `IClassType` fields/properties/methods            | 🔲 Not started             |
| Serialisation / reflection helpers                | **[PLACEHOLDER]**          |
| Scripting / scripting bridge                      | **[PLACEHOLDER]**          |

---

## 10. Important Constraints & Gotchas

1. **`TypeRegistry` is a singleton and persists for the whole test binary run.** Tests that
   register types must use unique names (counter-based) to avoid collisions.

2. **`IName` construction does not intern.** Call `INamePool::get().addName(str)` explicitly if
   you need `IName::toString()` / `INamePool::get().getName(name)` to work.

3. **`TArrayType<T>` underlying type is `std::vector<T>`.** All void* pointers passed to its
   methods must actually point to `std::vector<T>` instances.

4. **`Iterator<void>` pointer arithmetic** uses `m_arrayType->inner()->size()` — the `IArrayType*`
   must therefore be non-null when constructing a void iterator.

5. **`TypeRegistrar` is not yet implemented** — do not use it or assume it will auto-register
   types at startup.

6. **Header `include/rtti/TypeRegistrar.cpp`** is listed in `CMakeLists.txt` under `include/` —
   this is intentional (header-only or near-header implementation pattern used elsewhere).

7. **Coverage build requires Clang** — the flag guard is `CMAKE_CXX_COMPILER_ID MATCHES "Clang"`.

8. **C++23 required.** Uses `std::construct_at`, `std::destroy_at`, concepts, and other C++20/23
   features freely.

---

## 11. Placeholders Needing Owner Input

Please fill in the items below and remove this section once complete.

| #   | Question                                                                                                             |
|-----|----------------------------------------------------------------------------------------------------------------------|
| P1  | Primary compiler for non-coverage builds: MSVC, Clang-cl, or something else?                                         |
| P2  | What is the `app/` target intended to do (sandbox, demo, full game loop)?                                            |
| P3  | Is `TypeRegistrar` meant to be a static auto-registration helper (similar to a factory registry), or something else? |
| P4  | What types beyond `int` / `int[]` are planned next (float, string, user-defined classes)?                            |
| P5  | Is there a scripting layer planned (Lua, Python, custom), and will RTTI feed into it?                                |
| P6  | Are there plans for serialisation (binary, JSON, etc.) and will RTTI drive it?                                       |
| P7  | What is the long-term scope of the engine (2D, 3D, ECS, other)?                                                      |
| P8  | Are there any third-party libraries planned beyond Catch2 (e.g., rendering, windowing, math)?                        |
| P9  | What is the target release/milestone timeline (if any)?                                                              |
| P10 | Is `SpinLock` intended to replace `std::shared_mutex` in `TypeRegistry`, or do they serve different purposes?        |

