include(FetchContent)

FetchContent_Declare(
        Catch2
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.5.4.zip
)

FetchContent_MakeAvailable(Catch2)

add_executable(core_tests
        INameTests.cpp
        INamePoolTests.cpp
        IntTypeTests.cpp
        IntArrayTypeTests.cpp
        TypeRegistryTests.cpp
        RttiIteratorTests.cpp
        RttiReverseIteratorTests.cpp
)

target_include_directories(core_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/core/include
        ${PROJECT_SOURCE_DIR}/tests
)

target_link_libraries(core_tests PRIVATE
        Catch2::Catch2WithMain
        jaybird-engine-core
)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(core_tests)

