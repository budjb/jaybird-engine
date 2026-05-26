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
12. [Code Docs](#12-code-docs)

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
├── tests/         ← Catch2 test executable: core-tests
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
- `IName` is hashable via `std::hash<core::IName>` (specialized in `IName.hpp`).

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
cmake --build cmake-build-debug --target core-tests
ctest --test-dir cmake-build-debug -V
```

### Windows MSVC Shell Requirement

On Windows, agent-run `cmake`/build/test commands that target MSVC must run from a Visual Studio developer environment.
If standard headers such as `cstdint`, `memory`, `string`, or `functional` appear as missing, initialize the shell with
`VsDevCmd.bat` first and then run CMake/build commands in that same process.

```powershell
$repoRoot = $PWD.Path
$buildDir = Join-Path $repoRoot "cmake-build-debug-vsenv"
$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
$vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$vsDevCmd = Join-Path $vsPath "Common7\Tools\VsDevCmd.bat"
cmd /c '"' + $vsDevCmd + '" -arch=x64 -host_arch=x64 >nul && cmake -S "' + $repoRoot + '" -B "' + $buildDir + '" -G Ninja && cmake --build "' + $buildDir + '" --target core-tests'
```

Use the existing `cmake-build-debug-vsenv/` directory (or a similarly named build directory) for MSVC-based agent runs.

---

## 7. Testing

- Framework: **Catch2 v3.5.4** (fetched via `FetchContent`).
- Linked with `Catch2::Catch2WithMain` (no custom `main()`).
- Tests discovered via `catch_discover_tests(core-tests)` / CTest.

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
2. Add it to `add_executable(core-tests ...)` in `tests/CMakeLists.txt`.

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

## 12. Code Docs

Code is documented with Doxygen-style comments. Public API in `core/include/` should have full doc comments; internal
implementation files in `core/src/` may have minimal or no comments, but should still be reasonably clear. Use
multi-line comments with `/** ... */` for all classes, functions, or properties. Do not consolidate these documents into
single lines. Additionally, descriptions must begin with `@brief`. For example:

### Line Wrapping

The project is configured with clang-format to enforce a maximum line width of 120 characters. Doc comment text must
wrap at **semantic boundaries** — that is, at natural end points of sentences, clauses, or logical thought units — and
must never exceed 120 characters. The goal is readability: keep related ideas on the same line when possible, and wrap
only when introducing a new sentence, clause, or major concept.

**Critical rule**: Never break `@c token` or `@code token(s)@endcode` constructs across lines. These formatting
elements must stay together on a single line. If a code token would be split across the boundary, reflow the preceding
text or move the token to the next line.

**Guidelines**:

- If a sentence or clause fits within 120 characters, keep it on one line.
- Wrap only when a new sentence begins or a new clause starts.
- Do not wrap mid-sentence just because there is space left on the line.
- Prefer shorter, punchier sentences over long wrapped ones.
- Review your line endings: if wrapping feels awkward, consider rephrasing for clarity.

For example:

```cpp
/**
 * @brief This is a longer description that wraps naturally at sentence boundaries.
 *
 * This text includes references like @c MyClass and @code myFunction()@endcode; they stay intact on one line.
 * When a new sentence begins, wrap to the next line. This keeps ideas grouped logically.
 */

/**
 * @brief This is good.
 */

/**
 * This is bad.
 */

/** @brief This is bad too. */

// This is good: each sentence is on a single line, and we wrap only when a new sentence starts.
/**
 * @brief Casts a pointer to the derived @code IArrayType@endcode interface.
 *
 * Serves mostly as a documentation helper.
 *
 * @param arrayType The pointer to cast.
 * @return The same pointer, explicitly typed as @code IArrayType*@endcode.
 */

// This is bad: sentences are split arbitrarily mid-clause.
/**
 * @brief Casts a type descriptor pointer to an @code IArrayType@endcode
 *   pointer.
 *
 * This function is a convenience helper that ensures type safety when
 *   casting a raw pointer to an @code IArrayType@endcode.
 *
 * @param arrayType This parameter is a pointer to an @code IArrayType@endcode to cast.
 * @return This function explicitly returns the pointer as
 *   @code IArrayType*@endcode.
 */
```

Every function, constructor, and conversion operator must document all the following that apply:

- **`@param name`** — one tag per parameter, for every parameter without exception.
- **`@return`** — required for every non-`void` function and every conversion operator.
- **`@tparam Name`** — one tag per template parameter, for every template parameter without exception.
- **`@throws ExceptionType`** — when applicable.

These are not optional even for trivial or self-evident cases; they must always be present. For complex or non-obvious
behaviors, elaborate on specifics and gotchas, but do not introduce verbosity by narrating the implementation where the
code itself is easily followed.

The goal of the brief is to provide a one- or two-sentence summary of the entity's purpose. The rest of the comment can
then expand on details, constraints, and usage examples as needed. We are aiming for balancing brevity with clarity.
However, **every sentence in a doc comment — in the `@brief`, in body paragraphs, and in tag descriptions — must be
proper, complete English prose with a subject and predicate.** Sentence fragments, noun-phrase-only sentences, and
adjective-dangling openers such as "Thread-safe." or "Satisfies X." are not acceptable. Write "It is thread-safe." or
"This class satisfies X." instead.

Where relevant, include example usage snippets in the comments.

### Code Formatting Rule: `@c` vs. `@code`

Use code formatting for types, parameters, important terms, and literals (e.g., keywords, enum values). This rule
applies **everywhere** in a doc comment: in the `@brief`, in body paragraphs, and in all tag descriptions (`@param`,
`@return`, `@tparam`, `@throws`).

**The Rule**: Use `@c term` for all inline code formatting, **except** when a code token is the **last grammatical
element in a clause or sentence** and is immediately followed by punctuation. In that case, use `@code term@endcode`
instead so that punctuation renders outside the formatted span.

A code token is considered the "last element" if nothing other than sentence-ending or clause-ending punctuation
(`.`, `!`, `?`, `,`, `;`, or `)`) immediately follows it.

**Examples**:

```cpp
/**
 * @brief Gets the @c IType descriptor for the given @c name.
 *   // "name" is followed by a period, but it is not a code token — fine.
 * @brief Returns a pointer to the @code IType@endcode.
 *   // "IType" is a code token at sentence end — must use @code...@endcode.
 *
 * @param func The function pointer (of type @code FuncPtr@endcode).
 *   // "FuncPtr" is a code token before closing paren — must use @code...@endcode.
 *
 * @param name The name of the descriptor (e.g., @c "Vector3D").
 *   // "Vector3D" is quoted text, not the last element — @c is fine.
 *
 * @return A vector of @code int@endcode values.
 *   // "int" is a code token at sentence end — must use @code...@endcode.
 *
 * Accepts instances of type @code T@endcode, where @c T must satisfy the @code Drawable@endcode concept.
 *   // "T" at clause end: @code...@endcode.
 *   // "Drawable" at end of sentence: @code...@endcode.
 *   // "concept" is plain text after "Drawable" — but no, word ends at Drawable.
 *   // Actually, read it as: "where T (must satisfy Drawable concept)" — so Drawable is not at sentence end.
 *   // Let me reconsider: "satisfies the X concept" — X is before "concept", not at sentence end.
 *   // So: @c Drawable (concept) is fine, or we could say (of type @code Drawable@endcode) if Drawable alone ended.
 *
 * Serializes to a buffer of size @code N@endcode bytes.
 *   // "N" is a code token before whitespace then period — the period follows "bytes", not "N".
 *   // Actually, re-read: "...size @code N@endcode bytes." — "bytes" ends the sentence.
 *   // So this is correct usage of @code...@endcode for N because nothing follows N except whitespace.
 */
