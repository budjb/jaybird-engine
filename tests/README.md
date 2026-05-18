# Unit Testing with Catch2

This project uses Catch2 (v3) for its test framework.

## Configure

```powershell
cmake -S . -B cmake-build-tests -DBUILD_TESTING=ON
```

## Build Tests

```powershell
cmake --build cmake-build-tests --config Debug --target iterator_tests
```

## Run Tests

```powershell
ctest --test-dir cmake-build-tests -C Debug --output-on-failure
```

## Notes

- Tests are auto-discovered with `catch_discover_tests` in `tests/CMakeLists.txt`.
- Initial iterator coverage is in `tests/IteratorTests.cpp`.

