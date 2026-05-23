# Unit Testing with Catch2

This project uses Catch2 (v3) for its test framework.

## Configure

```powershell
cmake -S . -B cmake-build-tests -DBUILD_TESTING=ON
```

To configure with coverage instrumentation (GCC/Clang):

```powershell
cmake -S . -B cmake-build-tests-coverage -DBUILD_TESTING=ON -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
```

## Build Tests

```powershell
cmake --build cmake-build-tests --config Debug --target core_tests
```

Or build everything in one go:

```powershell
cmake --build cmake-build-tests --config Debug
```

## Run Tests

```powershell
ctest --test-dir cmake-build-tests -C Debug --output-on-failure
```

Coverage build run:

```powershell
cmake --build cmake-build-tests-coverage --target core_tests
ctest --test-dir cmake-build-tests-coverage --output-on-failure
```

## CLion Coverage Notes

- Use a toolchain backed by GCC or Clang when running with `-DENABLE_COVERAGE=ON`.
- Create a CMake profile in CLion with `BUILD_TESTING=ON`, `ENABLE_COVERAGE=ON`, and `CMAKE_BUILD_TYPE=Debug`.
- Run the `core_tests` Catch2 target from that profile, then use **Run with Coverage**.
- CLion reads generated coverage data from the coverage-instrumented build directory and overlays results in the editor.

## Notes

- Tests are auto-discovered with `catch_discover_tests` in `tests/CMakeLists.txt`.
- All test files are consolidated into the `core_tests` binary target.
- RTTI iterator coverage is split into `tests/RttiIteratorTests.cpp` and `tests/RttiReverseIteratorTests.cpp`.


