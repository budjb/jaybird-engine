include(FetchContent)

FetchContent_Declare(
        Catch2
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.5.4.zip
)

FetchContent_MakeAvailable(Catch2)

add_executable(iterator_tests
        IteratorTests.cpp
)

target_include_directories(iterator_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/core/include
)

target_link_libraries(iterator_tests PRIVATE
        Catch2::Catch2WithMain
)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(iterator_tests)