```

**Clarification on punctuation-adjacent tokens**: The rule triggers when a code token has **no other text** between
itself and sentence-ending punctuation. Whitespace does not count. For example:

- `@c N.` (period directly follows) → use `@code N@endcode.`
- `@c N,` (comma directly follows) → use `@code N@endcode,`
- `@c N)` (closing paren directly follows) → use `@code N@endcode)`
- `@c N bytes.` (plain text "bytes" follows the token) → `@c N` is fine (bytes ends the sentence)

**Scope**: This rule applies in the `@brief`, in extended description paragraphs, in `@param` tag descriptions, in
`@return` tag descriptions, in `@tparam` tag descriptions, and in `@throws` descriptions. There are no exceptions.

### Prose Quality: Complete Sentences Everywhere

Every sentence in every doc comment — including the `@brief`, body paragraphs, and tag descriptions — must be a proper,
complete English sentence with a clear subject and a predicate. There are no exceptions, not even in tag descriptions or
when the meaning seems obvious. Additionally, all sentences should target lengths of less than 40 words. This is not an
absolute rule, but a guideline: if a sentence exceeds 40 words, consider breaking it into multiple sentences for
improved clarity and readability. The goal is to avoid overwhelming readers with overly long, complex clauses.

**Unacceptable (fragments)**:

- "Thread-safe." ← Fragment (no subject or predicate)
- "Satisfies @code BasicLockable@endcode." ← Fragment (noun phrase + action, but no subject pronoun)
- "Maps hashes to strings." ← Fragment (participle phrase, no main subject)

**Acceptable (complete sentences)**:

- "It is thread-safe." ← Subject ("It") + predicate ("is thread-safe")
- "This class satisfies @code BasicLockable@endcode." ← Subject ("This class") + predicate ("satisfies")
- "Hashes are mapped to strings internally." ← Complete passive sentence
- "The registry maps hashes to strings." ← Complete active sentence

This rule applies in:

- `@brief` tags
- Body paragraphs (extended descriptions)
- `@param` tag descriptions (e.g., "the value of the @c X parameter; must not be null")
- `@return` tag descriptions (e.g., "the new size of the container after insertion")
- `@tparam` tag descriptions
- `@throws` tag descriptions
- Any explanatory text within a doc comment

If you find yourself wanting to write a label or noun phrase, add a subject and verb to make it a complete thought.
For example, instead of "The buffer size", write "The size of the buffer is N bytes" or "Returns the buffer size in
bytes."
