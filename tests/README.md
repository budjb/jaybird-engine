# Unit Testing with Catch2

This project uses Catch2 (v3) for its test framework.

## Configure

```powershell
cmake -S . -B cmake-build-tests -DBUILD_TESTING=ON
```

## Build Tests

```powershell
cmake --build cmake-build-tests --config Debug --target iterator_tests
cmake --build cmake-build-tests --config Debug --target vector_tests
```

Or build everything in one go:

```powershell
cmake --build cmake-build-tests --config Debug
```

## Run Tests

```powershell
ctest --test-dir cmake-build-tests -C Debug --output-on-failure
```

## Notes

- Tests are auto-discovered with `catch_discover_tests` in `tests/CMakeLists.txt`.
- Iterator coverage is in `tests/IteratorTests.cpp`.
- Vector coverage is in `tests/VectorTests.cpp`.

